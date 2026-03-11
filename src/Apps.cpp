#include "Apps.h"

#include <map>
#include <array>
#include "icons.h"
#include "MatrixDisplayUi.h"
#include "Functions.h"
#include "MenuManager.h"
#include "PeripheryManager.h"
#include <WiFi.h>
#include "effects.h"
#include "MQTTManager.h"
#include "Overlays.h"
#include "timer.h"

const uint8_t bigdigits_mask[12][7] = {
    {132, 48, 48, 48, 48, 48, 132},      // 0
    {204, 140, 204, 204, 204, 204, 0},   // 1
    {132, 48, 240, 196, 156, 48, 0},     // 2
    {132, 48, 240, 196, 240, 48, 132},   // 3
    {228, 196, 132, 36, 0, 228, 192},    // 4
    {0, 60, 4, 240, 240, 48, 132},       // 5
    {196, 156, 60, 4, 48, 48, 132},      // 6
    {0, 48, 240, 228, 204, 204, 204},    // 7
    {132, 48, 48, 132, 48, 48, 132},     // 8
    {132, 48, 48, 128, 240, 228, 140},   // 9
    {252, 204, 204, 252, 204, 204, 252}, // :
    {252, 252, 252, 252, 252, 252, 252}  // ; (blank)
};

uint32_t COLOR_HOUR_ON = 0xFF0000;   // Rot
uint32_t COLOR_MINUTE_ON = 0x00FF00; // Grün
uint32_t COLOR_SECOND_ON = 0x0000FF; // Blau
uint32_t COLOR_OFF = 0xFFFFFF;       // Weiß

// Native BinaryTicker mode (TIME_MODE == 7)
// User-defined 3x3 font (ticker)
static std::map<char, std::array<const char *, 3>> FONT3 = {
    {'A', {"010", "111", "101"}}, {'B', {"110", "111", "111"}}, {'C', {"111", "100", "111"}},
    {'D', {"110", "101", "110"}}, {'E', {"111", "110", "111"}}, {'F', {"111", "110", "100"}},
    {'G', {"110", "101", "111"}}, {'H', {"101", "111", "101"}}, {'I', {"111", "010", "111"}},
    {'J', {"011", "001", "111"}}, {'K', {"101", "110", "101"}}, {'L', {"100", "100", "111"}},
    {'M', {"111", "111", "101"}}, {'N', {"111", "101", "101"}}, {'O', {"111", "101", "111"}},
    {'P', {"111", "111", "100"}}, {'Q', {"111", "101", "011"}}, {'R', {"111", "110", "101"}},
    {'S', {"011", "010", "110"}}, {'T', {"111", "010", "010"}}, {'U', {"101", "101", "111"}},
    {'V', {"101", "101", "010"}}, {'W', {"101", "111", "111"}}, {'X', {"101", "010", "101"}},
    {'Y', {"101", "010", "010"}}, {'Z', {"110", "010", "011"}},
    {'0', {"111", "101", "111"}}, {'1', {"010", "010", "010"}}, {'2', {"110", "010", "011"}},
    {'3', {"111", "011", "111"}}, {'4', {"101", "111", "001"}}, {'5', {"011", "010", "110"}},
    {'6', {"100", "111", "111"}}, {'7', {"111", "001", "001"}}, {'8', {"111", "111", "111"}},
    {'9', {"111", "111", "001"}}, {' ', {"000", "000", "000"}},
};

// User-defined 3x4 font (temperature)
static std::map<char, std::array<const char *, 4>> FONT4 = {
    {'A', {"010", "101", "111", "101"}}, {'B', {"110", "111", "101", "111"}}, {'C', {"111", "100", "100", "111"}},
    {'D', {"110", "101", "101", "110"}}, {'E', {"111", "110", "100", "111"}}, {'F', {"111", "110", "100", "100"}},
    {'G', {"111", "100", "101", "111"}}, {'H', {"101", "111", "101", "101"}}, {'I', {"111", "010", "010", "111"}},
    {'J', {"001", "001", "101", "111"}}, {'K', {"101", "110", "110", "101"}}, {'L', {"100", "100", "100", "111"}},
    {'M', {"111", "111", "101", "101"}}, {'N', {"110", "111", "101", "101"}}, {'O', {"111", "101", "101", "111"}},
    {'P', {"111", "101", "111", "100"}}, {'Q', {"111", "101", "111", "011"}}, {'R', {"110", "101", "110", "101"}},
    {'S', {"110", "111", "001", "111"}}, {'T', {"111", "010", "010", "010"}}, {'U', {"101", "101", "101", "111"}},
    {'V', {"101", "101", "101", "010"}}, {'W', {"101", "101", "111", "101"}}, {'X', {"101", "010", "010", "101"}},
    {'Y', {"101", "010", "010", "010"}}, {'Z', {"111", "011", "110", "111"}},
    {'0', {"111", "101", "101", "111"}}, {'1', {"010", "110", "010", "111"}}, {'2', {"011", "111", "100", "111"}},
    {'3', {"111", "011", "001", "111"}}, {'4', {"101", "101", "111", "001"}}, {'5', {"110", "111", "001", "111"}},
    {'6', {"111", "110", "101", "111"}}, {'7', {"111", "001", "001", "001"}}, {'8', {"111", "111", "101", "111"}},
    {'9', {"111", "101", "111", "001"}}, {' ', {"000", "000", "000", "000"}},
};

static int16_t BT_tickerX = 32;
static uint32_t BT_lastStepMs = 0;
static uint8_t BT_sceneIndex = 0;
static uint32_t BT_sceneLastSwitchMs = 0;

static uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static uint32_t wheelColor(uint8_t p)
{
    p = 255 - p;
    if (p < 85) return rgb(255 - p * 3, 0, p * 3);
    if (p < 170)
    {
        p -= 85;
        return rgb(0, p * 3, 255 - p * 3);
    }
    p -= 170;
    return rgb(p * 3, 255 - p * 3, 0);
}

static void drawGlyph3x3(FastLED_NeoMatrix *matrix, char c, int16_t x, int16_t y, uint32_t color)
{
    auto it = FONT3.find(toupper(c));
    if (c == ' ') it = FONT3.find(' ');
    if (it == FONT3.end()) return;
    const auto &rows = it->second;
    for (uint8_t gy = 0; gy < 3; gy++)
    {
        for (uint8_t gx = 0; gx < 3; gx++)
        {
            if (rows[gy][gx] == '1') matrix->drawPixel(x + gx, y + gy, color);
        }
    }
}

static void drawGlyph3x4(FastLED_NeoMatrix *matrix, char c, int16_t x, int16_t y, uint32_t color)
{
    auto it = FONT4.find(toupper(c));
    if (c == ' ') it = FONT4.find(' ');
    if (it == FONT4.end()) return;
    const auto &rows = it->second;
    for (uint8_t gy = 0; gy < 4; gy++)
    {
        for (uint8_t gx = 0; gx < 3; gx++)
        {
            if (rows[gy][gx] == '1') matrix->drawPixel(x + gx, y + gy, color);
        }
    }
}

static uint8_t pickNextScene()
{
    uint8_t scenes[8];
    uint8_t n = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        if ((BINARY_ROW_SCENES >> i) & 1) scenes[n++] = i;
    }
    if (n == 0) return 0;
    return scenes[random(0, n)];
}

static void drawMidlineOverlay(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    int rowY = 3 + y;
    bool progressActive = (BINARY_ROW_PROGRESS >= 0 && BINARY_ROW_PROGRESS <= 100);

    uint8_t mode = BINARY_ROW_MODE; // 0=scene,1=progress,2=auto
    bool useProgress = (mode == 1) || (mode == 2 && progressActive);

    if (useProgress)
    {
        int p = constrain(BINARY_ROW_PROGRESS, 0, 100);
        int lit = map(p, 0, 100, 0, 32);
        for (int i = 0; i < 32; i++)
        {
            uint32_t c = (i < lit) ? rgb(0, 180, 255) : rgb(20, 10, 30);
            matrix->drawPixel(i + x, rowY, c);
        }
        return;
    }

    uint32_t nowMs = millis();
    uint32_t switchMs = max((uint32_t)1000, (uint32_t)BINARY_ROW_INTERVAL * 1000);
    if (nowMs - BT_sceneLastSwitchMs >= switchMs)
    {
        BT_sceneIndex = pickNextScene();
        BT_sceneLastSwitchMs = nowMs;
    }

    uint8_t sp = constrain(BINARY_ROW_SPEED, (uint8_t)1, (uint8_t)100);
    uint32_t t = nowMs / (110 - sp); // higher speed => faster motion

    switch (BT_sceneIndex)
    {
    case 0: // police lights
    {
        bool phase = ((t / 6) % 2) == 0;
        for (int i = 0; i < 32; i++)
        {
            if (i < 16) matrix->drawPixel(i + x, rowY, phase ? rgb(255, 0, 0) : rgb(20, 0, 0));
            else matrix->drawPixel(i + x, rowY, phase ? rgb(0, 0, 30) : rgb(0, 0, 255));
        }
        break;
    }
    case 1: // rainbow chase
    {
        for (int i = 0; i < 32; i++)
        {
            uint8_t hue = (uint8_t)((i * 8 + t) & 0xFF);
            matrix->drawPixel(i + x, rowY, wheelColor(hue));
        }
        break;
    }
    case 2: // pulse
    {
        uint8_t v = (sin8((uint8_t)t) / 2) + 40;
        uint32_t c = rgb(v, v, v);
        for (int i = 0; i < 32; i++) matrix->drawPixel(i + x, rowY, c);
        break;
    }
    default: // sparkle
    {
        for (int i = 0; i < 32; i++) matrix->drawPixel(i + x, rowY, rgb(10, 0, 15));
        for (int k = 0; k < 5; k++)
        {
            int px = random(0, 32);
            matrix->drawPixel(px + x, rowY, wheelColor((uint8_t)random(0, 255)));
        }
        break;
    }
    }
}

uint16_t nativeAppsCount;

int WEATHER_CODE;
String WEATHER_TEMP;
String WEATHER_HUM;

std::vector<std::pair<String, AppCallback>> Apps;
String currentCustomApp;
std::map<String, CustomApp> customApps;

CustomApp *getCustomAppByName(String name)
{
    return customApps.count(name) ? &customApps[name] : nullptr;
}

String getAppNameByFunction(AppCallback AppFunction)
{
    for (const auto &appPair : Apps)
    {
        if (appPair.second == AppFunction)
        {
            return appPair.first;
        }
    }

    return "";
}

String getAppNameAtIndex(int index)
{
    if (index >= 0 && index < Apps.size())
    {
        return Apps[index].first;
    }
    else
    {
        return "";
    }
}

int findAppIndexByName(const String &name)
{
    auto it = std::find_if(Apps.begin(), Apps.end(), [&name](const std::pair<String, AppCallback> &appPair)
                           { return appPair.first == name; });
    if (it != Apps.end())
    {
        return std::distance(Apps.begin(), it);
    }
    return -1;
}

const char *getTimeFormat()
{
    if (TIME_MODE == 0)
    {
        return TIME_FORMAT.c_str();
    }
    else
    {
        if (TIME_FORMAT.length() > 5)
        {
            return TIME_FORMAT[2] == ' ' ? "%H %M" : "%H:%M";
        }
        else
        {
            return TIME_FORMAT.c_str();
        }
    }
}

void TimeApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Time";
    currentCustomApp = "";
    const char *timeformat = getTimeFormat();
    static File BIGTIME_BG_GIF;
    static bool BIGTIME_BG_ISGIF = false;
    if (BIGTIME_BG_GIF && TIME_MODE != 5)
    {
        BIGTIME_BG_GIF.close();

        BIGTIME_BG_ISGIF = false;
    }
    if (TIME_MODE == 5)
    {
        char t[20];
        strftime(t, sizeof(t), timeformat, timer_localtime());

        static uint16_t BIGTIME_BG_CURRENTFRAME = 0;
        if (!BIGTIME_BG_ISGIF)
        {
            if (LittleFS.exists("/bigtime.gif"))
            {
                BIGTIME_BG_GIF = LittleFS.open("/bigtime.gif");
                BIGTIME_BG_ISGIF = true;
            }
        }

        if (BIGTIME_BG_ISGIF)
        {
            gifPlayer->playGif(0 + x, 0 + y, &BIGTIME_BG_GIF, BIGTIME_BG_CURRENTFRAME);
            BIGTIME_BG_CURRENTFRAME = gifPlayer->getFrame();
        }
        else
        {
            DisplayManager.drawFilledRect(0 + x, 0 + y, 32, 8, TEXTCOLOR_888);
        }

        t[2] = (timeformat[2] == ' ' && timer_time() % 2) ? ';' : ':';
        t[0] = t[0] == ' ' ? ';' : t[0];
        for (int i = 0; i < 5; i++)
        {
            int xx = i * 7 - (i > 2 ? 2 : 0) - (i == 2);
            matrix->drawBitmap(xx + x, y, bigdigits_mask[t[i] - '0'], 6, 7, 0);
        }

        matrix->drawFastHLine(0 + x, 7 + y, 32, 0);
        matrix->drawFastVLine(6 + x, 0 + y, 7, 0);
        matrix->drawFastVLine(25 + x, 0 + y, 7, 0);
        return;
    }
    else if (TIME_MODE == 6)
    {

        struct tm *currentTime = timer_localtime();
        int hour = currentTime->tm_hour;
        int minute = currentTime->tm_min;
        int second = currentTime->tm_sec;

        auto drawBit = [&](int bit, int x1, int y1, uint32_t colorOn, uint32_t colorOff)
        {
            bool isSet = bit;
            uint32_t color = isSet ? colorOn : colorOff;
            for (int dx = 0; dx < 2; dx++)
            {
                for (int dy = 0; dy < 2; dy++)
                {
                    matrix->drawPixel(x1 + dx + x, y1 + dy + y, color);
                }
            }
        };

        for (int i = 0; i < 6; i++)
        {
            int bitValue = (hour >> (6 - 1 - i)) & 1;
            int x1 = 5 + i * 4;
            int y1 = 0;
            drawBit(bitValue, x1 + x, y1 + y, COLOR_HOUR_ON, COLOR_OFF);
        }

        for (int i = 0; i < 6; i++)
        {
            int bitValue = (minute >> (6 - 1 - i)) & 1;
            int x1 = 5 + i * 4;
            int y1 = 3;
            drawBit(bitValue, x1 + x, y1 + y, COLOR_MINUTE_ON, COLOR_OFF);
        }

        for (int i = 0; i < 6; i++)
        {
            int bitValue = (second >> (6 - 1 - i)) & 1;
            int x1 = 5 + i * 4;
            int y1 = 6;
            drawBit(bitValue, x1 + x, y1 + y, COLOR_SECOND_ON, COLOR_OFF);
        }

        return;
    }
    else if (TIME_MODE == 7)
    {
        // Top ticker (3x3)
        String tickerText = BINARY_TICKER_TEXT;
        if (tickerText.length() == 0)
            tickerText = " ";
        int16_t tickerWidth = tickerText.length() * 4;
        int16_t tx = BT_tickerX;
        for (size_t i = 0; i < tickerText.length(); i++)
        {
            drawGlyph3x3(matrix, tickerText[i], tx + x, y, 0xFFFFFF);
            tx += 4;
        }
        // Use SSPEED (0..1000) as ticker timing for TMODE 7.
        // Higher SSPEED => slower movement.
        uint32_t nowMs = millis();
        uint32_t stepInterval = constrain((uint32_t)SCROLL_SPEED * 2, 20u, 2000u);
        if (nowMs - BT_lastStepMs >= stepInterval)
        {
            BT_tickerX -= 1;
            BT_lastStepMs = nowMs;
            if (BT_tickerX < -tickerWidth)
                BT_tickerX = 32;
        }

        // Middle row overlay engine (row 3)
        drawMidlineOverlay(matrix, x, y);

        // Bottom binary clock (rows 4..7)
        struct tm *currentTime = timer_localtime();
        int hh24 = currentTime->tm_hour;
        int mm = currentTime->tm_min;
        int ss = currentTime->tm_sec;

        bool use12h = TIME_FORMAT.indexOf("%I") >= 0;
        bool isPM = hh24 >= 12;
        int hh = hh24;
        if (use12h)
        {
            hh = hh24 % 12;
            if (hh == 0) hh = 12;
        }

        int d[6] = {hh / 10, hh % 10, mm / 10, mm % 10, ss / 10, ss % 10};
        int bits[6] = {2, 4, 3, 4, 3, 4};
        uint32_t colors[6] = {0x33CCFF, 0x33CCFF, 0x00FF66, 0x00FF66, 0xFFCC33, 0xFFCC33};
        uint32_t off = 0x32003A;

        int cx = 1;
        int hhLeftX = cx;
        for (int i = 0; i < 6; i++)
        {
            for (int b = 0; b < bits[i]; b++)
            {
                bool on = (d[i] >> b) & 1;
                int py = 7 - b;
                matrix->drawPixel(cx + x, py + y, on ? colors[i] : off);
            }
            if (i == 0 || i == 2 || i == 4)
                cx += 1;
            else if (i == 1 || i == 3)
                cx += 2;
        }

        // AM/PM marker: single pixel above-left of HH columns.
        // Red pixel with low brightness (same perceived level as dim-purple OFF bits).
        if (use12h)
        {
            uint32_t ampm = isPM ? rgb(50, 0, 0) : rgb(20, 0, 20);
            matrix->drawPixel(hhLeftX + x, 4 + y, ampm);
        }

        // Temp in F using 3x4 font
        int tempF = (BINARY_OUT_TEMP_F > -500) ? BINARY_OUT_TEMP_F : int(round((CURRENT_TEMP * 9 / 5) + 32));
        String tStr = String(tempF);
        uint8_t maxDigits = constrain(BINARY_TEMP_DIGITS, (uint8_t)1, (uint8_t)3);
        if (tStr.length() > maxDigits)
        {
            tStr = tStr.substring(tStr.length() - maxDigits);
        }
        int tx2 = BINARY_TEMP_X;
        int ty2 = BINARY_TEMP_Y;
        for (size_t i = 0; i < tStr.length(); i++)
        {
            drawGlyph3x4(matrix, tStr[i], tx2 + x, ty2 + y, 0xFFFFFF);
            tx2 += 4;
        }

        return;
    }

    if (TIME_COLOR > 0)
    {
        DisplayManager.setTextColor(TIME_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }

    char t[20];
    if (timeformat[2] == ' ')
    {
        // blink separator
        char t2[20];
        strcpy(t2, timeformat);
        if (timer_time() % 2)
        {
            t2[2] = ' ';
        }
        else
        {
            t2[2] = ':';
        }
        strftime(t, sizeof(t), t2, timer_localtime());
    }
    else
    {
        strftime(t, sizeof(t), timeformat, timer_localtime());
    }

    int16_t wdPosY;
    int16_t timePosY;
    if (TIME_MODE == 2 || TIME_MODE == 4)
    {
        // week days on top line
        wdPosY = 0;
        timePosY = 7;
    }
    else
    {
        // week days on bottom line
        wdPosY = 7;
        timePosY = 6;
    }

    // time
    DisplayManager.printText(12 + x, timePosY + y, t, TIME_MODE == 0, 2);

    // day of month in calendar box
    if (TIME_MODE > 0)
    {
        int offset;
        char day_str[3];
        sprintf(day_str, "%d", timer_localtime()->tm_mday);

        // calendar box
        DisplayManager.drawFilledRect(x, y, 9, 8, CALENDAR_BODY_COLOR);
        if (TIME_MODE <= 2)
        {
            DisplayManager.drawFilledRect(x, y, 9, 2, CALENDAR_HEADER_COLOR);
        }
        else
        {
            DisplayManager.drawLine(1, 0, 2, 0, 0x000000);
            DisplayManager.drawLine(6, 0, 7, 0, 0x000000);
        }

        // day of month
        if (timer_localtime()->tm_mday < 10)
            offset = 3;
        else
            offset = 1;
        DisplayManager.setCursor(offset + x, 7 + y);
        DisplayManager.setTextColor(CALENDAR_TEXT_COLOR);
        DisplayManager.matrixPrint(day_str);
    }

    if (!SHOW_WEEKDAY)
        return;

    // line of week days
    uint8_t LINE_WIDTH = TIME_MODE > 0 ? 2 : 3;
    uint8_t LINE_SPACING = 1;
    uint8_t LINE_START = TIME_MODE > 0 ? 10 : 2;
    uint8_t dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        int lineStart = LINE_START + i * (LINE_WIDTH + LINE_SPACING);
        int lineEnd = lineStart + LINE_WIDTH - 1;

        uint32_t color;
        if (i == (timer_localtime()->tm_wday + 6 + dayOffset) % 7)
            color = WDC_ACTIVE; // current day
        else
            color = WDC_INACTIVE; // other days

        DisplayManager.drawLine(lineStart + x, wdPosY + y, lineEnd + x, wdPosY + y, color);
    }
}

void DateApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Date";
    currentCustomApp = "";
    if (DATE_COLOR > 0)
    {
        DisplayManager.setTextColor(DATE_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    char d[20];
    strftime(d, sizeof(d), DATE_FORMAT.c_str(), timer_localtime());
    DisplayManager.printText(0 + x, 6 + y, d, true, 2);
    if (!SHOW_WEEKDAY)
        return;
    int dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timer_localtime()->tm_wday + 6 + dayOffset) % 7)
        {
            DisplayManager.drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, WDC_ACTIVE);
        }
        else
        {
            DisplayManager.drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, WDC_INACTIVE);
        }
    }
}

void TempApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Temperature";
    currentCustomApp = "";
    if (TEMP_COLOR > 0)
    {
        DisplayManager.setTextColor(TEMP_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    matrix->drawRGBBitmap(x, y, icon_234, 8, 8);

    if (TEMP_DECIMAL_PLACES > 0)
        DisplayManager.setCursor(8 + x, 6 + y);
    else
        DisplayManager.setCursor(12 + x, 6 + y);

    if (IS_CELSIUS)
    {
        DisplayManager.matrixPrint(CURRENT_TEMP, TEMP_DECIMAL_PLACES);
        DisplayManager.matrixPrint(utf8ascii("°C"));
    }
    else
    {
        double tempF = (CURRENT_TEMP * 9 / 5) + 32;
        DisplayManager.matrixPrint(tempF, TEMP_DECIMAL_PLACES);
        DisplayManager.matrixPrint(utf8ascii("°F"));
    }
}

void HumApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Humidity";
    currentCustomApp = "";
    if (HUM_COLOR > 0)
    {
        DisplayManager.setTextColor(HUM_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    matrix->drawRGBBitmap(x, y + 1, icon_2075, 8, 8);
    DisplayManager.setCursor(14 + x, 6 + y);
    int humidity = CURRENT_HUM;
    DisplayManager.matrixPrint(humidity, 0);
    DisplayManager.matrixPrint("%");
}

#ifndef awtrix2_upgrade
void BatApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Battery";
    currentCustomApp = "";
    if (BAT_COLOR > 0)
    {
        DisplayManager.setTextColor(BAT_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    matrix->drawRGBBitmap(x, y, icon_1486, 8, 8);
    DisplayManager.setCursor(14 + x, 6 + y);
    DisplayManager.matrixPrint(BATTERY_PERCENT, 0); // Ausgabe des Ladezustands
    DisplayManager.matrixPrint("%");
}
#endif

String replacePlaceholders(String text)
{
    int start = 0;
    while ((start = text.indexOf("{{", start)) != -1)
    {
        int end = text.indexOf("}}", start);
        if (end == -1)
        {
            break;
        }
        String placeholder = text.substring(start + 2, end);
        String topic = placeholder;
        text.replace("{{" + placeholder + "}}", MQTTManager.getValueForTopic(topic));
        start = end + 2;
    }
    return text;
}

void ShowCustomApp(String name, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    // Abort if notifyFlag is set
    if (notifyFlag)
    {
        return;
    }

    // Get custom App by ID
    CustomApp *ca = getCustomAppByName(name);

    // Abort if custom App not found
    if (ca == nullptr)
    {
        return;
    }

    if (!DisplayManager.appIsSwitching)
    {
        if (ca->duration > 0)
        {
            DisplayManager.setAppTime(ca->duration);
        }
    }

    DisplayManager.drawFilledRect(x, y, 32, 8, ca->background);

    if (ca->effect > -1)
    {
        callEffect(matrix, x, y, ca->effect);
    }

    CURRENT_APP = ca->name;
    currentCustomApp = name;

    if ((ca->iconName.length() > 0) && !ca->icon)
    {
        const char *extensions[] = {".jpg", ".gif"};
        bool isGifFlags[] = {false, true};

        for (int i = 0; i < 2; i++)
        {
            String filePath = "/ICONS/" + ca->iconName + extensions[i];
            if (LittleFS.exists(filePath))
            {
                ca->isGif = isGifFlags[i];
                ca->icon = LittleFS.open(filePath);
                ca->currentFrame = 0;
                break;
            }
        }
    }

    bool hasIcon = ca->icon || ca->jpegDataSize > 0;

    uint16_t textWidth = 0;
    if (!ca->fragments.empty())
    {
        for (const auto &fragment : ca->fragments)
        {
            String replacedFragment = replacePlaceholders(fragment);
            textWidth += getTextWidth(replacedFragment.c_str(), ca->textCase);
        }
    }
    else
    {
        String replacedText = replacePlaceholders(ca->text);
        textWidth = getTextWidth(replacedText.c_str(), ca->textCase);
    }

    uint16_t availableWidth = (hasIcon) ? 24 : 32;

    bool noScrolling = textWidth <= availableWidth;
    int iconWidth;
    auto renderFirst = [&]()
    {
        if (hasIcon)
        {
            if (ca->pushIcon > 0 && !noScrolling)
            {
                if (ca->iconPosition < 0 && ca->iconWasPushed == false && ca->scrollposition > 8)
                {
                    if (state->appState == FIXED)
                        ca->iconPosition += movementFactor;
                }
                if (ca->scrollposition < (9 - ca->textOffset) && !ca->iconWasPushed)
                {
                    ca->iconPosition = ca->scrollposition - 9 + ca->textOffset;

                    if (ca->iconPosition <= -9 - ca->textOffset)
                    {
                        ca->iconWasPushed = true;
                    }
                }
            }
            if (ca->isGif)
            {
                iconWidth = gifPlayer->playGif(x + ca->iconPosition + ca->iconOffset, y, &ca->icon, ca->currentFrame);
                ca->currentFrame = gifPlayer->getFrame();
            }
            else
            {
                iconWidth = 8;
                if (ca->jpegDataSize > 0)
                {
                    DisplayManager.drawJPG(x + ca->iconPosition + ca->iconOffset, y, ca->jpegDataBuffer, ca->jpegDataSize);
                }
                else
                {
                    DisplayManager.drawJPG(x + ca->iconPosition + ca->iconOffset, y, ca->icon);
                }
            }
            if (!noScrolling)
            {
                if (ca->progress > -1)
                {
                    DisplayManager.drawLine(iconWidth + x + ca->iconPosition, 0 + y, iconWidth + x + ca->iconPosition, 6 + y, ca->background);
                }
                else
                {
                    DisplayManager.drawLine(iconWidth + x + ca->iconPosition, 0 + y, iconWidth + x + ca->iconPosition, 7 + y, ca->background);
                }
            }
        }

        if (ca->drawInstructions.length() > 0)
        {
            DisplayManager.processDrawInstructions(x, y, ca->drawInstructions);
        }

        if (ca->progress > -1)
        {
            DisplayManager.drawProgressBar((hasIcon ? 8 : 0), 7 + y, ca->progress, ca->pColor, ca->pbColor);
        }

        if (ca->barSize > 0)
        {
            DisplayManager.drawBarChart(x, y, ca->barData, ca->barSize, hasIcon, ca->color, ca->barBG);
        }

        if (ca->lineSize > 0)
        {
            DisplayManager.drawLineChart(x, y, ca->lineData, ca->lineSize, hasIcon, ca->color);
        }
    };

    if (ca->topText)
    {
        renderFirst();
    }

    if ((ca->repeat > 0) && (textWidth > availableWidth) && (state->appState == FIXED))
    {
        DisplayManager.setAutoTransition(false);
    }
    else
    {
        DisplayManager.setAutoTransition(true);
    }

    if (textWidth > availableWidth && !(state->appState == IN_TRANSITION))
    {
        if (ca->scrollposition + ca->textOffset <= (-textWidth))
        {
            if (ca->iconWasPushed && ca->pushIcon == 2)
            {
                ca->iconWasPushed = false;
            }
            if ((ca->currentRepeat + 1 >= ca->repeat) && (ca->repeat > 0))
            {
                DisplayManager.setAutoTransition(true);
                ca->currentRepeat = 0;
                DisplayManager.nextApp();
                ca->scrollDelay = 0;
                ca->scrollposition = 9 + ca->textOffset;
                return;
            }
            else if (ca->repeat > 0)
            {
                ++ca->currentRepeat;
            }
            ca->scrollDelay = 0;
            ca->scrollposition = 9 + ca->textOffset;
        }
    }
    if (!noScrolling)
    {
        if ((ca->scrollDelay > MATRIX_FPS) || ((hasIcon ? ca->textOffset + 9 : ca->textOffset) > 31))
        {
            if (state->appState == FIXED && !ca->noScrolling)
            {
                if (ca->scrollSpeed == -1)
                {
                    ca->scrollposition -= movementFactor * ((float)SCROLL_SPEED / 100);
                }
                else
                {
                    ca->scrollposition -= movementFactor * (ca->scrollSpeed / 100);
                }
            }
        }
        else
        {
            ++ca->scrollDelay;
            if (hasIcon)
            {
                if (ca->iconWasPushed && ca->pushIcon == 1)
                {
                    ca->scrollposition = 0 + ca->textOffset;
                }
                else
                {
                    ca->scrollposition = 9 + ca->textOffset;
                }
            }
            else
            {
                ca->scrollposition = 0 + ca->textOffset;
            }
        }
    }

    int16_t textX;
    if (ca->center)
    {
        textX = hasIcon ? ((24 - textWidth) / 2) + 9 : ((32 - textWidth) / 2);
    }
    else
    {
        textX = hasIcon ? 9 : 0;
    }

    String text = replacePlaceholders(ca->text);

    if (noScrolling)
    {
        ca->repeat = -1; // Disable repeat if text is too short for scrolling

        if (!ca->fragments.empty())
        {
            int16_t fragmentX = textX + ca->textOffset;
            for (size_t i = 0; i < ca->fragments.size(); ++i)
            {
                String text = replacePlaceholders(ca->fragments[i]);
                DisplayManager.setTextColor(TextEffect(ca->colors[i], ca->fade, ca->blink));
                DisplayManager.printText(x + fragmentX, y + 6, text.c_str(), false, ca->textCase);
                fragmentX += getTextWidth(text.c_str(), ca->textCase);
            }
        }
        else
        {
            String text = replacePlaceholders(ca->text);
            if (ca->rainbow)
            {
                DisplayManager.HSVtext(x + textX + ca->textOffset, 6 + y, text.c_str(), false, ca->textCase);
            }
            else if (ca->gradient[0] > -1 && ca->gradient[1] > -1)
            {
                DisplayManager.GradientText(x + textX + ca->textOffset, 6 + y, text.c_str(), ca->gradient[0], ca->gradient[1], false, ca->textCase);
            }
            else
            {
                DisplayManager.setTextColor(TextEffect(ca->color, ca->fade, ca->blink));
                DisplayManager.printText(x + textX + ca->textOffset, y + 6, text.c_str(), false, ca->textCase);
            }
        }
    }
    else
    {
        if (!ca->fragments.empty())
        {
            int16_t fragmentX = ca->scrollposition + ca->textOffset;
            for (size_t i = 0; i < ca->fragments.size(); ++i)
            {
                String text = replacePlaceholders(ca->fragments[i]);
                DisplayManager.setTextColor(TextEffect(ca->colors[i], ca->fade, ca->blink));
                DisplayManager.printText(x + fragmentX, y + 6, text.c_str(), false, ca->textCase);
                fragmentX += getTextWidth(text.c_str(), ca->textCase);
            }
        }
        else
        {
            if (ca->rainbow)
            {
                DisplayManager.HSVtext(x + ca->scrollposition + ca->textOffset, 6 + y, text.c_str(), false, ca->textCase);
            }
            else if (ca->gradient[0] > -1 && ca->gradient[1] > -1)
            {
                DisplayManager.GradientText(x + ca->scrollposition + ca->textOffset, 6 + y, text.c_str(), ca->gradient[0], ca->gradient[1], false, ca->textCase);
            }
            else
            {
                DisplayManager.setTextColor(TextEffect(ca->color, ca->fade, ca->blink));
                DisplayManager.printText(x + ca->scrollposition + ca->textOffset, 6 + y, text.c_str(), false, ca->textCase);
            }
        }
    }

    if (!ca->topText)
    {
        renderFirst();
    }

    if (ca->lifeTimeEnd)
    {
        DisplayManager.drawRect(x, y, 32 + x, 8 + y, 0x6e0700);
    }

    if (!ca->overlay == NONE)
    {
        EffectOverlay(matrix, x, y, ca->overlay);
    }

    DisplayManager.getInstance().resetTextColor();
}

// Unattractive to have a function for every customapp wich does the same, but currently still no other option found TODO

void CApp1(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp1);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp2(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp2);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp3(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp3);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp4(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp4);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp5(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp5);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp6(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp6);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp7(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp7);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp8(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp8);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp9(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp9);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp10(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp10);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp11(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp11);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp12(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp12);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp13(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp13);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp14(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp14);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp15(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp15);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp16(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp16);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp17(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp17);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp18(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp18);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp19(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp19);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void CApp20(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp20);
    ShowCustomApp(name, matrix, state, x, y, gifPlayer);
}

void (*customAppCallbacks[20])(FastLED_NeoMatrix *, MatrixDisplayUiState *, int16_t, int16_t, GifPlayer *) = {CApp1, CApp2, CApp3, CApp4, CApp5, CApp6, CApp7, CApp8, CApp9, CApp10, CApp11, CApp12, CApp13, CApp14, CApp15, CApp16, CApp17, CApp18, CApp19, CApp20};
