#ifndef AppS_H
#define AppS_H

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
#include <ArduinoJson.h>
#include "LookingEyes.h"
Ticker downloader;

tm timeInfo;
uint16_t nativeAppsCount;

int WEATHER_CODE;
String WEATHER_TEMP;
String WEATHER_HUM;

struct CustomApp
{
    String drawInstructions;
    int16_t scrollposition = 0;
    int16_t scrollDelay = 0;
    String text;
    uint16_t color;
    File icon;
    bool isGif;
    bool rainbow;
    bool soundPlayed;
    uint16_t duration = 0;
    String sound;
    byte textCase = 0;
    int16_t repeat = 0;
    int16_t currentRepeat = 0;
    String name;
    byte pushIcon = 0;
    int16_t iconPosition = 0;
    bool iconWasPushed = false;
    int barData[16] = {0};
    int lineData[16] = {0};
    int barSize;
    int lineSize;
    long lastUpdate;
    int16_t lifetime;
    std::vector<uint16_t> colors;
    std::vector<String> fragments;
    uint8_t textOffset;
    int progress = -1;
    uint16_t pColor;
    uint16_t background = 0;
    uint16_t pbColor;
};

String currentCustomApp;
std::map<String, CustomApp> customApps;

struct Notification
{
    String drawInstructions;
    int16_t scrollposition = 34;
    int16_t scrollDelay = 0;
    String text;
    uint16_t color;
    File icon;
    bool rainbow;
    bool isGif;
    unsigned long startime = 0;
    uint16_t duration = 0;
    int16_t repeat = -1;
    bool hold = false;
    byte textCase = 0;
    byte pushIcon = 0;
    int16_t iconPosition = 0;
    bool iconWasPushed = false;
    int barData[16] = {0};
    int lineData[16] = {0};
    int barSize;
    int lineSize;
    std::vector<uint16_t> colors;
    std::vector<String> fragments;
    uint8_t textOffset;
    int progress = -1;
    uint16_t pColor;
    uint16_t background = 0;
    uint16_t pbColor;
};
std::vector<Notification> notifications;
bool notifyFlag = false;
std::vector<std::pair<String, AppCallback>> Apps;

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

void TimeApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Time";

    if (TIME_COLOR > 0)
    {
        matrix->setTextColor(TIME_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }

    time_t now = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now);
    const char *timeformat = TIME_FORMAT.c_str();
    char t[20];
    char t2[20];
    if (timeformat[2] == ' ')
    {
        strcpy(t2, timeformat);
        if (now % 2)
        {
            t2[2] = ' ';
        }
        else
        {
            t2[2] = ':';
        }
        strftime(t, sizeof(t), t2, localtime(&now));
    }
    else
    {
        strftime(t, sizeof(t), timeformat, localtime(&now));
    }

    DisplayManager.printText(0 + x, 6 + y, t, true, false);

    if (!SHOW_WEEKDAY)
        return;
    int dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timeInfo->tm_wday + 6 + dayOffset) % 7)
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, WDC_ACTIVE);
        }
        else
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, WDC_INACTIVE);
        }
    }
}

void DateApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Date";
    if (DATE_COLOR > 0)
    {
        matrix->setTextColor(DATE_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
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
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, WDC_ACTIVE);
        }
        else
        {
            matrix->drawLine((2 + i * 4) + x, y + 7, (i * 4 + 4) + x, y + 7, WDC_INACTIVE);
        }
    }
}

void TempApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Temperature";
    if (TEMP_COLOR > 0)
    {
        matrix->setTextColor(TEMP_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    matrix->drawRGBBitmap(x, y, icon_234, 8, 8);

    if (TEMP_DECIMAL_PLACES > 0)
        matrix->setCursor(8 + x, 6 + y);
    else
        matrix->setCursor(12 + x, 6 + y);

    if (IS_CELSIUS)
    {
        matrix->print(CURRENT_TEMP, TEMP_DECIMAL_PLACES);
        matrix->print(utf8ascii("°C"));
    }
    else
    {
        double tempF = (CURRENT_TEMP * 9 / 5) + 32;
        matrix->print(tempF, TEMP_DECIMAL_PLACES);
        matrix->print(utf8ascii("°F"));
    }
}

void HumApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Humidity";
    if (HUM_COLOR > 0)
    {
        matrix->setTextColor(HUM_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    matrix->drawRGBBitmap(x, y + 1, icon_2075, 8, 8);
    matrix->setCursor(14 + x, 6 + y);
    int humidity = CURRENT_HUM;
    matrix->print(humidity);
    matrix->print("%");
}

#ifdef ULANZI
void BatApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    if (notifyFlag)
        return;
    CURRENT_APP = "Battery";
    if (BAT_COLOR > 0)
    {
        matrix->setTextColor(BAT_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }
    matrix->drawRGBBitmap(x, y, icon_1486, 8, 8);
    matrix->setCursor(14 + x, 6 + y);
    matrix->print(BATTERY_PERCENT); // Ausgabe des Ladezustands
    matrix->print("%");
}
#endif

void MenuApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    if (!MenuManager.inMenu)
        return;
    matrix->fillScreen(0);
    DisplayManager.printText(0, 6, utf8ascii(MenuManager.menutext()).c_str(), true, 2);
}

void AlarmApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    if (ALARM_ACTIVE)
    {
        matrix->fillScreen(matrix->Color(255, 0, 0));
        CURRENT_APP = "Alarm";
        uint16_t textWidth = getTextWidth("ALARM", 0);
        int16_t textX = ((32 - textWidth) / 2);
        matrix->setTextColor(0);
        matrix->setCursor(textX, 6);
        matrix->print("ALARM");
        if (ALARM_SOUND != "")
        {
            if (!PeripheryManager.isPlaying())
            {
#ifdef ULANZI
                PeripheryManager.playFromFile(ALARM_SOUND);
#else
                PeripheryManager.playFromFile(DFMINI_MP3_ALARM);
#endif
            }
        }
    }
}

void TimerApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    if (TIMER_ACTIVE)
    {
        matrix->fillScreen(matrix->Color(0, 255, 0));
        CURRENT_APP = "Timer";
        String menuText = "TIMER";
        uint16_t textWidth = getTextWidth(menuText.c_str(), 0);
        int16_t textX = ((32 - textWidth) / 2);
        matrix->setTextColor(0);
        matrix->setCursor(textX, 6);
        matrix->print(menuText);
        if (TIMER_SOUND != "")
        {
            if (!PeripheryManager.isPlaying())
            {
#ifdef ULANZI
                PeripheryManager.playFromFile(TIMER_SOUND);
#else
                PeripheryManager.playFromFile(DFMINI_MP3_TIMER);
#endif
            }
        }
    }
}

void ShowCustomApp(String name, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
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

    CURRENT_APP = ca->name;
    currentCustomApp = name;

    bool hasIcon = ca->icon;

    matrix->fillRect(x, y, 32, 8, ca->background);

    // Calculate text and available width
    uint16_t textWidth = 0;
    if (!ca->fragments.empty())
    {
        for (const auto &fragment : ca->fragments)
        {
            textWidth += getTextWidth(fragment.c_str(), ca->textCase);
        }
    }
    else
    {
        textWidth = getTextWidth(ca->text.c_str(), ca->textCase);
    }

    uint16_t availableWidth = (hasIcon) ? 24 : 32;

    bool noScrolling = textWidth <= availableWidth;

    if (ca->barSize > 0)
    {
        DisplayManager.drawBarChart(x, y, ca->barData, ca->barSize, hasIcon, ca->color);
    }
    else if (ca->lineSize > 0)
    {
        DisplayManager.drawLineChart(x, y, ca->lineData, ca->lineSize, hasIcon, ca->color);
    }
    else
    {
        if ((ca->repeat > 0) && (getTextWidth(ca->text.c_str(), ca->textCase) > availableWidth) && (state->appState == FIXED))
        {
            DisplayManager.setAutoTransition(false);
        }
        else
        {
            DisplayManager.setAutoTransition(true);
        }

        if (textWidth > availableWidth && !(state->appState == IN_TRANSITION))
        {
            if (ca->scrollposition <= -textWidth)
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
            if ((ca->scrollDelay > MATRIX_FPS * 1.2) || ((hasIcon ? ca->textOffset + 9 : ca->textOffset) > 31))
            {
                if (state->appState == FIXED)
                    --ca->scrollposition;
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

        int16_t textX = hasIcon ? ((24 - textWidth) / 2) + 9 : ((32 - textWidth) / 2);
        if (noScrolling)
        {
            ca->repeat = -1; // Disable repeat if text is too short for scrolling

            if (!ca->fragments.empty())
            {
                int16_t fragmentX = textX;
                for (size_t i = 0; i < ca->fragments.size(); ++i)
                {
                    matrix->setTextColor(ca->colors[i]);
                    DisplayManager.printText(x + fragmentX, y + 6, ca->fragments[i].c_str(), false, ca->textCase);
                    fragmentX += getTextWidth(ca->fragments[i].c_str(), ca->textCase);
                }
            }
            else
            {
                if (ca->rainbow)
                {
                    DisplayManager.HSVtext(x + textX, 6 + y, ca->text.c_str(), false, ca->textCase);
                }
                else
                {
                    // Display text
                    matrix->setTextColor(ca->color);
                    DisplayManager.printText(x + textX, y + 6, ca->text.c_str(), false, ca->textCase);
                }
            }
        }
        else
        {
            if (!ca->fragments.empty())
            {
                int16_t fragmentX = ca->scrollposition;
                for (size_t i = 0; i < ca->fragments.size(); ++i)
                {
                    matrix->setTextColor(ca->colors[i]);
                    DisplayManager.printText(x + fragmentX, y + 6, ca->fragments[i].c_str(), false, ca->textCase);
                    fragmentX += getTextWidth(ca->fragments[i].c_str(), ca->textCase);
                }
            }
            else
            {
                if (ca->rainbow)
                {
                    DisplayManager.HSVtext(x + ca->scrollposition, 6 + y, ca->text.c_str(), false, ca->textCase);
                }
                else
                {
                    matrix->setTextColor(ca->color);
                    DisplayManager.printText(x + ca->scrollposition, 6 + y, ca->text.c_str(), false, ca->textCase);
                }
            }
        }
    }

    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (ca->pushIcon > 0 && !noScrolling && ca->barSize == 0)
        {
            if (ca->iconPosition < 0 && ca->iconWasPushed == false && ca->scrollposition > 8)
            {
                if (state->appState == FIXED)
                    ++ca->iconPosition;
            }

            if (ca->scrollposition < 9 && !ca->iconWasPushed)
            {
                ca->iconPosition = ca->scrollposition - 9;

                if (ca->iconPosition <= -9)
                {
                    ca->iconWasPushed = true;
                }
            }
        }

        // Display animated GIF if enabled and App is fixed, since we have only one gifplayer instance, it looks weird when 2 apps want to draw a different gif
        if (ca->isGif)
        {
            gifPlayer->playGif(x + ca->iconPosition, y, &ca->icon);
        }
        else
        {
            // Display JPEG image
            DisplayManager.drawJPG(x + ca->iconPosition, y, ca->icon);
        }

        // Draw vertical line if text is scrolling
        if (!noScrolling)
        {
            matrix->drawLine(8 + x + ca->iconPosition, 0 + y, 8 + x + ca->iconPosition, 7 + y, 0);
        }
    }

    if (ca->drawInstructions.length() > 0)
    {
        DisplayManager.processDrawInstructions(x, y, ca->drawInstructions);
    }

    if (ca->progress > -1)
    {
        DisplayManager.drawProgressBar((hasIcon ? 9 : 0), 7 + y, ca->progress, ca->pColor, ca->pbColor);
    }

    // Reset text color
    DisplayManager.getInstance().resetTextColor();
}

static unsigned long lastTime = 0;
void EyesApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{

    CURRENT_APP = "Eyes";
    if (blinkCountdown < sizeof(blinkIndex) / sizeof(blinkIndex[0]) - 1)
    {
        matrix->drawRGBBitmap(6 + x, 0 + y, eye[blinkIndex[blinkCountdown]], 8, 8);
        matrix->drawRGBBitmap(18 + x, 0 + y, eye[blinkIndex[blinkCountdown]], 8, 8);
    }
    else
    {
        matrix->drawRGBBitmap(6 + x, 0 + y, eye[0], 8, 8);
        matrix->drawRGBBitmap(18 + x, 0 + y, eye[0], 8, 8);
    }

    blinkCountdown--;
    if (blinkCountdown == 0)
    {
        blinkCountdown = random(20, 250);
    }

    if (gazeCountdown <= gazeFrames)
    {
        gazeCountdown--;
        matrix->fillRect(newX - (dX * gazeCountdown / gazeFrames) + 6 + x, newY - (dY * gazeCountdown / gazeFrames) + y, 2, 2, 0);
        matrix->fillRect(newX - (dX * gazeCountdown / gazeFrames) + 18 + x, newY - (dY * gazeCountdown / gazeFrames) + y, 2, 2, 0);
        if (gazeCountdown == 0)
        {
            eyeX = newX;
            eyeY = newY;
            do
            {
                switch (PET_MOOD)
                {
                case 0:
                    newX = random(0, 6);
                    newY = random(0, 6);
                    dX = newX - 4;
                    dY = newY - 4;
                    break;
                case 1:
                    newX = random(0, 7);
                    newY = random(0, 7);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                case 2:
                    newX = random(1, 7);
                    newY = random(1, 4);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                case 3:
                    newX = random(0, 7);
                    newY = random(3, 7);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                }
            } while (((dX * dX + dY * dY) <= 3));
            dX = newX - eyeX;
            dY = newY - eyeY;
            gazeFrames = random(5, 9);
            gazeCountdown = random(gazeFrames, 60);
        }
    }
    else
    {
        gazeCountdown--;
        matrix->fillRect(eyeX + 6 + x, eyeY + y, 2, 2, 0);
        matrix->fillRect(eyeX + 18 + x, eyeY + y, 2, 2, 0);
    }
}

void NotifyApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
{
    // Check if notification flag is set
    if (notifications.empty())
    {
        notifyFlag = false;
        return; // Exit function if flag is not set
    }
    else
    {
        notifyFlag = true;
    }

    // Set current app name
    CURRENT_APP = "Notification";

    // Check if notification duration has expired or if repeat count is 0 and hold is not enabled
    if ((((millis() - notifications[0].startime >= notifications[0].duration) && notifications[0].repeat == -1) || notifications[0].repeat == 0) && !notifications[0].hold)
    {
        // Reset notification flags and exit function
        DEBUG_PRINTLN("Notification deleted");
        if (notifications.size() >= 2)
        {
            notifications[1].startime = millis();
        }
        notifications.erase(notifications.begin());
        return;
    }

    // Check if notification has an icon
    bool hasIcon = notifications[0].icon;

    // Clear the matrix display
    matrix->fillRect(0, 0, 32, 8, notifications[0].background);

    // Calculate text and available width
    uint16_t textWidth = 0;
    if (!notifications[0].fragments.empty())
    {
        for (const auto &fragment : notifications[0].fragments)
        {
            textWidth += getTextWidth(fragment.c_str(), notifications[0].textCase);
        }
    }
    else
    {
        textWidth = getTextWidth(notifications[0].text.c_str(), notifications[0].textCase);
    }

    uint16_t availableWidth = hasIcon ? 24 : 32;

    // Check if text is scrolling
    bool noScrolling = textWidth <= availableWidth;
    if (notifications[0].barSize > 0)
    {
        DisplayManager.drawBarChart(0, 0, notifications[0].barData, notifications[0].barSize, hasIcon, notifications[0].color);
    }
    else if (notifications[0].lineSize > 0)
    {
        DisplayManager.drawLineChart(0, 0, notifications[0].lineData, notifications[0].lineSize, hasIcon, notifications[0].color);
    }
    else
    {
        // Check if text needs to be scrolled
        if (textWidth > availableWidth && notifications[0].scrollposition <= -textWidth)
        {
            // Reset scroll position and icon position if needed
            notifications[0].scrollDelay = 0;
            notifications[0].scrollposition = 9 + notifications[0].textOffset;

            if (notifications[0].pushIcon == 2)
            {
                notifications[0].iconWasPushed = false;
            }

            if (notifications[0].repeat > 0)
            {
                --notifications[0].repeat;
                if (notifications[0].repeat == 0)
                    return;
            }
        }

        if (!noScrolling)
        {
            if ((notifications[0].scrollDelay > MATRIX_FPS * 1.2) || ((hasIcon ? notifications[0].textOffset + 9 : notifications[0].textOffset) > 31))
            {
                --notifications[0].scrollposition;
            }
            else
            {
                ++notifications[0].scrollDelay;
                if (hasIcon)
                {
                    if (notifications[0].iconWasPushed && notifications[0].pushIcon == 1)
                    {
                        notifications[0].scrollposition = 0 + notifications[0].textOffset;
                    }
                    else
                    {
                        notifications[0].scrollposition = 9 + notifications[0].textOffset;
                    }
                }
                else
                {
                    notifications[0].scrollposition = 0 + notifications[0].textOffset;
                }
            }
        }

        // Calculate text X position based on icon presence
        int16_t textX = hasIcon ? ((24 - textWidth) / 2) + 9 : ((32 - textWidth) / 2);

        // Set text color
        matrix->setTextColor(notifications[0].color);

        if (noScrolling)
        {
            // Disable repeat if text is not scrolling
            notifications[0].repeat = -1;

            if (!notifications[0].fragments.empty())
            {
                int16_t fragmentX = textX;
                for (size_t i = 0; i < notifications[0].fragments.size(); ++i)
                {
                    matrix->setTextColor(notifications[0].colors[i]);
                    DisplayManager.printText(fragmentX, 6, notifications[0].fragments[i].c_str(), false, notifications[0].textCase);
                    fragmentX += getTextWidth(notifications[0].fragments[i].c_str(), notifications[0].textCase);
                }
            }
            else
            {
                if (notifications[0].rainbow)
                {
                    DisplayManager.HSVtext(textX, 6, notifications[0].text.c_str(), false, notifications[0].textCase);
                }
                else
                {
                    DisplayManager.printText(textX, 6, notifications[0].text.c_str(), false, notifications[0].textCase);
                }
            }
        }
        else
        {
            if (!notifications[0].fragments.empty())
            {
                int16_t fragmentX = notifications[0].scrollposition;
                for (size_t i = 0; i < notifications[0].fragments.size(); ++i)
                {
                    matrix->setTextColor(notifications[0].colors[i]);
                    DisplayManager.printText(fragmentX, 6, notifications[0].fragments[i].c_str(), false, notifications[0].textCase);
                    fragmentX += getTextWidth(notifications[0].fragments[i].c_str(), notifications[0].textCase);
                }
            }
            else
            {
                if (notifications[0].rainbow)
                {
                    // Display scrolling text in rainbow color if enabled
                    DisplayManager.HSVtext(notifications[0].scrollposition, 6, notifications[0].text.c_str(), false, notifications[0].textCase);
                }
                else
                {
                    // Display scrolling text in solid color
                    DisplayManager.printText(notifications[0].scrollposition, 6, notifications[0].text.c_str(), false, notifications[0].textCase);
                }
            }
        }
    }

    // Display icon if present and not pushed
    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (notifications[0].pushIcon > 0 && !noScrolling && notifications[0].barSize == 0)
        {
            if (notifications[0].iconPosition < 0 && notifications[0].iconWasPushed == false && notifications[0].scrollposition > 8)
            {
                ++notifications[0].iconPosition;
            }

            if (notifications[0].scrollposition < 9 && !notifications[0].iconWasPushed)
            {
                notifications[0].iconPosition = notifications[0].scrollposition - 9;

                if (notifications[0].iconPosition <= -9)
                {
                    notifications[0].iconWasPushed = true;
                }
            }
        }

        // Display animated GIF if
        if (notifications[0].isGif)
        {
            // Display GIF if present

            gifPlayer->playGif(notifications[0].iconPosition, 0, &notifications[0].icon);
        }
        else
        {
            // Display JPG image if present
            DisplayManager.drawJPG(notifications[0].iconPosition, 0, notifications[0].icon);
        }

        // Display icon divider line if text is scrolling
        if (!noScrolling)
        {
            matrix->drawLine(8 + notifications[0].iconPosition, 0, 8 + notifications[0].iconPosition, 7, 0);
        }
    }

    if (notifications[0].progress > -1)
    {
        DisplayManager.drawProgressBar((hasIcon ? 9 : 0), 7, notifications[0].progress, notifications[0].pColor, notifications[0].pbColor);
    }

    if (notifications[0].drawInstructions.length() > 0)
    {
        DisplayManager.processDrawInstructions(0, 0, notifications[0].drawInstructions);
    }

    // Reset text color after displaying notification
    DisplayManager.getInstance().resetTextColor();
}

// Unattractive to have a function for every customapp wich does the same, but currently still no other option found TODO

void CApp1(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp1);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp2(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp2);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp3(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp3);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp4(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp4);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp5(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp5);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp6(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp6);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp7(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp7);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp8(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp8);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp9(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp9);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp10(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp10);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp11(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp11);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp12(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp12);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp13(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp13);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp14(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp14);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp15(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp15);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp16(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp16);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp17(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp17);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp18(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp18);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp19(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp19);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

void CApp20(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    String name = getAppNameByFunction(CApp20);
    ShowCustomApp(name, matrix, state, x, y, firstFrame, lastFrame, gifPlayer);
}

OverlayCallback overlays[] = {MenuApp, NotifyApp, AlarmApp, TimerApp};
void (*customAppCallbacks[20])(FastLED_NeoMatrix *, MatrixDisplayUiState *, int16_t, int16_t, bool, bool, GifPlayer *) = {CApp1, CApp2, CApp3, CApp4, CApp5, CApp6, CApp7, CApp8, CApp9, CApp10, CApp11, CApp12, CApp13, CApp14, CApp15, CApp16, CApp17, CApp18, CApp19, CApp20};
#endif