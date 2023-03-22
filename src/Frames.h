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

int WEATHER_CODE;
String WEATHER_TEMP;
String WEATHER_HUM;

struct CustomFrame
{
    uint8_t id;
    int16_t scrollposition = 34;
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

uint8_t currentCustomFrame;
std::map<uint8_t, CustomFrame> customFrames;

struct Notification
{
    uint8_t id;
    int16_t scrollposition = 34;
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

CustomFrame *getCustomFrameById(uint8_t id)
{
    return customFrames.count(id) ? &customFrames[id] : nullptr;
}

void TimeFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Time";
    DisplayManager.getInstance().resetTextColor();
    time_t now1 = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now1);
    char t[14];
    if (SHOW_SECONDS)
    {
        sprintf_P(t, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
        matrix->setCursor(2 + x, 6 + y);
    }
    else
    {
        sprintf_P(t, PSTR("%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min);
        matrix->setCursor(7 + x, 6 + y);
    }

    matrix->println(t);

    if (!SHOW_WEEKDAY)
        return;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timeInfo->tm_wday + 6) % 7)
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
    time_t now1 = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now1);
    char d[11];
    sprintf_P(d, PSTR("%02d.%02d.%02d"), timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year % 100);
    matrix->setCursor(2 + x, 6 + y);
    matrix->println(d);
    if (!SHOW_WEEKDAY)
        return;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timeInfo->tm_wday + 6) % 7)
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
    matrix->setCursor(14 + x, 6 + y);
    matrix->print((int)CURRENT_TEMP); // Ausgabe der Temperatur
    matrix->print(utf8ascii("°"));
}

void HumFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Humidity";
    DisplayManager.getInstance().resetTextColor();
    matrix->drawRGBBitmap(x, y + 1, get_icon(2075), 8, 8);
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
    DisplayManager.printText(0, 6, MenuManager.menutext().c_str(), true);
}

void AlarmFrame(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state)
{
    if (ALARM_ACTIVE)

    {
        matrix->fillScreen(matrix->Color(255, 0, 0));
        CURRENT_APP = "Alarm";
        uint16_t textWidth = getTextWidth("ALARM");
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
        uint16_t textWidth = getTextWidth(menuText.c_str());
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

void ShowCustomFrame(uint8_t id, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    // Abort if notify.flag is set
    if (notify.flag)
    {
        return;
    }

    // Get custom frame by ID
    CustomFrame *cf = getCustomFrameById(id);

    // Abort if custom frame not found
    if (cf == nullptr)
    {
        return;
    }

    // Initialize custom frame properties if first frame
    if (lastFrame)
    {
        cf->iconWasPushed = false;
        cf->scrollposition = 34;
        cf->iconPosition = 0;
    }

    // Update current app and custom frame IDs
    CURRENT_APP = cf->name;
    currentCustomFrame = id;

    // Check if there is an icon
    bool hasIcon = cf->icon;

    // Calculate available display width based on icon
    uint16_t availableWidth = (hasIcon) ? 24 : 32;

    // Disable auto transition if text is repeating and too wide
    if ((cf->repeat > 0) && (getTextWidth(cf->text.c_str()) > availableWidth) && (state->frameState == FIXED))
    {
        DisplayManager.setAutoTransition(false);
    }
    else
    {
        DisplayManager.setAutoTransition(true);
    }

    // Check if text is wider than available display width and frame is not in transition
    if (getTextWidth(cf->text.c_str()) > availableWidth && !(state->frameState == IN_TRANSITION))
    {
        // Reset scroll position when text has finished scrolling all the way to the left
        if (cf->scrollposition <= -getTextWidth(cf->text.c_str()))
        {
            cf->scrollposition = 38;

            // Set iconWasPushed to false if icon has been pushed
            if (cf->iconWasPushed && cf->pushIcon == 2)
            {
                cf->iconWasPushed = false;
            }

            // Transition to next app if frame is repeating and repeat limit has been reached
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

        // Update scroll position
        --cf->scrollposition;
    }

    // Calculate horizontal text alignment
    int16_t textX = (hasIcon) ? ((24 - getTextWidth(cf->text.c_str())) / 2) + 9 : ((32 - getTextWidth(cf->text.c_str())) / 2);

    // Set text color
    matrix->setTextColor(cf->color);

    // Check if text is scrolling or not
    bool noScrolling = getTextWidth(cf->text.c_str()) <= availableWidth;

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

            DisplayManager.printText(x + textX, y + 6, cf->text.c_str(), false);
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
            DisplayManager.printText(x + cf->scrollposition, 6 + y, cf->text.c_str(), false);
        }
    } // Display icon if present and not pushed
    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (cf->pushIcon > 0 && !noScrolling)
        {
            if (cf->iconPosition < 0 && cf->iconWasPushed == false && cf->scrollposition > 9)
            {
                ++cf->iconPosition;
            }

            if (cf->scrollposition < 9 && !cf->iconWasPushed)
            {
                cf->iconPosition = cf->scrollposition - 9;

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
            matrix->drawLine(8 + x + notify.iconPosition, 0 + y, 8 + x + notify.iconPosition, 7 + y, 0);
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
        return;
    }

    // Check if notification has an icon
    bool hasIcon = notify.icon;

    // Clear the matrix display
    matrix->fillRect(0, 0, 32, 8, 0);

    // Calculate text and available width
    uint16_t textWidth = getTextWidth(notify.text.c_str());
    uint16_t availableWidth = hasIcon ? 24 : 32;

    // Check if text needs to be scrolled
    if (textWidth > availableWidth && notify.scrollposition <= -textWidth)
    {
        // Reset scroll position and icon position if needed
        notify.scrollposition = 38;

        if (notify.pushIcon == 2)
        {
            notify.iconWasPushed = false;
            // notify.iconPosition = 0;
        }

        if (notify.repeat > 0)
        {
            --notify.repeat;
        }
    }
    else if (textWidth > availableWidth)
    {
        // Update scroll position
        --notify.scrollposition;
    }

    // Calculate text X position based on icon presence
    int16_t textX = hasIcon ? ((24 - textWidth) / 2) + 9 : ((32 - textWidth) / 2);

    // Set text color
    matrix->setTextColor(notify.color);

    // Check if text is scrolling
    bool noScrolling = textWidth <= availableWidth;

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
            DisplayManager.printText(textX, 6, notify.text.c_str(), false);
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
            DisplayManager.printText(notify.scrollposition, 6, notify.text.c_str(), false);
        }
    }

    // Display icon if present and not pushed
    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (notify.pushIcon > 0 && !noScrolling)
        {
            if (notify.iconPosition < 0 && notify.iconWasPushed == false && notify.scrollposition > 9)
            {
                ++notify.iconPosition;
            }

            if (notify.scrollposition < 9 && !notify.iconWasPushed)
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
            matrix->drawLine(8 + notify.iconPosition, 0, 8 + notify.iconPosition, 7, 0);
        }
    }

    // Reset text color after displaying notification
    DisplayManager.getInstance().resetTextColor();
}

void CFrame1(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(1, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame2(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(2, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame3(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(3, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame4(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(4, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame5(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(5, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame6(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(6, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame7(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(7, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame8(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(8, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame9(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(9, matrix, state, x, y, firstFrame, lastFrame);
}

void CFrame10(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame)
{
    ShowCustomFrame(10, matrix, state, x, y, firstFrame, lastFrame);
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
    uint16_t textWidth = getTextWidth(text.c_str());
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

std::vector<std::pair<uint16_t, AppCallback>> Apps;
OverlayCallback overlays[] = {MenuFrame, NotifyFrame, AlarmFrame, TimerFrame};
#endif