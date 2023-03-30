#include <DisplayManager.h>
#include <FastLED_NeoMatrix.h>
#include "MatrixDisplayUi.h"
#include <TJpg_Decoder.h>
#include "icons.h"
#include "Globals.h"
#include <ArduinoJson.h>
#include "PeripheryManager.h"
#include "MQTTManager.h"
#include "GifPlayer.h"
#include <Ticker.h>
#include "Functions.h"
#include "ServerManager.h"
#include "MenuManager.h"
#include "Apps.h"

Ticker AlarmTicker;
Ticker TimerTicker;

#ifdef ULANZI
#define MATRIX_PIN 32
#else
#define MATRIX_PIN D2
#endif

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

bool DisplayManager_::setAutoTransition(bool active)
{

    if (ui.AppCount < 2)
    {
        ui.disablesetAutoTransition();
        return false;
    }
    if (active && AUTO_TRANSITION)
    {
        ui.enablesetAutoTransition();
        return true;
    }
    else
    {
        ui.disablesetAutoTransition();
        return false;
    }
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

void DisplayManager_::drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
{
    matrix.drawRGBBitmap(y, x, bitmap, w, h);
}

void DisplayManager_::applyAllSettings()
{
    ui.setTargetFPS(MATRIX_FPS);
    ui.setTimePerApp(TIME_PER_APP);
    ui.setTimePerTransition(TIME_PER_TRANSITION);
    setBrightness(BRIGHTNESS);
    setTextColor(TEXTCOLOR_565);
    setAutoTransition(AUTO_TRANSITION);
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

void DisplayManager_::printText(int16_t x, int16_t y, const char *text, bool centered, bool ignoreUppercase)
{
    if (centered)
    {
        uint16_t textWidth = getTextWidth(text, ignoreUppercase);
        int16_t textX = ((32 - textWidth) / 2);
        matrix.setCursor(textX, y);
    }
    else
    {
        matrix.setCursor(x, y);
    }

    if (UPPERCASE_LETTERS && !ignoreUppercase)
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
        xpos += getTextWidth(temp_str, false);
    }
    hueOffset++;
    if (clear)
        matrix.show();
}

void pushCustomApp(String name, int position)
{
    if (customApps.count(name) == 0)
    {
        ++customPagesCount;
        void (*customApps[10])(FastLED_NeoMatrix *, MatrixDisplayUiState *, int16_t, int16_t, bool, bool) = {CApp1, CApp2, CApp3, CApp4, CApp5, CApp6, CApp7, CApp8, CApp9, CApp10};

        if (position < 0) // Insert at the end of the vector
        {
            Apps.push_back(std::make_pair(name, customApps[customPagesCount]));
        }
        else if (position < Apps.size()) // Insert at a specific position
        {
            Apps.insert(Apps.begin() + position, std::make_pair(name, customApps[customPagesCount]));
        }
        else // Invalid position, Insert at the end of the vector
        {
            Apps.push_back(std::make_pair(name, customApps[customPagesCount]));
        }

        ui.setApps(Apps); // Add Apps
    }
}

void removeCustomApp(const String &name)
{
    auto it = std::find_if(Apps.begin(), Apps.end(), [&name](const std::pair<String, AppCallback> &appPair)
                           { return appPair.first == name; });

    if (it != Apps.end())
    {
        Apps.erase(it);
        ui.setApps(Apps);
    }
}

void DisplayManager_::generateCustomPage(String name, const char *json)
{
    if (json == "" && customApps.count(name))
    {
        customApps.erase(customApps.find(name));
        removeCustomApp(name);
        return;
    }

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return;

    CustomApp customApp;

    if (customApps.find(name) != customApps.end())
    {
        customApp = customApps[name];
    }

    if (doc.containsKey("sound"))
    {
        customApp.sound = ("/" + doc["sound"].as<String>() + ".txt");
    }
    else
    {
        customApp.sound = "";
    }

    if (doc.containsKey("bar"))
    {
        JsonArray barData = doc["bar"];
        int i = 0;
        for (JsonVariant v : barData)
        {
            if (i >= 16)
            {
                break;
            }
            customApp.barData[i] = v.as<int>();
            i++;
        }
        customApp.barSize = i;
    }
    else
    {
        customApp.barSize = 0;
    }

    int pos = doc.containsKey("pos") ? doc["pos"].as<uint8_t>() : -1;
    customApp.rainbow = doc.containsKey("rainbow") ? doc["rainbow"] : false;
    customApp.pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    customApp.name = name;
    customApp.text = utf8ascii(doc["text"].as<String>());

    if (doc.containsKey("color"))
    {
        auto color = doc["color"];
        if (color.is<String>())
        {
            customApp.color = hexToRgb565(color.as<String>());
        }
        else if (color.is<JsonArray>() && color.size() == 3)
        {
            uint8_t r = color[0];
            uint8_t g = color[1];
            uint8_t b = color[2];
            customApp.color = (r << 11) | (g << 5) | b;
        }
        else
        {
            customApp.color = TEXTCOLOR_565;
        }
    }
    else
    {
        customApp.color = TEXTCOLOR_565;
    }

    if (currentCustomApp != name)
    {
        customApp.scrollposition = 9;
    }

    customApp.repeat = doc.containsKey("repeat") ? doc["repeat"].as<uint8_t>() : -1;

    if (doc.containsKey("icon"))
    {
        String iconFileName = String(doc["icon"].as<String>());
        if (customApp.icon && String(customApp.icon.name()).startsWith(iconFileName))
        {
        }
        else
        {
            if (LittleFS.exists("/ICONS/" + iconFileName + ".jpg"))
            {
                customApp.isGif = false;
                customApp.icon = LittleFS.open("/ICONS/" + iconFileName + ".jpg");
            }
            else if (LittleFS.exists("/ICONS/" + iconFileName + ".gif"))
            {
                customApp.isGif = true;
                customApp.icon = LittleFS.open("/ICONS/" + iconFileName + ".gif");
            }
            else
            {
                fs::File nullPointer;
                customApp.icon = nullPointer;
            }
        }
    }
    else
    {
        fs::File nullPointer;
        customApp.icon = nullPointer;
    }

    pushCustomApp(name, pos - 1);
    customApps[name] = customApp;
}

void DisplayManager_::generateNotification(const char *json)
{
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, json);

    notify.duration = doc.containsKey("duration") ? doc["duration"].as<int>() * 1000 : TIME_PER_APP;
    notify.text = utf8ascii(doc["text"].as<String>());
    notify.repeat = doc.containsKey("repeat") ? doc["repeat"].as<uint16_t>() : -1;
    notify.rainbow = doc.containsKey("rainbow") ? doc["rainbow"].as<bool>() : false;
    notify.hold = doc.containsKey("hold") ? doc["hold"].as<bool>() : false;
    notify.pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    notify.flag = true;
    notify.startime = millis();
    notify.scrollposition = 9;
    notify.iconWasPushed = false;
    notify.iconPosition = 0;

    if (doc.containsKey("sound"))
    {
        PeripheryManager.playFromFile("/MELODIES/" + doc["sound"].as<String>() + ".txt");
    }

    if (doc.containsKey("bar"))
    {
        JsonArray barData = doc["bar"];
        int i = 0;
        for (JsonVariant v : barData)
        {
            if (i >= 16)
            {
                break;
            }
            notify.barData[i] = v.as<int>();
            i++;
        }
        notify.barSize = i;
    }
    else
    {
        notify.barSize = 0;
    }

    if (doc.containsKey("color"))
    {
        auto color = doc["color"];
        if (color.is<String>())
        {
            notify.color = hexToRgb565(color.as<String>());
        }
        else if (color.is<JsonArray>() && color.size() == 3)
        {
            uint8_t r = color[0];
            uint8_t g = color[1];
            uint8_t b = color[2];
            notify.color = (r << 11) | (g << 5) | b;
        }
        else
        {
            notify.color = TEXTCOLOR_565;
        }
    }
    else
    {
        notify.color = TEXTCOLOR_565;
    }

    if (doc.containsKey("icon"))
    {
        String iconFileName = doc["icon"].as<String>();
        if (LittleFS.exists("/ICONS/" + iconFileName + ".jpg"))
        {
            notify.isGif = false;
            notify.icon = LittleFS.open("/ICONS/" + iconFileName + ".jpg");
            return;
        }
        else if (LittleFS.exists("/ICONS/" + iconFileName + ".gif"))
        {
            notify.isGif = true;
            notify.icon = LittleFS.open("/ICONS/" + iconFileName + ".gif");
            return;
        }
        else
        {
            fs::File nullPointer;
            notify.icon = nullPointer;
        }
    }
    else
    {
        fs::File nullPointer;
        notify.icon = nullPointer;
    }
}

void DisplayManager_::loadNativeApps()
{
    // Define a helper function to check and update an app
    auto updateApp = [&](const String &name, AppCallback callback, bool show, size_t position)
    {
        auto it = std::find_if(Apps.begin(), Apps.end(), [&](const std::pair<String, AppCallback> &app)
                               { return app.first == name; });
        if (it != Apps.end())
        {
            if (!show)
            {
                Apps.erase(it);
            }
        }
        else
        {
            if (show)
            {
                if (position >= Apps.size())
                {
                    Apps.push_back(std::make_pair(name, callback));
                }
                else
                {
                    Apps.insert(Apps.begin() + position, std::make_pair(name, callback));
                }
            }
        }
    };

    // Update the "time" app at position 0
    updateApp("time", TimeApp, SHOW_TIME, 0);

    // Update the "date" app at position 1
    updateApp("date", DateApp, SHOW_DATE, 1);

    // Update the "temp" app at position 2
    updateApp("temp", TempApp, SHOW_TEMP, 2);

    // Update the "hum" app at position 3
    updateApp("hum", HumApp, SHOW_HUM, 3);

    // Update the "bat" app at position 4
    updateApp("bat", BatApp, SHOW_BAT, 4);

    ui.setApps(Apps);

    setAutoTransition(true);
}

void DisplayManager_::setup()
{
    TJpgDec.setCallback(jpg_output);
    FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
    gif.setMatrix(&matrix);
    ui.setAppAnimation(SLIDE_DOWN);
    ui.setOverlays(overlays, 4);
    setAutoTransition(AUTO_TRANSITION);
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
        if (ui.getUiState()->appState == IN_TRANSITION && !appIsSwitching)
        {
            appIsSwitching = true;
            MQTTManager.setCurrentApp(CURRENT_APP);
        }
        else if (ui.getUiState()->appState == FIXED && appIsSwitching)
        {
            appIsSwitching = false;
            MQTTManager.setCurrentApp(CURRENT_APP);
        }
    }
}

void DisplayManager_::clear()
{
    matrix.clear();
}

void DisplayManager_::show()
{
    matrix.show();
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

void DisplayManager_::switchToApp(const char *json)
{
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return;
    String name = doc["name"].as<String>();
    bool fast = doc["fast"] | false;
    int index = findAppIndexByName(name);
    if (index > -1)
        ui.transitionToApp(index);
}

void DisplayManager_::setNewSettings(const char *json)
{
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return;
    TIME_PER_APP = doc.containsKey("apptime") ? doc["apptime"] : TIME_PER_APP;
    TIME_PER_TRANSITION = doc.containsKey("transition") ? doc["transition"] : TIME_PER_TRANSITION;
    TEXTCOLOR_565 = doc.containsKey("textcolor") ? hexToRgb565(doc["textcolor"]) : TEXTCOLOR_565;
    MATRIX_FPS = doc.containsKey("fps") ? doc["fps"] : MATRIX_FPS;
    BRIGHTNESS = doc.containsKey("brightness") ? doc["brightness"] : BRIGHTNESS;
    AUTO_BRIGHTNESS = doc.containsKey("autobrightness") ? doc["autobrightness"] : AUTO_BRIGHTNESS;
    AUTO_TRANSITION = doc.containsKey("autotransition") ? doc["autotransition"] : AUTO_TRANSITION;
    applyAllSettings();
    saveSettings();
}

void DisplayManager_::drawProgressBar(int cur, int total)
{
    matrix.clear();
    int progress = (cur * 100) / total;
    char progressStr[5];
    snprintf(progressStr, 5, "%d%%", progress); // Formatieren des Prozentzeichens
    printText(0, 6, progressStr, true, false);
    int leds_for_progress = (progress * MATRIX_WIDTH * MATRIX_HEIGHT) / 100;
    matrix.drawFastHLine(0, 7, MATRIX_WIDTH, matrix.Color(100, 100, 100));
    matrix.drawFastHLine(0, 7, leds_for_progress / MATRIX_HEIGHT, matrix.Color(0, 255, 0));
    matrix.show();
}

void DisplayManager_::drawMenuIndicator(int cur, int total, uint16_t color)
{
    int menuItemWidth = 1;
    int totalWidth = total * menuItemWidth + (total - 1);
    int leftMargin = (MATRIX_WIDTH - totalWidth) / 2;
    int pixelSpacing = 1;
    for (int i = 0; i < total; i++)
    {
        int x = leftMargin + i * (menuItemWidth + pixelSpacing);
        if (i == cur)
        {
            matrix.drawLine(x, 7, x + menuItemWidth - 1, 7, color);
        }
        else
        {
            matrix.drawLine(x, 7, x + menuItemWidth - 1, 7, matrix.Color(100, 100, 100));
        }
    }
}

void DisplayManager_::drawBarChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint16_t color)
{
    int maximum = 0;
    int newData[dataSize];

    // Finde das Maximum in der Datenliste
    for (int i = 0; i < dataSize; i++)
    {
        int d = data[i];
        if (d > maximum)
        {
            maximum = d;
        }
    }

    // Berechne neue Datenwerte zwischen 0 und 8 basierend auf dem Maximum
    for (int i = 0; i < dataSize; i++)
    {
        int d = data[i];
        newData[i] = map(d, 0, maximum, 0, 8);
    }

    // Berechne die Breite der Balken basierend auf der Anzahl der Daten und der Breite der Matrix
    int barWidth;
    if (withIcon)
    {
        barWidth = ((32 - 9) / (dataSize)-1);
    }
    else
    {
        barWidth = (32 / (dataSize)-1);
    }

    // Berechne die Startposition des Graphen basierend auf dem Icon-Parameter
    int startX = withIcon ? 9 : 0;

    // Zeichne die Balken auf die Matrix
    for (int i = 0; i < dataSize; i++)
    {
        int x1 = x + startX + (barWidth + 1) * i;
        int barHeight = newData[i];
        int y1 = min(8 - barHeight, 7);
        matrix.fillRect(x1, y1 + y, barWidth, barHeight, color);
    }
}

void DisplayManager_::updateAppVector(const char *json)
{
    // Parse the JSON input
    DynamicJsonDocument doc(1024);
    auto error = deserializeJson(doc, json);
    if (error)
    {
        // If parsing fails, print an error message and return
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Create new vectors to store updated apps
    std::vector<std::pair<String, AppCallback>> newApps;
    std::vector<String> activeApps;

    // Loop through all apps in the JSON input
    for (const auto &app : doc.as<JsonArray>())
    {
        // Get the app name, active status, and position (if specified)
        String name = app["name"].as<String>();
        bool show = true;
        int position = -1;

         if (app.containsKey("show"))
        {
            show = app["show"].as<bool>();
        }
        if (app.containsKey("pos"))
        {
            position = app["pos"].as<int>();
        }

        // Find the corresponding AppCallback function based on the app name
        AppCallback callback;
        if (name == "time")
        {
            callback = TimeApp;
            SHOW_TIME = show;
        }
        else if (name == "date")
        {
            callback = DateApp;
            SHOW_DATE = show;
        }
        else if (name == "temp")
        {
            callback = TempApp;
            SHOW_TEMP = show;
        }
        else if (name == "hum")
        {
            callback = HumApp;
            SHOW_HUM = show;
        }
        else if (name == "bat")
        {
            callback = BatApp;
            SHOW_BAT = show;
        }
        else
        {
            // If the app is not one of the built-in apps, check if it's already in the vector
            int appIndex = findAppIndexByName(name);
            if (appIndex >= 0)
            {
                // The app is in the vector, so we can move it to a new position or remove it
                auto it = Apps.begin() + appIndex;
                if (show)
                {
                    if (position >= 0 && static_cast<size_t>(position) < newApps.size())
                    {
                        Apps.erase(it);
                        newApps.insert(newApps.begin() + position, std::make_pair(name, it->second));
                    }
                }
                else
                {
                    // If the app is being removed, also remove it from the customApps map
                    if (customApps.count(name))
                    {
                        customApps.erase(customApps.find(name));
                        removeCustomApp(name);
                    }
                }
            }
            continue;
        }
        if (show)
        {
            // Add the app to the new vector
            if (position >= 0 && static_cast<size_t>(position) < newApps.size())
            {
                newApps.insert(newApps.begin() + position, std::make_pair(name, callback));
            }
            else
            {
                newApps.emplace_back(name, callback);
            }
        }

        activeApps.push_back(name);
    }

    // Loop through all apps currently in the vector
    for (const auto &app : Apps)
    {
        // If the app is not in the updated activeApps vector, add it to the newApps vector
        if (std::find(activeApps.begin(), activeApps.end(), app.first) == activeApps.end())
        {
            newApps.push_back(app);
        }
    }

    // Update the apps vector, set it in the UI, and save settings
    Apps = std::move(newApps);
    ui.setApps(Apps);
}