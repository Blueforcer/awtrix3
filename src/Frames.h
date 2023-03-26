#ifndef FRAMES_H
#define FRAMES_H

#include <vector>
#include <map>
#include "icons.h"
#include <FastLED_NeoMatrix.h>
#include "MatrixDisplayUi.h"
#include "Globals.h"
#include "Functions.h"
#include "MenuManager.h"
#include "PeripheryManager.h"
#include "DisplayManager.h"
#include "LittleFS.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Ticker.h>

Ticker downloader;

tm timeInfo;
uint16_t nativeAppsCount;
uint16_t customPagesCount;

int WEATHER_CODE;
String WEATHER_TEMP;
String WEATHER_HUM;

struct CustomFrame
{
    int16_t scrollposition = 0;
    int16_t scrollDelay = 0;
    String text;
    uint16_t color;
    File icon;
    bool isGif;
    bool rainbow;
    bool soundPlayed;
    String sound;
    int16_t repeat = 0;
    int16_t currentRepeat = 0;
    String name;
    byte pushIcon = 0;
    int16_t iconPosition = 0;
    bool iconWasPushed = false;
};

String currentCustomFrame;
std::map<String, CustomFrame> customFrames;

struct Notification
{
    int16_t scrollposition = 34;
    int16_t scrollDelay = 0;
    String text;
    uint16_t color;
    File icon;
    bool rainbow;
    bool isGif;
    bool flag = false;
    unsigned long startime = 0;
    unsigned long duration = 0;
    int16_t repeat = -1;
    bool hold = false;
    byte pushIcon = 0;
    int16_t iconPosition = 0;
    bool iconWasPushed = false;
};

Notification notify;

std::vector<std::pair<String, AppCallback>> Apps;

CustomFrame *getCustomFrameById(String name)
{
    return customFrames.count(name) ? &customFrames[name] : nullptr;
}

String getFrameNameByFunction(AppCallback frameFunction)
{
    for (const auto &appPair : Apps)
    {
        if (appPair.second == frameFunction)
        {
            return appPair.first;
        }
    }

    return ""; // Gibt einen leeren String zurück, wenn die Frame-Funktion nicht gefunden wurde
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

void TimeFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Time";
    DisplayManager.getInstance().resetTextColor();
    time_t now = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now);
    char t[20];
    if (now % 2) {
        if (TIME_FORMAT == "%H:%M") {
            TIME_FORMAT = "%H %M";
        } else if (TIME_FORMAT == "%l:%M") {
            TIME_FORMAT = "%l %M";
        } else if (TIME_FORMAT == "%l:%M %p") {
            TIME_FORMAT = "%l %M %p";
        }
    } else {
        if (TIME_FORMAT == "%H %M") {
            TIME_FORMAT = "%H:%M";
        } else if (TIME_FORMAT == "%l %M") {
            TIME_FORMAT = "%l:%M";
        } else if (TIME_FORMAT == "%l %M %p") {
            TIME_FORMAT = "%l:%M %p";
        }
    }
    strftime(t, sizeof(t), TIME_FORMAT.c_str(), localtime(&now));
    DisplayManager.printText(0 + x, 6 + y, t, true, false);

    if (!SHOW_WEEKDAY)
        return;
    int dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timeInfo->tm_wday + 6 + dayOffset) % 7)
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, matrix->Color(200, 200, 200));
        }
        else
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, matrix->Color(100, 100, 100));
        }
    }
}

void DateFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Date";
    DisplayManager.getInstance().resetTextColor();
    time_t now = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now);
    char d[20];
    strftime(d, sizeof(d), DATE_FORMAT.c_str(), localtime(&now));
    DisplayManager.printText(0 + x, 6 + y, d, true, true);
    if (!SHOW_WEEKDAY)
        return;
    int dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timeInfo->tm_wday + 6 + dayOffset) % 7)
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, matrix->Color(200, 200, 200));
        }
        else
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, matrix->Color(100, 100, 100));
        }
    }
}

void TempFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Temperature";
    DisplayManager.getInstance().resetTextColor();
    matrix->drawRGBBitmap(x, y, get_icon(234), 8, 8);
    matrix->setCursor(12 + x, 6 + y);
    if (IS_CELSIUS)
    {
        matrix->print((int)CURRENT_TEMP);
        matrix->print(utf8ascii("°C"));
    }
    else
    {
        int tempF = (CURRENT_TEMP * 9 / 5) + 32;
        matrix->print(tempF);
        matrix->print(utf8ascii("°F"));
    }
}

void HumFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Humidity";
    DisplayManager.getInstance().resetTextColor();
    matrix->drawRGBBitmap(x, y + 1, get_icon(2075), 8,8);
    matrix->setCursor(14 + x, 6 + y);
    int humidity = CURRENT_HUM; // Temperatur ohne Nachkommastellen
    matrix->print(humidity);    // Ausgabe der Temperatur
    matrix->print("%");
}

void BatFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Battery";
    DisplayManager.getInstance().resetTextColor();
    matrix->drawRGBBitmap(x, y, get_icon(1486), 8, 8);
    matrix->setCursor(14 + x, 6 + y);
    matrix->print(BATTERY_PERCENT); // Ausgabe des Ladezustands
    matrix->print("%");
}

void MenuFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state)
{
    if (!MenuManager.inMenu)
        return;
    matrix->fillScreen(0);
    DisplayManager.printText(0, 6, utf8ascii(MenuManager.menutext()).c_str(), true, true);
}

void AlarmFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state)
{
    if (ALARM_ACTIVE)
    {
        matrix->fillScreen(matrix->Color(255, 0, 0));
        CURRENT_APP = "Alarm";
        uint16_t textWidth = getTextWidth("ALARM", false);
        int16_t textX = ((32 - textWidth) / 2);
        matrix->setTextColor(0);
        matrix->setCursor(textX, 6);
        matrix->print("ALARM");
        if (ALARM_SOUND != "")
        {
            if (!PeripheryManager.isPlaying())
                PeripheryManager.playFromFile("/MELODIES/" + ALARM_SOUND + ".txt");
        }
    }
}

void TimerFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state)
{
    if (TIMER_ACTIVE)
    {
        matrix->fillScreen(matrix->Color(0, 255, 0));
        CURRENT_APP = "Timer";
        String menuText = "TIMER";
        uint16_t textWidth = getTextWidth(menuText.c_str(), false);
        int16_t textX = ((32 - textWidth) / 2);
        matrix->setTextColor(0);
        matrix->setCursor(textX, 6);
        matrix->print(menuText);
        if (TIMER_SOUND != "")
        {
            if (!PeripheryManager.isPlaying())
                PeripheryManager.playFromFile("/MELODIES/" + TIMER_SOUND + ".txt");
        }
    }
}

void ShowCustomFrame(String name, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    // Abort if notify.flag is set
    if (notify.flag)
    {
        return;
    }

    // Get custom frame by ID
    CustomFrame *cf = getCustomFrameById(name);

    // Abort if custom frame not found
    if (cf == nullptr)
    {
        return;
    }

    // reset custom frame properties if last frame
    if (lastFrame)
    {
        cf->iconWasPushed = false;
        cf->scrollposition = 9;
        cf->iconPosition = 0;
        cf->scrollDelay = 0;
    }

    CURRENT_APP = cf->name;
    currentCustomFrame = name;

    bool hasIcon = cf->icon;
    uint16_t availableWidth = (hasIcon) ? 24 : 32;

    bool noScrolling = getTextWidth(cf->text.c_str(), false) <= availableWidth;

    if ((cf->repeat > 0) && (getTextWidth(cf->text.c_str(), false) > availableWidth) && (state->frameState == FIXED))
    {
        DisplayManager.setAutoTransition(false);
    }
    else
    {
        DisplayManager.setAutoTransition(true);
    }

    if (getTextWidth(cf->text.c_str(), false) > availableWidth && !(state->frameState == IN_TRANSITION))
    {
        if (cf->scrollposition <= -getTextWidth(cf->text.c_str(), false))
        {
            cf->scrollDelay = 0;
            cf->scrollposition = 9;
            if (cf->iconWasPushed && cf->pushIcon == 2)
            {
                cf->iconWasPushed = false;
            }
            if ((cf->currentRepeat + 1 >= cf->repeat) && (cf->repeat > 0))
            {
                DisplayManager.setAutoTransition(true);
                cf->currentRepeat = 0;
                DisplayManager.nextApp();
                return;
            }
            else if (cf->repeat > 0)
            {
                ++cf->currentRepeat;
            }
        }
    }

    if (!noScrolling)
    {
        if ((cf->scrollDelay > MATRIX_FPS * 1.2))
        {
            --cf->scrollposition;
        }
        else
        {
            ++cf->scrollDelay;
            if (hasIcon)
            {
                if (cf->iconWasPushed && cf->pushIcon == 1)
                {
                    cf->scrollposition = 0;
                }
                else
                {
                    cf->scrollposition = 9;
                }
            }
            else
            {
                cf->scrollposition = 0;
            }
        }
    }
    int16_t textX = (hasIcon) ? ((24 - getTextWidth(cf->text.c_str(), false)) / 2) + 9 : ((32 - getTextWidth(cf->text.c_str(), false)) / 2);
    matrix->setTextColor(cf->color);
    if (noScrolling)
    {
        cf->repeat = -1; // Disable repeat if text is too short for scrolling
        // Display text with rainbow effect if enabled
        if (cf->rainbow)
        {
            DisplayManager.getInstance().HSVtext(x + textX, 6 + y, cf->text.c_str(), false);
        }
        else
        {
            // Display text
            DisplayManager.printText(x + textX, y + 6, cf->text.c_str(), false, false);
        }
    }
    else
    {
        // Display scrolling text with rainbow effect if enabled
        if (cf->rainbow)
        {
            DisplayManager.getInstance().HSVtext(x + cf->scrollposition, 6 + y, cf->text.c_str(), false);
        }
        else
        {
            DisplayManager.printText(x + cf->scrollposition, 6 + y, cf->text.c_str(), false, false);
        }
    } // Display icon if present and not pushed
    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (cf->pushIcon > 0 && !noScrolling)
        {
            if (cf->iconPosition < 0 && cf->iconWasPushed == false && cf->scrollposition > 8)
            {
                ++cf->iconPosition;
            }

            if (cf->scrollposition < 8 && !cf->iconWasPushed)
            {
                cf->iconPosition = cf->scrollposition - 8;

                if (cf->iconPosition <= -9)
                {
                    cf->iconWasPushed = true;
                }
            }
        }

        // Display animated GIF if enabled and frame is fixed, since we have only one gifplayer instance, it looks weird when 2 apps want to draw a different gif
        if (cf->isGif)
        {
            if (state->frameState == FIXED)
            {
                DisplayManager.drawGIF(x + cf->iconPosition, y, cf->icon);
            }
        }
        else
        {
            // Display JPEG image
            DisplayManager.drawJPG(x + cf->iconPosition, y, cf->icon);
        }

        // Draw vertical line if text is scrolling
        if (!noScrolling)
        {
            // matrix->drawLine(8 + x + cf->iconPosition, 0 + y, 8 + x + cf->iconPosition, 7 + y, 0);
        }
    }
    // Reset text color
    DisplayManager.getInstance().resetTextColor();
}

void NotifyFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state)
{
    // Check if notification flag is set
    if (!notify.flag)
    {
        return; // Exit function if flag is not set
    }

    // Set current app name
    CURRENT_APP = "Notification";
    // Get current time
    unsigned long currentTime = millis();

    // Check if notification duration has expired or if repeat count is 0 and hold is not enabled
    if ((((currentTime - notify.startime >= notify.duration) && notify.repeat == -1) || notify.repeat == 0) && !notify.hold)
    {
        // Reset notification flags and exit function
        notify.flag = false;
        notify.duration = 0;
        notify.startime = 0;
        notify.scrollposition = 34;
        notify.iconWasPushed = false;
        notify.iconPosition = 0;
        notify.scrollDelay = 0;
        return;
    }

    // Check if notification has an icon
    bool hasIcon = notify.icon;

    // Clear the matrix display
    matrix->fillRect(0, 0, 32, 8, 0);

    // Calculate text and available width
    uint16_t textWidth = getTextWidth(notify.text.c_str(), false);
    uint16_t availableWidth = hasIcon ? 24 : 32;

    // Check if text is scrolling
    bool noScrolling = textWidth <= availableWidth;

    // Check if text needs to be scrolled
    if (textWidth > availableWidth && notify.scrollposition <= -textWidth)
    {
        // Reset scroll position and icon position if needed
        notify.scrollDelay = 0;
        notify.scrollposition = 9;

        if (notify.pushIcon == 2)
        {
            notify.iconWasPushed = false;
        }

        if (notify.repeat > 0)
        {
            --notify.repeat;
            if (notify.repeat == 0)
                return;
        }
    }

    if (!noScrolling)
    {
        if ((notify.scrollDelay > MATRIX_FPS * 1.2))
        {
            --notify.scrollposition;
        }
        else
        {
            ++notify.scrollDelay;
            if (hasIcon)
            {
                if (notify.iconWasPushed && notify.pushIcon == 1)
                {
                    notify.scrollposition = 0;
                }
                else
                {
                    notify.scrollposition = 9;
                }
            }
            else
            {
                notify.scrollposition = 0;
            }
        }
    }

    // Calculate text X position based on icon presence
    int16_t textX = hasIcon ? ((24 - textWidth) / 2) + 9 : ((32 - textWidth) / 2);

    // Set text color
    matrix->setTextColor(notify.color);

    if (noScrolling)
    {
        // Disable repeat if text is not scrolling
        notify.repeat = -1;

        if (notify.rainbow)
        {
            // Display text in rainbow color if enabled
            DisplayManager.getInstance().HSVtext(textX, 6, notify.text.c_str(), false);
        }
        else
        {
            // Display text in solid color
            DisplayManager.printText(textX, 6, notify.text.c_str(), false, false);
        }
    }
    else
    {
        if (notify.rainbow)
        {
            // Display scrolling text in rainbow color if enabled
            DisplayManager.getInstance().HSVtext(notify.scrollposition, 6, notify.text.c_str(), false);
        }
        else
        {
            // Display scrolling text in solid color
            DisplayManager.printText(notify.scrollposition, 6, notify.text.c_str(), false, false);
        }
    }

    // Display icon if present and not pushed
    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (notify.pushIcon > 0 && !noScrolling)
        {
            if (notify.iconPosition < 0 && notify.iconWasPushed == false && notify.scrollposition > 8)
            {
                ++notify.iconPosition;
            }

            if (notify.scrollposition < 8 && !notify.iconWasPushed)
            {
                notify.iconPosition = notify.scrollposition - 9;

                if (notify.iconPosition <= -9)
                {
                    notify.iconWasPushed = true;
                }
            }
        }

        // Display animated GIF if
        if (notify.isGif)
        {
            // Display GIF if present
            DisplayManager.drawGIF(notify.iconPosition, 0, notify.icon);
        }
        else
        {
            // Display JPG image if present
            DisplayManager.drawJPG(notify.iconPosition, 0, notify.icon);
        }

        // Display icon divider line if text is scrolling
        if (!noScrolling)
        {
            // matrix->drawLine(8 + notify.iconPosition, 0, 8 + notify.iconPosition, 7, 0);
        }
    }

    // Reset text color after displaying notification
    DisplayManager.getInstance().resetTextColor();
}

void CFrame1(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame1);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame2(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame2);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame3(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame3);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame4(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame4);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame5(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame5);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame6(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame6);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame7(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame7);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame8(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame8);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame9(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame9);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame10(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    String name = getFrameNameByFunction(CFrame10);
    ShowCustomFrame(name, matrix, state, x, y, firstFrame, lastFrame);
}

const uint16_t *getWeatherIcon(int code)
{
    switch (code)
    {
    case 1:
        return icon_475;
        break;

    default:
        return icon_475;
        break;
    }
}

void WeatherFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Weather";
    DisplayManager.getInstance().resetTextColor();
    matrix->drawRGBBitmap(x, y, getWeatherIcon(WEATHER_CODE), 8, 8);
    String text = WEATHER_TEMP + "°" + WEATHER_HUM + "%";
    uint16_t textWidth = getTextWidth(text.c_str(), false);
    int16_t textX = ((23 - textWidth) / 2);
    matrix->setCursor(textX + 11, 6 + y);
    matrix->print(utf8ascii(text));
}

void getWeatherData()
{
    Serial.println("UPDATE");
    String weatherUrl = "https://wttr.in/" + CITY + "?format=p1";
    if ((WiFi.status() == WL_CONNECTED))
    {
        HTTPClient http;
        http.begin(weatherUrl);
        http.setTimeout(5000);
        int httpCode = http.GET();

        if (httpCode > 0)
        {
            String payload = http.getString();
            int temperatureIndex = payload.indexOf("temperature_celsius{forecast=\"current\"}");
            int humIndex = payload.indexOf("humidity_percentage{forecast=\"current\"}");
            int weatherCodeIndex = payload.indexOf("weather_code{forecast=\"current\"}");

            if (temperatureIndex >= 0 && weatherCodeIndex >= 0)
            {
                int tempEndIndex = payload.indexOf('\n', temperatureIndex);
                int codeEndIndex = payload.indexOf('\n', weatherCodeIndex);
                int humEndIndex = payload.indexOf('\n', humIndex);
                String temperatureValue = payload.substring(temperatureIndex + 40, tempEndIndex);
                String humValue = payload.substring(humIndex + 40, humEndIndex);
                String weatherCodeValue = payload.substring(weatherCodeIndex + 33, codeEndIndex);

                WEATHER_TEMP = temperatureValue;
                WEATHER_HUM = humValue;
                WEATHER_CODE = weatherCodeValue.toInt();
            }
        }
        http.end();
    }
}

void StartAppUpdater()
{
    // downloader.attach(60,getWeatherData);
    // getWeatherData();
}

OverlayCallback overlays[] = {MenuFrame, NotifyFrame, AlarmFrame, TimerFrame};
#endif
