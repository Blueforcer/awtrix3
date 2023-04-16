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

Ticker downloader;

tm timeInfo;
uint16_t nativeAppsCount;
uint16_t customPagesCount;

int WEATHER_CODE;
String WEATHER_TEMP;
String WEATHER_HUM;

struct CustomApp
{
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
};

String currentCustomApp;
std::map<String, CustomApp> customApps;

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
};

Notification notify;

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
    if (notify.flag)
        return;
    CURRENT_APP = "Time";
    DisplayManager.getInstance().resetTextColor();
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
    if (notify.flag)
        return;
    CURRENT_APP = "Temperature";
    DisplayManager.getInstance().resetTextColor();
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
    if (notify.flag)
        return;
    CURRENT_APP = "Humidity";
    DisplayManager.getInstance().resetTextColor();
    matrix->drawRGBBitmap(x, y + 1, icon_2075, 8, 8);
    matrix->setCursor(14 + x, 6 + y);
    int humidity = CURRENT_HUM; // Humidity without decimal places
    matrix->print(humidity);    // Output humidity
    matrix->print("%");
}

#ifdef ULANZI
void BatApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    if (notify.flag)
        return;
    CURRENT_APP = "Battery";
    DisplayManager.getInstance().resetTextColor();
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
    DisplayManager.printText(0, 6, utf8ascii(MenuManager.menutext()).c_str(), true, true);
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
                PeripheryManager.playFromFile("/MELODIES/" + ALARM_SOUND + ".txt");
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
                PeripheryManager.playFromFile("/MELODIES/" + TIMER_SOUND + ".txt");
#else
                PeripheryManager.playFromFile(DFMINI_MP3_TIMER);
#endif
            }
        }
    }
}

void ShowCustomApp(String name, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, bool firstFrame, bool lastFrame, GifPlayer *gifPlayer)
{
    // Abort if notify.flag is set
    if (notify.flag)
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

    // reset custom App properties if last frame
    if (lastFrame)
    {
        ca->iconWasPushed = false;
        ca->scrollposition = 9 + ca->textOffset;
        ca->iconPosition = 0;
        ca->scrollDelay = 0;
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
                ca->scrollDelay = 0;
                ca->scrollposition = 9 + ca->textOffset;
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
            }
        }

        if (!noScrolling)
        {
            if ((ca->scrollDelay > MATRIX_FPS * 1.2) || ((hasIcon ? ca->textOffset + 9 : ca->textOffset) > 31))
            {
                --ca->scrollposition;
            }
            else
            {
                ++ca->scrollDelay;
                if (hasIcon)
                {
                    if (ca->iconWasPushed && ca->pushIcon == 1)
                    {
                        ca->scrollposition = 0 + notify.textOffset;
                    }
                    else
                    {
                        ca->scrollposition = 9 + ca->textOffset;
                    }
                }
                else
                {
                    ca->scrollposition = 0 + notify.textOffset;
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
                ++ca->iconPosition;
            }

            if (ca->scrollposition < 8 && !ca->iconWasPushed)
            {
                ca->iconPosition = ca->scrollposition - 8;

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
            // matrix->drawLine(8 + x + ca->iconPosition, 0 + y, 8 + x + ca->iconPosition, 7 + y, 0);
        }
    }
    // Reset text color
    DisplayManager.getInstance().resetTextColor();
}

void NotifyApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer)
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
        DisplayManager.showGif = false;
        return;
    }

    // Check if notification has an icon
    bool hasIcon = notify.icon;

    // Clear the matrix display
    matrix->fillRect(0, 0, 32, 8, 0);

    // Calculate text and available width
    uint16_t textWidth = 0;
    if (!notify.fragments.empty())
    {
        for (const auto &fragment : notify.fragments)
        {
            textWidth += getTextWidth(fragment.c_str(), notify.textCase);
        }
    }
    else
    {
        textWidth = getTextWidth(notify.text.c_str(), notify.textCase);
    }

    uint16_t availableWidth = hasIcon ? 24 : 32;

    // Check if text is scrolling
    bool noScrolling = textWidth <= availableWidth;
    if (notify.barSize > 0)
    {
        DisplayManager.drawBarChart(0, 0, notify.barData, notify.barSize, hasIcon, notify.color);
    }
    else if (notify.lineSize > 0)
    {
        DisplayManager.drawLineChart(0, 0, notify.lineData, notify.lineSize, hasIcon, notify.color);
    }
    else
    {
        // Check if text needs to be scrolled
        if (textWidth > availableWidth && notify.scrollposition <= -textWidth)
        {
            // Reset scroll position and icon position if needed
            notify.scrollDelay = 0;
            notify.scrollposition = 9 + notify.textOffset;

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
            if ((notify.scrollDelay > MATRIX_FPS * 1.2) || ((hasIcon ? notify.textOffset + 9 : notify.textOffset) > 31))
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
                        notify.scrollposition = 0 + notify.textOffset;
                    }
                    else
                    {
                        notify.scrollposition = 9 + notify.textOffset;
                    }
                }
                else
                {
                    notify.scrollposition = 0 + notify.textOffset;
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

            if (!notify.fragments.empty())
            {
                int16_t fragmentX = textX;
                for (size_t i = 0; i < notify.fragments.size(); ++i)
                {
                    matrix->setTextColor(notify.colors[i]);
                    DisplayManager.printText(fragmentX, 6, notify.fragments[i].c_str(), false, notify.textCase);
                    fragmentX += getTextWidth(notify.fragments[i].c_str(), notify.textCase);
                }
            }
            else
            {
                if (notify.rainbow)
                {
                    DisplayManager.HSVtext(textX, 6, notify.text.c_str(), false, notify.textCase);
                }
                else
                {
                    DisplayManager.printText(textX, 6, notify.text.c_str(), false, notify.textCase);
                }
            }
        }
        else
        {
            if (!notify.fragments.empty())
            {
                int16_t fragmentX = notify.scrollposition;
                for (size_t i = 0; i < notify.fragments.size(); ++i)
                {
                    matrix->setTextColor(notify.colors[i]);
                    DisplayManager.printText(fragmentX, 6, notify.fragments[i].c_str(), false, notify.textCase);
                    fragmentX += getTextWidth(notify.fragments[i].c_str(), notify.textCase);
                }
            }
            else
            {
                if (notify.rainbow)
                {
                    // Display scrolling text in rainbow color if enabled
                    DisplayManager.HSVtext(notify.scrollposition, 6, notify.text.c_str(), false, notify.textCase);
                }
                else
                {
                    // Display scrolling text in solid color
                    DisplayManager.printText(notify.scrollposition, 6, notify.text.c_str(), false, notify.textCase);
                }
            }
        }
    }

    // Display icon if present and not pushed
    if (hasIcon)
    {
        // Push icon if enabled and text is scrolling
        if (notify.pushIcon > 0 && !noScrolling && notify.barSize == 0)
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

            gifPlayer->playGif(notify.iconPosition, 0, &notify.icon);
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
#endif