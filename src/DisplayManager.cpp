#include <DisplayManager.h>
#include <FastLED_NeoMatrix.h>
#include "MatrixDisplayUi.h"
#include <TJpg_Decoder.h>
#include "icons.h"
#include "Settings.h"
#include <ArduinoJson.h>
#include "PeripheryManager.h"
#include "MQTTManager.h"
#include "GifPlayer.h"
#include "AudioManager.h"
#include <Ticker.h>
#include "Functions.h"
#include "ServerManager.h"
#include "MenuManager.h"
#include "Frames.h"

Ticker AlarmTicker;
Ticker TimerTicker;

#define MATRIX_PIN 32
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8

bool appIsSwitching;

GifPlayer gif;

CRGB leds[MATRIX_WIDTH * MATRIX_HEIGHT];
FastLED_NeoMatrix matrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

MatrixDisplayUi ui(&matrix);

DisplayManager_ &DisplayManager_::getInstance()
{
    static DisplayManager_ instance;
    return instance;
}

DisplayManager_ &DisplayManager = DisplayManager.getInstance();

void DisplayManager_::setBrightness(uint8_t bri)
{
    if (MATRIX_OFF && !ALARM_ACTIVE)
    {
        matrix.setBrightness(0);
    }
    else
    {
        matrix.setBrightness(bri);
    }
}

void DisplayManager_::setFPS(uint8_t fps)
{
    ui.setTargetFPS(fps);
}

void DisplayManager_::setTextColor(uint16_t color)
{
    matrix.setTextColor(color);
}

void DisplayManager_::MatrixState(bool on)
{
    MATRIX_OFF = !on;
    setBrightness(BRIGHTNESS);
}

void DisplayManager_::disableAutoTransition()
{
    ui.disableAutoTransition();
}

void DisplayManager_::enableAutoTransition()
{
    ui.enableAutoTransition();
}

void DisplayManager_::drawGIF(uint16_t x, uint16_t y, fs::File gifFile)
{
    gif.setFile(gifFile);
    gif.drawFrame(x, y);
}

void DisplayManager_::drawJPG(uint16_t x, uint16_t y, fs::File jpgFile)
{
    TJpgDec.drawFsJpg(x, y, jpgFile);
}

void DisplayManager_::setSettings()
{
    ui.setTargetFPS(MATRIX_FPS);
    ui.setTimePerApp(TIME_PER_FRAME);
    ui.setTimePerTransition(TIME_PER_TRANSITION);
}

void DisplayManager_::resetTextColor()
{
    matrix.setTextColor(TEXTCOLOR_565);
}

void DisplayManager_::clearMatrix()
{
    matrix.clear();
    matrix.show();
}

bool jpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    uint16_t bitmapIndex = 0;

    for (uint16_t row = 0; row < h; row++)
    {
        for (uint16_t col = 0; col < w; col++)
        {
            uint16_t color = bitmap[bitmapIndex++];
            uint8_t r = ((color & 0xF800) >> 11) << 3;
            uint8_t g = ((color & 0x07E0) >> 5) << 2;
            uint8_t b = (color & 0x001F) << 3;
            matrix.drawPixel(x + col, y + row, matrix.Color(r, g, b));
        }
    }
    return 0;
}

void DisplayManager_::printText(int16_t x, int16_t y, const char *text, bool centered)
{
    if (centered)
    {
        uint16_t textWidth = getTextWidth(text);
        int16_t textX = ((32 - textWidth) / 2);
        matrix.setCursor(textX, y);
    }
    else
    {
        matrix.setCursor(x, y);
    }

    if (UPPERCASE_LETTERS)
    {
        size_t length = strlen(text);
        char upperText[length + 1]; // +1 for the null terminator

        for (size_t i = 0; i < length; ++i)
        {
            upperText[i] = toupper(text[i]);
        }

        upperText[length] = '\0'; // Null terminator
        matrix.print(upperText);
    }
    else
    {
        matrix.print(text);
    }
}

void DisplayManager_::HSVtext(int16_t x, int16_t y, const char *text, bool clear)
{
    if (clear)
        matrix.clear();
    static uint8_t hueOffset = 0;
    uint16_t xpos = 0;
    for (uint16_t i = 0; i < strlen(text); i++)
    {
        uint8_t hue = map(i, 0, strlen(text), 0, 255) + hueOffset;
        uint32_t textColor = hsvToRgb(hue, 255, 255);
        matrix.setTextColor(textColor);
        const char *myChar = &text[i];

        matrix.setCursor(xpos + x, y);
        if (UPPERCASE_LETTERS)
        {
            matrix.print((char)toupper(text[i]));
        }
        else
        {
            matrix.print(&text[i]);
        }
        char temp_str[2] = {'\0', '\0'};
        temp_str[0] = text[i];
        xpos += getTextWidth(temp_str);
    }
    hueOffset++;
    if (clear)
        matrix.show();
}

void pushCustomFrame(uint16_t id)
{
    if (customFrames.count(id) == 0)
    {
        uint16_t newID = nativeAppsCount + id;
        switch (id)
        {
        case 1:
            Apps.push_back(std::make_pair(newID, CFrame1));
            break;
        case 2:
            Apps.push_back(std::make_pair(newID, CFrame2));
            break;
        case 3:
            Apps.push_back(std::make_pair(newID, CFrame3));
            break;
        case 4:
            Apps.push_back(std::make_pair(newID, CFrame4));
            break;
        case 5:
            Apps.push_back(std::make_pair(newID, CFrame5));
            break;
        case 6:
            Apps.push_back(std::make_pair(newID, CFrame6));
            break;
        case 7:
            Apps.push_back(std::make_pair(newID, CFrame7));
            break;
        case 8:
            Apps.push_back(std::make_pair(newID, CFrame8));
            break;
        case 9:
            Apps.push_back(std::make_pair(newID, CFrame9));
            break;
        case 10:
            Apps.push_back(std::make_pair(newID, CFrame10));
            break;
        default:
            return;
            break;
        }
        ui.setApps(Apps); // Add frames
    }
}

void removeCustomFrame(uint16_t id)
{
    id += nativeAppsCount;
    // Suchen Sie nach dem Element, das der ID entspricht
    auto it = std::find_if(Apps.begin(), Apps.end(), [id](const std::pair<uint16_t, AppCallback> &appPair)
                           { return appPair.first == id; });

    // Wenn das Element gefunden wurde, entfernen Sie es aus dem Vektor
    if (it != Apps.end())
    {
        Apps.erase(it);
        ui.setApps(Apps); // Aktualisieren Sie die Frames
    }
}

void DisplayManager_::generateCustomPage(uint16_t id, String payload)
{

    if (payload == "" && customFrames.count(id))
    {
        customFrames.erase(customFrames.find(id));
        removeCustomFrame(id);
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    if (error)
        return;

    CustomFrame customFrame;

    if (id == 0)
    {
        if (doc.containsKey("id"))
        {
            customFrame.id = doc["id"].as<uint8_t>();
        }
        else
        {
            return;
        }
    }

    if (id > 10)
        return;

    if (doc.containsKey("sound"))
    {
        customFrame.sound = ("/" + doc["sound"].as<String>() + ".txt");
    }
    else
    {
        customFrame.sound = "";
    }

    if (doc.containsKey("name"))
    {
        customFrame.name = doc["name"].as<String>();
    }
    else
    {
        customFrame.name = "Custom " + String(id);
    }

    customFrame.rainbow = doc.containsKey("rainbow") ? doc["rainbow"] : false;
    customFrame.pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    customFrame.id = id;
    customFrame.text = utf8ascii(doc["text"].as<String>());

    customFrame.color = doc.containsKey("color") ? doc["color"].is<String>() ? hexToRgb565(doc["color"]) : doc["color"].is<JsonArray>() ? hexToRgb565(doc["color"].as<String>())
                                                                                                                                        : TEXTCOLOR_565
                                                 : TEXTCOLOR_565;

    if (currentCustomFrame != id)
    {
        customFrame.scrollposition = 34;
    }

    customFrame.repeat = doc.containsKey("repeat") ? doc["repeat"].as<uint8_t>() : -1;

    if (doc.containsKey("icon"))
    {
        String iconFileName = String(doc["icon"].as<String>());

        if (LittleFS.exists("/ICONS/" + iconFileName + ".jpg"))
        {
            customFrame.isGif = false;
            customFrame.icon = LittleFS.open("/ICONS/" + iconFileName + ".jpg");
        }
        else if (LittleFS.exists("/ICONS/" + iconFileName + ".gif"))
        {
            customFrame.isGif = true;
            customFrame.icon = LittleFS.open("/ICONS/" + iconFileName + ".gif");
        }
    }

    pushCustomFrame(id);
    customFrames[id] = customFrame;
}

void DisplayManager_::generateNotification(String payload)
{
    StaticJsonDocument<128> doc;
    deserializeJson(doc, payload);

    notify.duration = doc.containsKey("duration") ? doc["duration"].as<int>() * 1000 : TIME_PER_FRAME;
    notify.text = utf8ascii(doc["text"].as<String>());
    notify.repeat = doc.containsKey("repeat") ? doc["repeat"].as<uint16_t>() : -1;
    notify.rainbow = doc.containsKey("rainbow") ? doc["rainbow"].as<bool>() : false;
    notify.hold = doc.containsKey("hold") ? doc["hold"].as<bool>() : false;
    notify.pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    notify.flag = true;
    notify.startime = millis();
    notify.scrollposition = 34;
    notify.iconWasPushed = false;
    notify.iconPosition = 0;

    if (doc.containsKey("sound"))
    {
        PeripheryManager.playFromFile("/MELODIES/" + doc["sound"].as<String>() + ".txt");
    }

    notify.color = doc.containsKey("color") ? doc["color"].is<String>() ? hexToRgb565(doc["color"]) : doc["color"].is<JsonArray>() ? hexToRgb565(doc["color"].as<String>())
                                                                                                                                   : TEXTCOLOR_565
                                            : TEXTCOLOR_565;

    if (doc.containsKey("icon"))
    {
        String iconFileName = doc["icon"].as<String>();
        if (LittleFS.exists("/ICONS/" + iconFileName + ".jpg"))
        {
            notify.isGif = false;
            notify.icon = LittleFS.open("/ICONS/" + iconFileName + ".jpg");
        }
        else if (LittleFS.exists("/ICONS/" + iconFileName + ".gif"))
        {
            notify.isGif = true;
            notify.icon = LittleFS.open("/ICONS/" + iconFileName + ".gif");
        }
    }
    else
    {
        File f;
        notify.icon = f;
    }
}

void DisplayManager_::loadApps()
{
    Apps.clear();
    Apps.push_back(std::make_pair(0, TimeFrame));
    if (SHOW_DATE)
        Apps.push_back(std::make_pair(1, DateFrame));
    if (SHOW_TEMP)
        Apps.push_back(std::make_pair(2, TempFrame));
    if (SHOW_HUM)
        Apps.push_back(std::make_pair(3, HumFrame));
    if (SHOW_BATTERY)
        Apps.push_back(std::make_pair(4, BatFrame));
    // if (SHOW_WEATHER)
    //     Apps.push_back(std::make_pair(5, WeatherFrame));
    nativeAppsCount = Apps.size();
    ui.setApps(Apps); // Add frames
    StartAppUpdater();
}

void DisplayManager_::setup()
{
    TJpgDec.setCallback(jpg_output);
    FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
    gif.setMatrix(&matrix);
    ui.setAppAnimation(SLIDE_DOWN);
    ui.setOverlays(overlays, 4);
    ui.init();
}

void DisplayManager_::tick()
{
    if (AP_MODE)
    {
        HSVtext(2, 6, "AP MODE", true);
    }
    else
    {
        ui.update();
        if (ui.getUiState()->frameState == IN_TRANSITION && !appIsSwitching)
        {
            appIsSwitching = true;
            MQTTManager.setCurrentApp(CURRENT_APP);
        }
        else if (ui.getUiState()->frameState == FIXED && appIsSwitching)
        {
            appIsSwitching = false;
            MQTTManager.setCurrentApp(CURRENT_APP);
        }
    }
}

void DisplayManager_::leftButton()
{
    if (!MenuManager.inMenu)
        ui.previousApp();
}

void DisplayManager_::rightButton()
{
    if (!MenuManager.inMenu)
        ui.nextApp();
}

void DisplayManager_::nextApp()
{
    if (!MenuManager.inMenu)
        ui.nextApp();
}

void DisplayManager_::previousApp()
{
    if (!MenuManager.inMenu)
        ui.previousApp();
}

void snozzeTimerCallback()
{
    ALARM_ACTIVE = true;
    AlarmTicker.detach();
}

void DisplayManager_::selectButton()
{
    if (!MenuManager.inMenu)
    {
        if (notify.flag && notify.hold)
        {
            DisplayManager.getInstance().dismissNotify();
        }
        if (ALARM_ACTIVE && SNOOZE_TIME > 0)
        {
            PeripheryManager.stopSound();
            ALARM_ACTIVE = false;
            AlarmTicker.once(SNOOZE_TIME * 60, snozzeTimerCallback);
        }

        if (TIMER_ACTIVE)
        {
            PeripheryManager.stopSound();
            TIMER_ACTIVE = false;
        }
    }
}

void DisplayManager_::selectButtonLong()
{
    if (ALARM_ACTIVE)
    {
        PeripheryManager.stopSound();
        ALARM_ACTIVE = false;
    }
}

void DisplayManager_::dismissNotify()
{
    notify.hold = false;
    notify.flag = false;
}

void timerCallback()
{
    TIMER_ACTIVE = true;
    TimerTicker.detach();
}

void DisplayManager_::gererateTimer(String Payload)
{
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, Payload);
    if (error)
        return;
    int hours = doc["hours"] | 0;
    int minutes = doc["minutes"] | 0;
    int seconds = doc["seconds"] | 0;
    TIMER_SOUND = doc.containsKey("sound") ? doc["sound"].as<String>() : "";
    time_t now = time(nullptr);
    struct tm futureTimeinfo = *localtime(&now);
    futureTimeinfo.tm_hour += hours;
    futureTimeinfo.tm_min += minutes;
    futureTimeinfo.tm_sec += seconds;
    time_t futureTime = mktime(&futureTimeinfo);
    int interval = difftime(futureTime, now) * 1000;
    TimerTicker.attach_ms(interval, timerCallback);
}
