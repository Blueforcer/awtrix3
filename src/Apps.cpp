#include "Apps.h"

#include <map>
#include "icons.h"
#include "MatrixDisplayUi.h"
#include "Functions.h"
#include "MenuManager.h"
#include "PeripheryManager.h"
#include <WiFi.h>
#include "effects.h"
#include "MQTTManager.h"
#include "Overlays.h"

tm timeInfo;
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
    if (TIME_COLOR > 0)
    {
        DisplayManager.setTextColor(TIME_COLOR);
    }
    else
    {
        DisplayManager.getInstance().resetTextColor();
    }

    time_t now = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now);
    const char *timeformat = getTimeFormat();
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

    int16_t wdPosY;
    int16_t timePosY;

    if (TIME_MODE == 1 || TIME_MODE == 2)
    {
        wdPosY = TIME_MODE == 1 ? 7 : 0;
        timePosY = TIME_MODE == 1 ? 6 : 7;
        DisplayManager.printText(12 + x, timePosY + y, t, false, 2);
        DisplayManager.drawFilledRect(x, y, 9, 8, CALENDAR_BODY_COLOR);
        DisplayManager.drawFilledRect(0 + x, 0 + y, 9, 2, CALENDAR_HEADER_COLOR);
    }
    else if (TIME_MODE == 3 || TIME_MODE == 4)
    {
        wdPosY = TIME_MODE == 3 ? 7 : 0;
        timePosY = TIME_MODE == 3 ? 6 : 7;
        DisplayManager.printText(12 + x, timePosY + y, t, false, 2);
        DisplayManager.drawFilledRect(0 + x, 0 + y, 9, 8, CALENDAR_BODY_COLOR);
        DisplayManager.drawLine(1, 0, 2, 0, 0x000000);
        DisplayManager.drawLine(6, 0, 7, 0, 0x000000);
    }
    else
    {
        wdPosY = 7;
        timePosY = 6;
        DisplayManager.printText(0 + x, timePosY + y, t, true, 2);
    }

    if (TIME_MODE > 0)
    {
        char day_str[3];
        sprintf(day_str, "%d", timeInfo->tm_mday);
        DisplayManager.setTextColor(CALENDAR_TEXT_COLOR);
        if (timeInfo->tm_mday < 10)
        {
            DisplayManager.setCursor(3 + x, 7 + y);
        }
        else
        {
            DisplayManager.setCursor(1 + x, 7 + y);
        }
        DisplayManager.matrixPrint(day_str);
        int16_t wdPosY = TIME_MODE > 0 ? 0 : 8;
        int16_t timePosY = TIME_MODE > 0 ? 6 : 0;
    }

    if (!SHOW_WEEKDAY)
        return;

    uint8_t LINE_WIDTH = TIME_MODE > 0 ? 2 : 3;
    uint8_t LINE_SPACING = 1;
    uint8_t LINE_START = TIME_MODE > 0 ? 10 : 2;
    uint8_t dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        int lineStart = LINE_START + i * (LINE_WIDTH + LINE_SPACING);
        int lineEnd = lineStart + LINE_WIDTH - 1;

        if (i == (timeInfo->tm_wday + 6 + dayOffset) % 7)
        {
            DisplayManager.drawLine(lineStart + x, y + wdPosY, lineEnd + x, y + wdPosY, WDC_ACTIVE);
        }
        else
        {
            DisplayManager.drawLine(lineStart + x, y + wdPosY, lineEnd + x, y + wdPosY, WDC_INACTIVE);
        }
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
    time_t now = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now);
    char d[20];
    strftime(d, sizeof(d), DATE_FORMAT.c_str(), localtime(&now));
    DisplayManager.printText(0 + x, 6 + y, d, true, 2);
    if (!SHOW_WEEKDAY)
        return;
    int dayOffset = START_ON_MONDAY ? 0 : 1;
    for (int i = 0; i <= 6; i++)
    {
        if (i == (timeInfo->tm_wday + 6 + dayOffset) % 7)
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
                break; // Exit loop if icon was found
            }
        }
        // ca->iconSearched = true;
    }

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
                DisplayManager.drawJPG(x + ca->iconPosition + ca->iconOffset, y, ca->icon);
                iconWidth = 8;
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
            DisplayManager.drawBarChart(x, y, ca->barData, ca->barSize, hasIcon, ca->color);
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

    if (noScrolling)
    {
        ca->repeat = -1; // Disable repeat if text is too short for scrolling

        if (!ca->fragments.empty())
        {
            int16_t fragmentX = textX + ca->textOffset;
            for (size_t i = 0; i < ca->fragments.size(); ++i)
            {
                DisplayManager.setTextColor(TextEffect(ca->colors[i], ca->fade, ca->blink));
                DisplayManager.printText(x + fragmentX, y + 6, ca->fragments[i].c_str(), false, ca->textCase);
                fragmentX += getTextWidth(ca->fragments[i].c_str(), ca->textCase);
            }
        }
        else
        {
            if (ca->rainbow)
            {
                DisplayManager.HSVtext(x + textX + ca->textOffset, 6 + y, ca->text.c_str(), false, ca->textCase);
            }
            else if (ca->gradient[0] > -1 && ca->gradient[1] > -1)
            {
                DisplayManager.GradientText(x + textX + ca->textOffset, 6 + y, ca->text.c_str(), ca->gradient[0], ca->gradient[1], false, ca->textCase);
            }
            else
            {
                DisplayManager.setTextColor(TextEffect(ca->color, ca->fade, ca->blink));
                DisplayManager.printText(x + textX + ca->textOffset, y + 6, ca->text.c_str(), false, ca->textCase);
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
                DisplayManager.setTextColor(TextEffect(ca->colors[i], ca->fade, ca->blink));
                DisplayManager.printText(x + fragmentX, y + 6, ca->fragments[i].c_str(), false, ca->textCase);
                fragmentX += getTextWidth(ca->fragments[i].c_str(), ca->textCase);
            }
        }
        else
        {
            if (ca->rainbow)
            {
                DisplayManager.HSVtext(x + ca->scrollposition + ca->textOffset, 6 + y, ca->text.c_str(), false, ca->textCase);
            }
            else if (ca->gradient[0] > -1 && ca->gradient[1] > -1)
            {
                DisplayManager.GradientText(x + ca->scrollposition + ca->textOffset, 6 + y, ca->text.c_str(), ca->gradient[0], ca->gradient[1], false, ca->textCase);
            }
            else
            {
                DisplayManager.setTextColor(TextEffect(ca->color, ca->fade, ca->blink));
                DisplayManager.printText(x + ca->scrollposition + ca->textOffset, 6 + y, ca->text.c_str(), false, ca->textCase);
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
