#include "Weather.h"

#ifdef AWTRIX_16X16
#include <ArduinoJson.h>
#include "DisplayManager.h"
#include "Functions.h"
#include "Globals.h"

enum WeatherIcon : uint8_t
{
    W_NONE,
    W_SUN,
    W_MOON,
    W_PARTLY,
    W_CLOUD,
    W_FOG,
    W_RAIN,
    W_SNOW,
    W_THUNDER,
};

// 8x8 icons, one character per pixel (see iconColor), kept in flash
static const char WEATHER_ICONS[][65] PROGMEM = {
    // W_SUN
    "...YY..."
    ".Y....Y."
    "...YY..."
    "Y.YYYY.Y"
    "Y.YYYY.Y"
    "...YY..."
    ".Y....Y."
    "...YY...",
    // W_MOON
    "..MMM..."
    ".MM....."
    "MM......"
    "MM......"
    "MM......"
    "MMM...M."
    ".MMMMMM."
    "..MMMM..",
    // W_PARTLY
    ".....YY."
    "....YYYY"
    "..WW.YYY"
    ".WWWW.YY"
    "WWWWWWW."
    "WWWWWWWW"
    ".WWWWWW."
    "........",
    // W_CLOUD
    "........"
    "........"
    "...GGG.."
    ".GGGGGG."
    "GGGGGGGG"
    "GGGGGGGG"
    ".GGGGGG."
    "........",
    // W_FOG
    "........"
    "GGGGGG.."
    "........"
    "..GGGGGG"
    "........"
    "GGGGGG.."
    "........"
    "..GGGGGG",
    // W_RAIN
    "...GGG.."
    ".GGGGGG."
    "GGGGGGGG"
    ".GGGGGG."
    "........"
    ".B..B..B"
    "B..B..B."
    "........",
    // W_SNOW
    "...GGG.."
    ".GGGGGG."
    "GGGGGGGG"
    ".GGGGGG."
    "........"
    "W..W..W."
    "........"
    ".W..W..W",
    // W_THUNDER
    "...DDD.."
    ".DDDDDD."
    "DDDDDDDD"
    ".DDDYDD."
    "...Y...."
    "..YYY..."
    "...Y...."
    "..Y.....",
};

// RGB888 colour of an icon character (DisplayManager.drawPixel takes 24-bit colours)
static uint32_t iconColor(char c)
{
    switch (c)
    {
    case 'Y':
        return 0xFFFF00; // yellow
    case 'M':
        return 0xFFF0D0; // pale moon yellow
    case 'W':
        return 0xFFFFFF; // white
    case 'G':
        return 0xC0C0C0; // light grey
    case 'D':
        return 0x787878; // dark grey
    case 'B':
        return 0x3870FF; // blue
    default:
        return 0;
    }
}

static bool hasWeather = false;
static int weatherTemp = 0;
static uint8_t weatherIcon = W_NONE;
static unsigned long weatherSince = 0;
static unsigned long weatherLifetime = 0; // ms, 0 = never expires

// Home Assistant weather states (sunny, clear-night, partlycloudy, cloudy, fog, rainy,
// pouring, snowy, snowy-rainy, hail, lightning, lightning-rainy, windy, ...) and a few
// common synonyms
static uint8_t iconFromCondition(String c)
{
    c.toLowerCase();
    c.trim();
    if (c.isEmpty())
        return W_NONE;
    if (c == "sunny" || c == "clear" || c == "clear-day")
        return W_SUN;
    if (c == "clear-night" || c == "night")
        return W_MOON;
    if (c.startsWith("partly"))
        return W_PARTLY;
    if (c == "fog" || c == "mist" || c == "haze")
        return W_FOG;
    if (c.indexOf("lightning") >= 0 || c.indexOf("thunder") >= 0)
        return W_THUNDER;
    if (c.indexOf("snow") >= 0 || c == "hail" || c == "sleet")
        return W_SNOW;
    if (c.indexOf("rain") >= 0 || c == "pouring" || c == "drizzle" || c == "showers")
        return W_RAIN;
    return W_CLOUD; // cloudy, windy, exceptional and anything unknown
}

// WMO weather interpretation codes, as returned by Open-Meteo
static uint8_t iconFromWmo(int code, bool day)
{
    if (code <= 1)
        return day ? W_SUN : W_MOON;
    if (code == 2)
        return W_PARTLY;
    if (code == 45 || code == 48)
        return W_FOG;
    if ((code >= 71 && code <= 77) || code == 85 || code == 86)
        return W_SNOW;
    if (code >= 95)
        return W_THUNDER;
    if (code >= 51 && code <= 82)
        return W_RAIN;
    return W_CLOUD;
}

bool weatherParser(const char *json)
{
    if (strcmp(json, "") == 0 || strcmp(json, "{}") == 0)
    {
        hasWeather = false;
        return true;
    }

    DynamicJsonDocument doc(256);
    if (deserializeJson(doc, json))
        return false;
    JsonVariant temp = doc["temp"];
    if (temp.isNull())
        return false;

    weatherTemp = lround(temp.as<float>());
    if (doc.containsKey("condition"))
    {
        weatherIcon = iconFromCondition(doc["condition"].as<String>());
    }
    else if (doc.containsKey("code"))
    {
        JsonVariant isDay = doc["is_day"];
        bool day = isDay.isNull() || (isDay.is<bool>() ? isDay.as<bool>() : isDay.as<int>() != 0);
        weatherIcon = iconFromWmo(doc["code"].as<int>(), day);
    }
    else
    {
        weatherIcon = W_NONE;
    }
    weatherLifetime = (unsigned long)(doc["lifetime"] | 7200L) * 1000UL;
    weatherSince = millis();
    hasWeather = true;
    return true;
}

bool weatherAvailable()
{
    if (!hasWeather)
        return false;
    if (weatherLifetime > 0 && millis() - weatherSince > weatherLifetime)
    {
        hasWeather = false; // stale: back to the plain clock until the next push
        return false;
    }
    return true;
}

static void drawWeatherIcon(int16_t x, int16_t y, uint8_t icon)
{
    const char *art = WEATHER_ICONS[icon - 1];
    for (uint8_t i = 0; i < 64; i++)
    {
        uint32_t color = iconColor(pgm_read_byte(art + i));
        if (color)
            DisplayManager.drawPixel(x + i % 8, y + i / 8, color);
    }
}

void drawWeather16(int16_t x, int16_t y)
{
    if (TEMP_COLOR > 0)
        DisplayManager.setTextColor(TEMP_COLOR);
    else
        DisplayManager.getInstance().resetTextColor();

    // Icon on the left 8 columns, temperature right-aligned in the right 8. The degree
    // sign is dropped when it doesn't fit; a value that doesn't fit at all (-10 and
    // below) is centred over the whole width without the icon.
    char text[8];
    snprintf(text, sizeof(text), "%d\xB0", weatherTemp);
    int width = getTextWidth(text, 2) - 1; // without the trailing gap
    if (width > 8)
    {
        snprintf(text, sizeof(text), "%d", weatherTemp);
        width = getTextWidth(text, 2) - 1;
    }

    if (weatherIcon == W_NONE || width > 8)
    {
        DisplayManager.printText(x + (MATRIX_W - width) / 2, 6 + y, text, false, 2);
        return;
    }
    drawWeatherIcon(x, y, weatherIcon);
    DisplayManager.printText(x + MATRIX_W - width, 6 + y, text, false, 2);
}
#endif
