#include <DisplayManager.h>
#include "MatrixDisplayUi.h"
#include <TJpg_Decoder.h>
#include "icons.h"
#include "Globals.h"
#include "PeripheryManager.h"
#include "MQTTManager.h"
#include "GifPlayer.h"
#include <Ticker.h>
#include "Functions.h"
#include "ServerManager.h"
#include "MenuManager.h"
#include "Apps.h"
#include "Dictionary.h"
#include <set>
#include "GifPlayer.h"
#include <ArtnetWifi.h>

Ticker AlarmTicker;
Ticker TimerTicker;

unsigned long lastArtnetStatusTime = 0;
const int numberOfChannels = 256 * 3;
// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;
#ifdef ULANZI
#define MATRIX_PIN 32
#else
#define MATRIX_PIN D2
#endif

#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
fs::File gifFile;
GifPlayer gif;

uint16_t gifX, gifY;
CRGB leds[MATRIX_WIDTH * MATRIX_HEIGHT];

// BMP Creation
const int w = 32;             // image width in pixels
const int h = 8;              // " height
const bool debugPrint = true; // print details of process over serial?

const int imgSize = w * h;
int px[w * h]; // actual pixel data (grayscale - added programatically below)

// NeoMatrix
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, 8, 8, 4, 1, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
MatrixDisplayUi *ui = new MatrixDisplayUi(matrix);

DisplayManager_ &DisplayManager_::getInstance()
{
    static DisplayManager_ instance;
    return instance;
}

DisplayManager_ &DisplayManager = DisplayManager.getInstance();

void DisplayManager_::setBrightness(int bri)
{
    bool wakeup;
    if (!notifications.empty())
    {
        wakeup = notifications[0].wakeup;
    }

    if (MATRIX_OFF && !ALARM_ACTIVE && !wakeup)
    {
        matrix->setBrightness(0);
    }
    else
    {

        matrix->setBrightness(bri);
    }
}

void DisplayManager_::setTextColor(uint16_t color)
{
    matrix->setTextColor(color);
}

bool DisplayManager_::setAutoTransition(bool active)
{
    if (ui->AppCount < 2)
    {
        ui->disablesetAutoTransition();
        return false;
    }
    if (active && AUTO_TRANSITION)
    {
        ui->enablesetAutoTransition();
        return true;
    }
    else
    {
        ui->disablesetAutoTransition();
        return false;
    }
    showGif = false;
}

void DisplayManager_::drawJPG(uint16_t x, uint16_t y, fs::File jpgFile)
{
    TJpgDec.drawFsJpg(x, y, jpgFile);
}

void DisplayManager_::drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
{
    matrix->drawRGBBitmap(y, x, bitmap, w, h);
}

void DisplayManager_::applyAllSettings()
{
    ui->setTargetFPS(MATRIX_FPS);
    ui->setTimePerApp(TIME_PER_APP);
    ui->setTimePerTransition(TIME_PER_TRANSITION);

    if (!AUTO_BRIGHTNESS)
        setBrightness(BRIGHTNESS);
    setTextColor(TEXTCOLOR_565);
    setAutoTransition(AUTO_TRANSITION);
}

void DisplayManager_::resetTextColor()
{
    matrix->setTextColor(TEXTCOLOR_565);
}

void DisplayManager_::clearMatrix()
{
    matrix->clear();
    matrix->show();
}

bool jpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    uint16_t bitmapIndex = 0;
    for (uint16_t row = 0; row < h; row++)
    {
        for (uint16_t col = 0; col < w; col++)
        {
            matrix->drawPixel(x + col, y + row, bitmap[bitmapIndex++]);
        }
    }
    return 0;
}

void DisplayManager_::printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase)
{

    if (centered)
    {
        uint16_t textWidth = getTextWidth(text, textCase);
        int16_t textX = ((32 - textWidth) / 2);
        matrix->setCursor(textX, y);
    }
    else
    {
        matrix->setCursor(x, y);
    }

    if ((UPPERCASE_LETTERS && textCase == 0) || textCase == 1)
    {
        size_t length = strlen(text);
        char upperText[length + 1]; // +1 for the null terminator

        for (size_t i = 0; i < length; ++i)
        {
            upperText[i] = toupper(text[i]);
        }

        upperText[length] = '\0'; // Null terminator
        matrix->print(upperText);
    }
    else
    {
        matrix->print(text);
    }
}

void DisplayManager_::HSVtext(int16_t x, int16_t y, const char *text, bool clear, byte textCase)
{
    if (clear)
        matrix->clear();
    static uint8_t hueOffset = 0;
    uint16_t xpos = 0;
    for (uint16_t i = 0; i < strlen(text); i++)
    {
        uint8_t hue = map(i, 0, strlen(text), 0, 255) + hueOffset;
        uint32_t textColor = hsvToRgb(hue, 255, 255);
        matrix->setTextColor(textColor);
        const char *myChar = &text[i];

        matrix->setCursor(xpos + x, y);
        if ((UPPERCASE_LETTERS && textCase == 0) || textCase == 1)
        {
            matrix->print((char)toupper(text[i]));
        }
        else
        {
            matrix->print(&text[i]);
        }
        char temp_str[2] = {'\0', '\0'};
        temp_str[0] = text[i];
        xpos += getTextWidth(temp_str, textCase);
    }
    hueOffset++;
    if (clear)
        matrix->show();
}

void pushCustomApp(String name, int position)
{
    if (customApps.count(name) == 0)
    {
        int availableCallbackIndex = -1;

        for (int i = 0; i < 20; ++i)
        {
            bool callbackUsed = false;

            for (const auto &appPair : Apps)
            {
                if (appPair.second == customAppCallbacks[i])
                {
                    callbackUsed = true;
                    break;
                }
            }

            if (!callbackUsed)
            {
                availableCallbackIndex = i;
                break;
            }
        }

        if (availableCallbackIndex == -1)
        {
            DEBUG_PRINTLN("Error adding custom app -> Maximum number of custom apps reached");
            return;
        }

        if (position < 0) // Insert at the end of the vector
        {
            Apps.push_back(std::make_pair(name, customAppCallbacks[availableCallbackIndex]));
        }
        else if (position < Apps.size()) // Insert at a specific position
        {
            Apps.insert(Apps.begin() + position, std::make_pair(name, customAppCallbacks[availableCallbackIndex]));
        }
        else // Invalid position, Insert at the end of the vector
        {
            Apps.push_back(std::make_pair(name, customAppCallbacks[availableCallbackIndex]));
        }

        ui->setApps(Apps); // Add Apps
        DisplayManager.getInstance().setAutoTransition(true);
    }
}

bool deleteCustomAppFile(const String &name)
{
    // Create the file name based on the app name
    String fileName = "/CUSTOMAPPS/" + name + ".json";

    // Check if the file exists
    if (!LittleFS.exists(fileName))
    {
        DEBUG_PRINTLN("File does not exist: " + fileName);
        return false;
    }

    // Delete the file
    if (LittleFS.remove(fileName))
    {
        DEBUG_PRINTLN("File removed successfully: " + fileName);
        return true;
    }
    else
    {
        DEBUG_PRINTLN("Failed to remove file: " + fileName);
        return false;
    }
}

void removeCustomAppFromApps(const String &name, bool setApps)
{
    // Remove apps from Apps list
    auto it = Apps.begin();
    while (it != Apps.end())
    {
        if (it->first.startsWith(name))
        {
            it = Apps.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Remove apps from customApps map
    auto mapIt = customApps.begin();
    while (mapIt != customApps.end())
    {
        if (mapIt->first.startsWith(name))
        {
            mapIt = customApps.erase(mapIt);
        }
        else
        {
            ++mapIt;
        }
    }

    if (setApps)
        ui->setApps(Apps);
    DisplayManager.getInstance().setAutoTransition(true);
    deleteCustomAppFile(name);
}

bool parseFragmentsText(const String &jsonText, std::vector<uint16_t> &colors, std::vector<String> &fragments, uint16_t standardColor)
{
    colors.clear();
    fragments.clear();
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, jsonText);
    if (error)
    {
        doc.clear();
        return false;
    }

    JsonArray fragmentArray = doc.as<JsonArray>();

    for (JsonObject fragmentObj : fragmentArray)
    {
        String textFragment = fragmentObj["t"].as<String>();
        uint16_t color;
        if (fragmentObj.containsKey("c"))
        {
            auto fragColor = fragmentObj["c"];
            color = getColorFromJsonVariant(fragColor, standardColor);
        }
        else
        {
            color = standardColor;
        }

        fragments.push_back(textFragment);
        colors.push_back(color);
    }
    doc.clear();
    return true;
}

bool DisplayManager_::parseCustomPage(const String &name, const char *json)
{
    if ((strcmp(json, "") == 0) || (strcmp(json, "{}") == 0))
    {
        removeCustomAppFromApps(name, true);
        return true;
    }
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        doc.clear();
        return false;
    }

    if (doc.is<JsonObject>())
    {
        DEBUG_PRINTLN("Single Page");
        return generateCustomPage(name, json, false);
    }
    else if (doc.is<JsonArray>())
    {
        JsonArray customPagesArray = doc.as<JsonArray>();
        int cpIndex = 0;
        for (JsonVariant customPage : customPagesArray)
        {
            Serial.printf("Multiple Page: %i", cpIndex);
            JsonObject customPageObject = customPage.as<JsonObject>();
            String customPageJson;
            serializeJson(customPageObject, customPageJson);
            generateCustomPage(name + String(cpIndex), customPageJson.c_str(), false);
            ++cpIndex;
        }
    }
    doc.clear();
    return true;
}

bool DisplayManager_::generateCustomPage(const String &name, const char *json, bool preventSave)
{
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        DEBUG_PRINTLN("PARSING FAILED");
        DEBUG_PRINTLN(error.c_str());
        doc.clear();
        return false;
    }

    CustomApp customApp;

    if (customApps.find(name) != customApps.end())
    {
        customApp = customApps[name];
    }

    customApp.progress = doc.containsKey("progress") ? doc["progress"].as<int>() : -1;

    if (doc.containsKey("background"))
    {
        auto background = doc["background"];
        customApp.background = getColorFromJsonVariant(background, 0);
    }

    if (doc.containsKey("save") && preventSave == false)
    {
        bool saveApp = doc["save"].as<bool>();
        if (saveApp)
        {
            // Create the file name based on the app name
            String fileName = "/CUSTOMAPPS/" + name + ".json";

            // Create the directory if it doesn't exist
            if (!LittleFS.exists("/CUSTOMAPPS"))
            {
                LittleFS.mkdir("/CUSTOMAPPS");
            }

            // Open the file for writing (this will overwrite the file if it already exists)
            File file = LittleFS.open(fileName, "w");
            if (!file)
            {
                DEBUG_PRINTLN("Failed to open file for writing: " + fileName);
                return false;
            }

            // Write the new custom app to the file
            serializeJson(doc, file);
            file.close();
        }
    }

    if (doc.containsKey("progressC"))
    {
        auto progressC = doc["progressC"];
        customApp.pColor = getColorFromJsonVariant(progressC, matrix->Color(0, 255, 0));
    }
    else
    {
        customApp.pColor = matrix->Color(0, 255, 0);
    }

    if (doc.containsKey("progressBC"))
    {
        auto progressBC = doc["progressBC"];
        customApp.pbColor = getColorFromJsonVariant(progressBC, matrix->Color(255, 255, 255));
    }
    else
    {
        customApp.pbColor = matrix->Color(255, 255, 255);
    }

    bool autoscale = true;
    if (doc.containsKey("autoscale"))
    {
        autoscale = doc["autoscale"].as<bool>();
    }

    if (doc.containsKey("bar"))
    {
        JsonArray barData = doc["bar"];
        int i = 0;
        int maximum = 0;
        for (JsonVariant v : barData)
        {
            if (i >= 16)
            {
                break;
            }
            int d = v.as<int>();
            if (d > maximum)
            {
                maximum = d;
            }
            customApp.barData[i] = d;
            i++;
        }
        customApp.barSize = i;

        if (autoscale)
        {
            for (int j = 0; j < customApp.barSize; j++)
            {
                customApp.barData[j] = map(customApp.barData[j], 0, maximum, 0, 8);
            }
        }
    }
    else
    {
        customApp.barSize = 0;
    }

    if (doc.containsKey("line"))
    {
        JsonArray lineData = doc["line"];
        int i = 0;
        int maximum = 0;
        for (JsonVariant v : lineData)
        {
            if (i >= 16)
            {
                break;
            }
            int d = v.as<int>();
            if (d > maximum)
            {
                maximum = d;
            }
            customApp.lineData[i] = d;
            i++;
        }
        customApp.lineSize = i;

        // Autoscaling
        if (autoscale)
        {
            for (int j = 0; j < customApp.lineSize; j++)
            {
                customApp.lineData[j] = map(customApp.lineData[j], 0, maximum, 0, 8);
            }
        }
    }
    else
    {
        customApp.lineSize = 0;
    }

    if (doc.containsKey("draw"))
    {
        customApp.drawInstructions = doc["draw"].as<String>();
    }
    else
    {
        customApp.drawInstructions = "";
    }

    customApp.effect = doc.containsKey("effect") ? getEffectIndex(doc["effect"].as<String>()) : -1;
    customApp.duration = doc.containsKey("duration") ? doc["duration"].as<long>() * 1000 : 0;
    int pos = doc.containsKey("pos") ? doc["pos"].as<uint8_t>() : -1;
    customApp.rainbow = doc.containsKey("rainbow") ? doc["rainbow"] : false;
    customApp.pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    customApp.textCase = doc.containsKey("textCase") ? doc["textCase"] : 0;
    customApp.lifetime = doc.containsKey("lifetime") ? doc["lifetime"] : 0;
    customApp.textOffset = doc.containsKey("textOffset") ? doc["textOffset"] : 0;
    customApp.scrollSpeed = doc.containsKey("scrollSpeed") ? doc["scrollSpeed"].as<int>() : -1;
    customApp.topText = doc.containsKey("topText") ? doc["topText"].as<bool>() : false;
    customApp.noScrolling = doc.containsKey("noScroll") ? doc["noScroll"] : false;
    customApp.name = name;

    customApp.lastUpdate = millis();

    if (doc.containsKey("color"))
    {
        auto color = doc["color"];
        customApp.color = getColorFromJsonVariant(color, TEXTCOLOR_565);
    }
    else
    {
        customApp.color = TEXTCOLOR_565;
    }

    if (doc.containsKey("text"))
    {
        String text = utf8ascii(doc["text"].as<String>());
        parseFragmentsText(text, customApp.colors, customApp.fragments, customApp.color);
        customApp.text = text;
    }
    else
    {
        customApp.text = "";
    }

    if (currentCustomApp != name)
    {
        customApp.scrollposition = 9 + customApp.textOffset;
    }

    customApp.repeat = doc.containsKey("repeat") ? doc["repeat"].as<int>() : -1;
    if (customApp.noScrolling)
    {
        customApp.repeat = -1;
    }

    if (doc.containsKey("icon"))
    {
        String iconFileName = String(doc["icon"].as<String>());
        if (customApp.icon && String(customApp.icon.name()).startsWith(iconFileName))
        {
            
        }
        else
        {
            if (customApp.icon)
            {
                customApp.icon.close();
            }
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
    doc.clear();
    DEBUG_PRINTLN("PARSING FINISHED");
    return true;
}

bool DisplayManager_::generateNotification(uint8_t source, const char *json)
{
    // source: 0=MQTT, 1=HTTP
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        doc.clear();
        return false;
    }

    Notification newNotification;

    newNotification.progress = doc.containsKey("progress") ? doc["progress"].as<int>() : -1;

    if (doc.containsKey("progressC"))
    {
        auto progressC = doc["progressC"];
        newNotification.pColor = getColorFromJsonVariant(progressC, matrix->Color(0, 255, 0));
    }
    else
    {
        newNotification.pColor = matrix->Color(0, 255, 0);
    }

    if (doc.containsKey("progressBC"))
    {
        auto progressBC = doc["progressBC"];
        newNotification.pbColor = getColorFromJsonVariant(progressBC, matrix->Color(255, 255, 255));
    }
    else
    {
        newNotification.pbColor = matrix->Color(255, 255, 255);
    }

    if (doc.containsKey("background"))
    {
        auto background = doc["background"];
        newNotification.background = getColorFromJsonVariant(background, 0);
    }

    if (doc.containsKey("draw"))
    {
        newNotification.drawInstructions = doc["draw"].as<String>();
    }
    else
    {
        newNotification.drawInstructions = "";
    }

    if (doc.containsKey("effect"))
    {
        newNotification.effect = getEffectIndex(doc["effect"].as<String>());
        if (doc.containsKey("effectSettings"))
        {
            updateEffectSettings(newNotification.effect, doc["effectSettings"].as<String>());
        }
    }

    newNotification.loopSound = doc.containsKey("loopSound") ? doc["loopSound"].as<bool>() : false;
    newNotification.sound = doc.containsKey("sound") ? doc["sound"].as<String>() : "";
    newNotification.rtttl = doc.containsKey("rtttl") ? doc["rtttl"].as<String>() : "";
    newNotification.duration = doc.containsKey("duration") ? doc["duration"].as<long>() * 1000 : TIME_PER_APP;
    newNotification.rainbow = doc.containsKey("rainbow") ? doc["rainbow"].as<bool>() : false;
    newNotification.hold = doc.containsKey("hold") ? doc["hold"].as<bool>() : false;
    newNotification.scrollSpeed = doc.containsKey("scrollSpeed") ? doc["scrollSpeed"].as<int>() : -1;
    newNotification.wakeup = doc.containsKey("wakeup") ? doc["wakeup"].as<bool>() : false;
    newNotification.pushIcon = doc.containsKey("pushIcon") ? doc["pushIcon"] : 0;
    newNotification.textCase = doc.containsKey("textCase") ? doc["textCase"] : 0;
    newNotification.textOffset = doc.containsKey("textOffset") ? doc["textOffset"] : 0;
    newNotification.topText = doc.containsKey("topText") ? doc["topText"].as<bool>() : false;
    newNotification.noScrolling = doc.containsKey("noScroll") ? doc["noScroll"] : false;
    newNotification.repeat = doc.containsKey("repeat") ? doc["repeat"].as<int>() : -1;
    if (newNotification.noScrolling)
    {
        newNotification.repeat = -1;
    }
    newNotification.scrollposition = 9 + newNotification.textOffset;
    newNotification.iconWasPushed = false;
    newNotification.iconPosition = 0;
    newNotification.scrollDelay = 0;

    bool autoscale = true;
    if (doc.containsKey("autoscale"))
    {
        autoscale = doc["autoscale"].as<bool>();
    }

    if (doc.containsKey("bar"))
    {
        JsonArray barData = doc["bar"];
        int i = 0;
        int maximum = 0;
        for (JsonVariant v : barData)
        {
            if (i >= 16)
            {
                break;
            }
            int d = v.as<int>();
            if (d > maximum)
            {
                maximum = d;
            }
            newNotification.barData[i] = d;
            i++;
        }
        newNotification.barSize = i;

        if (autoscale)
        {
            for (int j = 0; j < newNotification.barSize; j++)
            {
                newNotification.barData[j] = map(newNotification.barData[j], 0, maximum, 0, 8);
            }
        }
    }
    else
    {
        newNotification.barSize = 0;
    }

    if (doc.containsKey("line"))
    {
        JsonArray lineData = doc["line"];
        int i = 0;
        int maximum = 0;
        for (JsonVariant v : lineData)
        {
            if (i >= 16)
            {
                break;
            }
            int d = v.as<int>();
            if (d > maximum)
            {
                maximum = d;
            }
            newNotification.lineData[i] = d;
            i++;
        }
        newNotification.lineSize = i;

        // Autoscaling
        if (autoscale)
        {
            for (int j = 0; j < newNotification.lineSize; j++)
            {
                newNotification.lineData[j] = map(newNotification.lineData[j], 0, maximum, 0, 8);
            }
        }
    }
    else
    {
        newNotification.lineSize = 0;
    }

    if (doc.containsKey("color"))
    {
        auto color = doc["color"];
        newNotification.color = getColorFromJsonVariant(color, TEXTCOLOR_565);
    }
    else
    {
        newNotification.color = TEXTCOLOR_565;
    }

    if (doc.containsKey("text"))
    {
        String text = utf8ascii(doc["text"].as<String>());
        parseFragmentsText(text, newNotification.colors, newNotification.fragments, newNotification.color);
        newNotification.text = text;
    }
    else
    {
        newNotification.text = "";
    }

    if (doc.containsKey("icon"))
    {
        String iconFileName = doc["icon"].as<String>();
        if (LittleFS.exists("/ICONS/" + iconFileName + ".jpg"))
        {
            newNotification.isGif = false;
            newNotification.icon = LittleFS.open("/ICONS/" + iconFileName + ".jpg");
        }
        else if (LittleFS.exists("/ICONS/" + iconFileName + ".gif"))
        {
            newNotification.isGif = true;
            newNotification.icon = LittleFS.open("/ICONS/" + iconFileName + ".gif");
        }
        else
        {
            fs::File nullPointer;
            newNotification.icon = nullPointer;
        }
    }
    else
    {
        fs::File nullPointer;
        newNotification.icon = nullPointer;
    }

    if (doc.containsKey("clients"))
    {
        JsonArray ClientNames = doc["clients"];
        doc.remove("clients");
        String modifiedJson;
        serializeJson(doc, modifiedJson);
        for (JsonVariant c : ClientNames)
        {
            String client = c.as<String>();
            if (source == 0)
            {
                MQTTManager.rawPublish(client.c_str(), "notify", modifiedJson.c_str());
            }
            else
            {
                HTTPClient http;
                http.begin("http://" + client + "/api/notify");
                http.POST(modifiedJson);
                http.end();
            }
        }
    }
    newNotification.startime = millis();
    CURRENT_APP = "Notification";
    MQTTManager.setCurrentApp(CURRENT_APP);

    bool stack = doc.containsKey("stack") ? doc["stack"] : true;

    if (stack)
    {
        notifications.push_back(newNotification);
    }
    else
    {
        if (notifications.empty())
        {
            notifications.push_back(newNotification);
        }
        else
        {
            notifications[0] = newNotification;
        }
    }

    doc.clear();
    return true;
}

void DisplayManager_::loadCustomApps()
{
    File root = LittleFS.open("/CUSTOMAPPS");

    if (!root)
    {
        DEBUG_PRINTLN("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        DEBUG_PRINTLN("/CUSTOMAPPS is not a directory");
        return;
    }

    File file = root.openNextFile();

    while (file)
    {
        if (file.isDirectory())
        {
            file = root.openNextFile();
            continue;
        }

        String fileName = file.name();
        String json = "";
        while (file.available())
        {
            json += char(file.read());
        }
        file.close();

        String name = fileName.substring(fileName.lastIndexOf('/') + 1, fileName.lastIndexOf('.')); // remove path and .json extension
        generateCustomPage(name, json.c_str(), true);

        file = root.openNextFile();
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

    updateApp("time", TimeApp, SHOW_TIME, 0);
    updateApp("date", DateApp, SHOW_DATE, 1);

    if (SENSOR_READING)
    {
        updateApp("temp", TempApp, SHOW_TEMP, 2);
        updateApp("hum", HumApp, SHOW_HUM, 3);
    }
#ifdef ULANZI
    updateApp("bat", BatApp, SHOW_BAT, 4);
#endif

    ui->setApps(Apps);
    setAutoTransition(true);
}

void DisplayManager_::setup()
{
    TJpgDec.setCallback(jpg_output);
    TJpgDec.setJpgScale(1);
    random16_set_seed(millis());
    FastLED.addLeds<NEOPIXEL, MATRIX_PIN>(leds, MATRIX_WIDTH * MATRIX_HEIGHT);
    setMatrixLayout(MATRIX_LAYOUT);
    matrix->setRotation(ROTATE_SCREEN ? 90 : 0);
    if (COLOR_CORRECTION)
    {
        FastLED.setCorrection(COLOR_CORRECTION);
    }
    if (COLOR_TEMPERATURE)
    {
        FastLED.setTemperature(COLOR_TEMPERATURE);
    }
    gif.setMatrix(matrix);
    ui->setAppAnimation(SLIDE_DOWN);

    ui->setTargetFPS(MATRIX_FPS);
    ui->setTimePerApp(TIME_PER_APP);
    ui->setTimePerTransition(TIME_PER_TRANSITION);
    ui->setOverlays(overlays, 4);
    ui->setBackgroundEffect(BACKGROUND_EFFECT);
    setAutoTransition(AUTO_TRANSITION);
    ui->init();
}

void ResetCustomApps()
{
    if (customApps.empty())
    {
        return;
    }

    for (auto it = customApps.begin(); it != customApps.end(); ++it)
    {
        CustomApp &app = it->second;
        if (app.name != currentCustomApp)
        {
            app.iconWasPushed = false;
            app.scrollposition = (app.icon ? 9 : 0) + app.textOffset;
            app.iconPosition = 0;
            app.scrollDelay = 0;
        }
    }
}

void checkLifetime(uint8_t pos)
{
    if (customApps.empty())
    {
        return;
    }

    if (pos >= Apps.size())
    {
        pos = 0;
    }

    String appName = Apps[pos].first;
    auto appIt = customApps.find(appName);

    if (appIt != customApps.end())
    {
        CustomApp &app = appIt->second;
        if (app.lifetime > 0 && (millis() - app.lastUpdate) / 1000 >= app.lifetime)
        {
            DEBUG_PRINTLN("Removing " + appName + " -> Lifetime over");
            removeCustomAppFromApps(appName, false);

            DEBUG_PRINTLN("Set new Apploop");
            ui->setApps(Apps);
        }
    }
}

uint8_t received_packets = 0;
bool universe1_complete = false;
bool universe2_complete = false;

void DisplayManager_::tick()
{
    if (AP_MODE)
    {
        HSVtext(2, 6, "AP MODE", true, 1);
    }
    else if (ARTNET_MODE)
    {
        // handled by the DMXFrame callback
    }
    else if (MOODLIGHT_MODE)
    {
        // handled by the moodlight function
    }
    else
    {
        ui->update();
        if (ui->getUiState()->appState == IN_TRANSITION && !appIsSwitching)
        {
            appIsSwitching = true;
        }
        else if (ui->getUiState()->appState == FIXED && appIsSwitching)
        {

            appIsSwitching = false;
            MQTTManager.setCurrentApp(CURRENT_APP);
            setAppTime(TIME_PER_APP);
            checkLifetime(ui->getnextAppNumber());
            ResetCustomApps();
        }
    }

    if (!AP_MODE)
    {
        uint16_t ArtnetStatus = artnet.read();
        if (ArtnetStatus > 0)
        {
            lastArtnetStatusTime = millis();
            ARTNET_MODE = true;
        }
        else if (millis() - lastArtnetStatusTime > 1000)
        {
            ARTNET_MODE = false;
        }
    }
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
    sendFrame = 1;
    // set brightness of the whole strip
    if (universe == 10)
    {
        matrix->setBrightness(data[0]);
        matrix->show();
    }

    // Store which universe has got in
    if ((universe - startUniverse) < maxUniverses)
        universesReceived[universe - startUniverse] = 1;

    for (int i = 0; i < maxUniverses; i++)
    {
        if (universesReceived[i] == 0)
        {
            // Serial.println("Broke");
            sendFrame = 0;
            break;
        }
    }

    // read universe and put into the right part of the display buffer
    for (int i = 0; i < length / 3; i++)
    {
        int led = i + (universe - startUniverse) * (previousDataLength / 3);
        if (led < 256)
            matrix->drawPixel(led % matrix->width(), led / matrix->width(), matrix->Color(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]));
    }
    previousDataLength = length;

    if (sendFrame)
    {
        matrix->show();
        // Reset universeReceived to 0
        memset(universesReceived, 0, maxUniverses);
    }
}

void DisplayManager_::startArtnet()
{
    artnet.begin();
    artnet.setArtDmxCallback(onDmxFrame);
}

void DisplayManager_::clear()
{
    matrix->clear();
}

void DisplayManager_::show()
{
    matrix->show();
}

void DisplayManager_::leftButton()
{
    if (!MenuManager.inMenu)
        ui->previousApp();
}

void DisplayManager_::rightButton()
{
    if (!MenuManager.inMenu)
        ui->nextApp();
}

void DisplayManager_::nextApp()
{
    if (!MenuManager.inMenu)
    {
        DEBUG_PRINTLN(F("Switching to next app"));
        ui->nextApp();
    }
}

void DisplayManager_::previousApp()
{
    if (!MenuManager.inMenu)
    {
        DEBUG_PRINTLN(F("Switching to previous app"));
        ui->previousApp();
    }
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

        DisplayManager.getInstance().dismissNotify();

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

    bool wakeup;

    if (!notifications.empty())
    {
        wakeup = notifications[0].wakeup;
        notifications.erase(notifications.begin());
        PeripheryManager.stopSound();
    }

    if (notifications.empty())
    {
        if (wakeup && MATRIX_OFF)
        {
            DisplayManager.setBrightness(0);
        }
    }

    if (ALARM_ACTIVE)
    {
        PeripheryManager.stopSound();
        ALARM_ACTIVE = false;
    }
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
    TimerTicker.once_ms(interval, timerCallback);
}

bool DisplayManager_::switchToApp(const char *json)
{
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return false;
    String name = doc["name"].as<String>();
    bool fast = doc["fast"] | false;
    int index = findAppIndexByName(name);
    if (index > -1)
    {
        ui->transitionToApp(index);
        return true;
    }
    else
    {
        return false;
    }
}

void DisplayManager_::drawProgressBar(int16_t x, int16_t y, int progress, uint16_t pColor, uint16_t pbColor)
{
    int available_length = 32 - x;
    int leds_for_progress = (progress * available_length) / 100;
    matrix->drawFastHLine(x, y, available_length, pbColor);
    Serial.println(leds_for_progress);
    if (leds_for_progress > 0)
        matrix->drawFastHLine(x, y, leds_for_progress, pColor);
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
            matrix->drawLine(x, 7, x + menuItemWidth - 1, 7, color);
        }
        else
        {
            matrix->drawLine(x, 7, x + menuItemWidth - 1, 7, matrix->Color(100, 100, 100));
        }
    }
}

void DisplayManager_::drawBarChart(int16_t x, int16_t y, const int newData[], byte dataSize, bool withIcon, uint16_t color)
{
    int availableWidth = withIcon ? (32 - 9) : 32;
    int gap = 1;
    int totalGapsWidth = (dataSize - 1) * gap;
    int barWidth = (availableWidth - totalGapsWidth) / dataSize;
    int startX = withIcon ? 9 : 0;

    for (int i = 0; i < dataSize; i++)
    {
        int x1 = x + startX + i * (barWidth + gap);
        int barHeight = newData[i];
        int y1 = (barHeight > 0) ? (8 - barHeight) : 8;

        if (barHeight > 0)
        {
            matrix->fillRect(x1, y1 + y, barWidth, barHeight, color);
        }
    }
}

void DisplayManager_::drawLineChart(int16_t x, int16_t y, const int newData[], byte dataSize, bool withIcon, uint16_t color)
{
    int availableWidth = withIcon ? (32 - 9) : 32;
    int startX = withIcon ? 9 : 0;
    float xStep = static_cast<float>(availableWidth) / static_cast<float>(dataSize - 1);
    int lastX = x + startX;
    int lastY = y + 8 - newData[0];
    for (int i = 1; i < dataSize; i++)
    {
        int x1 = x + startX + static_cast<int>(xStep * i);
        int y1 = y + 8 - newData[i];
        matrix->drawLine(lastX, lastY, x1, y1, color);
        lastX = x1;
        lastY = y1;
    }
}

std::pair<String, AppCallback> getNativeAppByName(const String &appName)
{
    if (appName == "time")
    {
        return std::make_pair("time", TimeApp);
    }
    else if (appName == "date")
    {
        return std::make_pair("date", DateApp);
    }
    else if (appName == "temp")
    {
        return std::make_pair("temp", TempApp);
    }
    else if (appName == "hum")
    {
        return std::make_pair("hum", HumApp);
    }
#ifdef ULANZI
    else if (appName == "bat")
    {
        return std::make_pair("bat", BatApp);
    }
#endif
    return std::make_pair("", nullptr);
}

void DisplayManager_::updateAppVector(const char *json)
{
    DEBUG_PRINTLN(F("New apps vector received"));
    DEBUG_PRINTLN(json);
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        doc.clear();
        DEBUG_PRINTLN(F("Failed to parse json"));
        return;
    }

    JsonArray appArray;
    if (doc.is<JsonObject>())
    {
        JsonArray tempArray = doc.to<JsonArray>();
        tempArray.add(doc.as<JsonObject>());
        appArray = tempArray;
    }
    else if (doc.is<JsonArray>())
    {
        appArray = doc.as<JsonArray>();
    }

    for (JsonObject appObj : appArray)
    {
        String appName = appObj["name"].as<String>();
        bool show = appObj["show"].as<bool>();
        int position = appObj.containsKey("pos") ? appObj["pos"].as<int>() : Apps.size();

        auto appIt = std::find_if(Apps.begin(), Apps.end(), [&appName](const std::pair<String, AppCallback> &app)
                                  { return app.first == appName; });

        std::pair<String, AppCallback> nativeApp = getNativeAppByName(appName);

        if (!show)
        {
            if (appIt != Apps.end())
            {
                Apps.erase(appIt);
            }
        }
        else
        {
            if (nativeApp.second != nullptr)
            {
                if (appIt != Apps.end())
                {
                    Apps.erase(appIt);
                }
                position = position < 0 ? 0 : position >= Apps.size() ? Apps.size()
                                                                      : position;
                Apps.insert(Apps.begin() + position, nativeApp);
            }
            else
            {
                if (appIt != Apps.end() && appObj.containsKey("pos"))
                {
                    std::pair<String, AppCallback> app = *appIt;
                    Apps.erase(appIt);
                    position = position < 0 ? 0 : position >= Apps.size() ? Apps.size()
                                                                          : position;
                    Apps.insert(Apps.begin() + position, app);
                }
            }
        }
    }

    // Set the updated apps vector in the UI and save settings
    ui->setApps(Apps);
    saveSettings();
    sendAppLoop();
    setAutoTransition(AUTO_TRANSITION);
    doc.clear();
}

String DisplayManager_::getStats()
{
    StaticJsonDocument<512> doc;
    char buffer[20];
#ifdef ULANZI
    doc[BatKey] = BATTERY_PERCENT;
    doc[BatRawKey] = BATTERY_RAW;
#endif
    snprintf(buffer, 5, "%.0f", CURRENT_LUX);
    doc[LuxKey] = buffer;
    doc[LDRRawKey] = LDR_RAW;
    doc[RamKey] = ESP.getFreeHeap();
    doc[BrightnessKey] = BRIGHTNESS;
    if (SENSOR_READING)
    {

        snprintf(buffer, sizeof(buffer), "%.*f", TEMP_DECIMAL_PLACES, CURRENT_TEMP);
        doc[TempKey] = buffer;
        snprintf(buffer, 5, "%.0f", CURRENT_HUM);
        doc[HumKey] = buffer;
    }
    doc[UpTimeKey] = PeripheryManager.readUptime();
    doc[SignalStrengthKey] = WiFi.RSSI();
    doc[UpdateKey] = UPDATE_AVAILABLE;
    doc[MessagesKey] = RECEIVED_MESSAGES;
    doc[VersionKey] = VERSION;
    doc[F("indicator1")] = ui->indicator1State;
    doc[F("indicator2")] = ui->indicator2State;
    doc[F("indicator3")] = ui->indicator3State;
    doc[F("app")] = CURRENT_APP;
    doc[F("freeFlash")] = LittleFS.totalBytes() - LittleFS.usedBytes();
    doc[F("heap")] = ESP.getFreeHeap();
    String jsonString;
    return serializeJson(doc, jsonString), jsonString;
}

void DisplayManager_::setAppTime(long duration)
{
    ui->setTimePerApp(duration);
}

void DisplayManager_::setMatrixLayout(int layout)
{
    delete matrix; // Free memory from the current matrix object
    DEBUG_PRINTF("Set matrix layout to %i", layout);
    switch (layout)
    {
    case 0:
        matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);
        break;
    case 1:
        matrix = new FastLED_NeoMatrix(leds, 8, 8, 4, 1, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE);
        break;
    case 2:
        matrix = new FastLED_NeoMatrix(leds, 32, 8, NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);
        break;
    default:
        break;
    }

    delete ui;                        // Free memory from the current ui object
    ui = new MatrixDisplayUi(matrix); // Create a new ui object with the new matrix
}

String DisplayManager_::getAppsAsJson()
{
    DynamicJsonDocument doc(1024);
    JsonObject appsObject = doc.to<JsonObject>();
    for (size_t i = 0; i < Apps.size(); i++)
    {
        appsObject[Apps[i].first] = i;
    }
    String json;
    serializeJson(appsObject, json);
    return json;
}

void DisplayManager_::powerStateParse(const char *json)
{
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        setPower((strcmp(json, "true") == 0 || strcmp(json, "1") == 0) ? true : false);
        return;
    }

    if (doc.containsKey("power"))
    {
        bool power = doc["power"].as<bool>();
        setPower(power);
    }
}

void DisplayManager_::showSleepAnimation()
{
    matrix->setTextColor(0xFFFF);
    int steps[][2] = {{12, 8}, {13, 7}, {14, 6}, {15, 5}, {14, 4}, {13, 3}, {12, 2}, {13, 1}, {14, 0}, {15, -1}, {14, -2}, {13, -3}, {12, -4}, {13, -5}};
    int numSteps = sizeof(steps) / sizeof(steps[0]);
    for (int i = 0; i < numSteps; i++)
    {
        clear();
        printText(steps[i][0], steps[i][1], "Z", false, 1);
        show();
        delay(80);
    }
}

void DisplayManager_::showCurtainEffect()
{
    int width = 32;
    int height = 8;

    for (int i = 0; i <= width / 2; i++)
    {
        // ui->update();
        matrix->fillRect(0, 0, i, 8, 0xFFFF); // Linker Vorhang
        show();
        delay(80);
    }
}

void DisplayManager_::setPower(bool state)
{
    if (state)
    {
        MATRIX_OFF = false;
        setBrightness(BRIGHTNESS);
    }
    else
    {
        MATRIX_OFF = true;
        showSleepAnimation();
        setBrightness(0);
    }
}

void DisplayManager_::setIndicator1Color(uint16_t color)
{
    ui->setIndicator1Color(color);
}

void DisplayManager_::setIndicator1State(bool state)
{
    ui->setIndicator1State(state);
}

void DisplayManager_::setIndicator2Color(uint16_t color)
{
    ui->setIndicator2Color(color);
}

void DisplayManager_::setIndicator2State(bool state)
{
    ui->setIndicator2State(state);
}

void DisplayManager_::setIndicator3Color(uint16_t color)
{
    ui->setIndicator3Color(color);
}

void DisplayManager_::setIndicator3State(bool state)
{
    ui->setIndicator3State(state);
}

bool DisplayManager_::indicatorParser(uint8_t indicator, const char *json)
{

    if (strcmp(json, "") == 0)
    {
        switch (indicator)
        {
        case 1:
            ui->setIndicator1State(false);
            MQTTManager.setIndicatorState(1, ui->indicator1State, ui->indicator1Color);
            break;
        case 2:
            ui->setIndicator2State(false);
            MQTTManager.setIndicatorState(2, ui->indicator2State, ui->indicator2Color);
            break;
        case 3:
            ui->setIndicator3State(false);
            MQTTManager.setIndicatorState(3, ui->indicator3State, ui->indicator3Color);
            break;
        default:
            break;
        }
        return true;
    }

    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return false;

    if (doc.containsKey("color"))
    {
        auto color = doc["color"];

        uint16_t col = getColorFromJsonVariant(color, TEXTCOLOR_565);

        if (col > 0)
        {
            switch (indicator)
            {
            case 1:
                ui->setIndicator1State(true);
                ui->setIndicator1Color(col);
                break;
            case 2:
                ui->setIndicator2State(true);
                ui->setIndicator2Color(col);
                break;
            case 3:
                ui->setIndicator3State(true);
                ui->setIndicator3Color(col);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (indicator)
            {
            case 1:
                ui->setIndicator1State(false);
                break;
            case 2:
                ui->setIndicator2State(false);
                break;
            case 3:
                ui->setIndicator3State(false);
                break;
            default:
                break;
            }
        }
    }

    if (doc.containsKey("blink"))
    {
        switch (indicator)
        {
        case 1:
            ui->setIndicator1Blink(doc["blink"].as<int>());
            break;
        case 2:
            ui->setIndicator2Blink(doc["blink"].as<int>());
            break;
        case 3:
            ui->setIndicator3Blink(doc["blink"].as<int>());
            break;
        default:
            break;
        }
    }
    else
    {
        switch (indicator)
        {
        case 1:
            ui->setIndicator1Blink(0);
            break;
        case 2:
            ui->setIndicator2Blink(0);
            break;
        case 3:
            ui->setIndicator3Blink(0);
            break;
        default:
            break;
        }
    }
    MQTTManager.setIndicatorState(indicator, ui->indicator1State, ui->indicator1Color);
    return true;
}

void DisplayManager_::gammaCorrection()
{
    if (GAMMA > 0)
    {
        for (int i = 0; i < 256; i++)
        {
            leds[i] = applyGamma_video(leds[i], GAMMA);
        }
    }
}

void DisplayManager_::sendAppLoop()
{
    MQTTManager.publish("stats/loop", getAppsAsJson().c_str());
}

String DisplayManager_::getSettings()
{
    StaticJsonDocument<512> doc;
    doc["ABRI"] = AUTO_BRIGHTNESS;
    doc["BRI"] = BRIGHTNESS;
    doc["ATRANS"] = AUTO_TRANSITION;
    doc["TCOL"] = TEXTCOLOR_565;
    doc["TEFF"] = TRANS_EFFECT;
    doc["TSPEED"] = TIME_PER_TRANSITION;
    doc["ATIME"] = TIME_PER_APP / 1000;
    doc["TFORMAT"] = TIME_FORMAT;
    doc["DFORMAT"] = DATE_FORMAT;
    doc["SOM"] = START_ON_MONDAY;
    doc["CEL"] = IS_CELSIUS;
    doc["BLOCKN"] = BLOCK_NAVIGATION;
    doc["MAT"] = MATRIX_LAYOUT;
    doc["SOUND"] = SOUND_ACTIVE;
    doc["GAMMA"] = GAMMA;
    doc["UPPERCASE"] = UPPERCASE_LETTERS;
    doc["CCORRECTION"] = COLOR_CORRECTION.raw;
    doc["CTEMP"] = COLOR_TEMPERATURE.raw;
    doc["WD"] = SHOW_WEEKDAY;
    doc["WDCA"] = WDC_ACTIVE;
    doc["WDCI"] = WDC_INACTIVE;
    doc["TIME_COL"] = TIME_COLOR;
    doc["DATE_COL"] = DATE_COLOR;
    doc["HUM_COL"] = HUM_COLOR;
    doc["TEMP_COL"] = TEMP_COLOR;
    doc["BAT_COL"] = BAT_COLOR;
    doc["SSPEED"] = SCROLL_SPEED;
    String jsonString;
    return serializeJson(doc, jsonString), jsonString;
}

void DisplayManager_::setNewSettings(const char *json)
{
    DEBUG_PRINTLN(F("Got new settings:"));
    DEBUG_PRINTLN(json);
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return;

    if (doc.containsKey("ATIME"))
    {
        long atime = doc["ATIME"].as<int>();
        TIME_PER_APP = atime * 1000;
    }
    else
    {
        TIME_PER_APP = TIME_PER_APP;
    }
    TRANS_EFFECT = doc.containsKey("TEFF") ? doc["TEFF"] : TRANS_EFFECT;
    TIME_PER_TRANSITION = doc.containsKey("TSPEED") ? doc["TSPEED"] : TIME_PER_TRANSITION;
    BRIGHTNESS = doc.containsKey("BRI") ? doc["BRI"] : BRIGHTNESS;
    SCROLL_SPEED = doc.containsKey("SSPEED") ? doc["SSPEED"] : SCROLL_SPEED;
    IS_CELSIUS = doc.containsKey("CEL") ? doc["CEL"] : IS_CELSIUS;
    START_ON_MONDAY = doc.containsKey("SOM") ? doc["SOM"].as<bool>() : START_ON_MONDAY;
    TIME_FORMAT = doc.containsKey("TFORMAT") ? doc["TFORMAT"].as<String>() : TIME_FORMAT;
    GAMMA = doc.containsKey("GAMMA") ? doc["GAMMA"].as<float>() : GAMMA;
    DATE_FORMAT = doc.containsKey("DFORMAT") ? doc["DFORMAT"].as<String>() : DATE_FORMAT;
    AUTO_BRIGHTNESS = doc.containsKey("ABRI") ? doc["ABRI"].as<bool>() : AUTO_BRIGHTNESS;
    AUTO_TRANSITION = doc.containsKey("ATRANS") ? doc["ATRANS"].as<bool>() : AUTO_TRANSITION;
    UPPERCASE_LETTERS = doc.containsKey("UPPERCASE") ? doc["UPPERCASE"].as<bool>() : UPPERCASE_LETTERS;
    SHOW_WEEKDAY = doc.containsKey("WD") ? doc["WD"].as<bool>() : SHOW_WEEKDAY;
    BLOCK_NAVIGATION = doc.containsKey("BLOCKN") ? doc["BLOCKN"].as<bool>() : BLOCK_NAVIGATION;
    if (doc.containsKey("CCORRECTION"))
    {
        auto colorValue = doc["CCORRECTION"];
        if (colorValue.is<String>())
        {
            String hexColor = colorValue.as<String>();
            uint32_t rgbColor = strtoul(hexColor.c_str(), NULL, 16);
            uint8_t r = (rgbColor >> 16) & 0xFF;
            uint8_t g = (rgbColor >> 8) & 0xFF;
            uint8_t b = rgbColor & 0xFF;
            COLOR_CORRECTION.setRGB(r, g, b);
        }
        else if (colorValue.is<JsonArray>() && colorValue.size() == 3)
        {
            uint8_t r = colorValue[0];
            uint8_t g = colorValue[1];
            uint8_t b = colorValue[2];
            COLOR_CORRECTION.setRGB(r, g, b);
        }

        if (COLOR_CORRECTION)
        {
            FastLED.setCorrection(COLOR_CORRECTION);
        }
    }
    if (doc.containsKey("CTEMP"))
    {
        auto colorValue = doc["CTEMP"];
        if (colorValue.is<String>())
        {
            String hexColor = colorValue.as<String>();
            uint32_t rgbColor = strtoul(hexColor.c_str(), NULL, 16);
            uint8_t r = (rgbColor >> 16) & 0xFF;
            uint8_t g = (rgbColor >> 8) & 0xFF;
            uint8_t b = rgbColor & 0xFF;
            COLOR_TEMPERATURE.setRGB(r, g, b);
        }
        else if (colorValue.is<JsonArray>() && colorValue.size() == 3)
        {
            uint8_t r = colorValue[0];
            uint8_t g = colorValue[1];
            uint8_t b = colorValue[2];
            COLOR_TEMPERATURE.setRGB(r, g, b);
        }

        if (COLOR_TEMPERATURE)
        {
            FastLED.setTemperature(COLOR_TEMPERATURE);
        }
    }
    if (doc.containsKey("WDCA"))
    {
        auto WDCA = doc["WDCA"];
        WDC_ACTIVE = getColorFromJsonVariant(WDCA, matrix->Color(255, 255, 255));
    }
    if (doc.containsKey("WDCI"))
    {
        auto WDCI = doc["WDCI"];
        WDC_INACTIVE = getColorFromJsonVariant(WDCI, matrix->Color(120, 120, 120));
    }
    if (doc.containsKey("TCOL"))
    {
        auto TCOL = doc["TCOL"];
        TEXTCOLOR_565 = getColorFromJsonVariant(TCOL, matrix->Color(255, 255, 255));
    }
    if (doc.containsKey("TIME_COL"))
    {
        auto TIME_COL = doc["TIME_COL"];
        TIME_COLOR = getColorFromJsonVariant(TIME_COL, TEXTCOLOR_565);
    }

    if (doc.containsKey("DATE_COL"))
    {
        auto DATE_COL = doc["DATE_COL"];
        DATE_COLOR = getColorFromJsonVariant(DATE_COL, TEXTCOLOR_565);
    }

    if (doc.containsKey("TEMP_COL"))
    {
        auto TEMP_COL = doc["TEMP_COL"];
        TEMP_COLOR = getColorFromJsonVariant(TEMP_COL, TEXTCOLOR_565);
    }

    if (doc.containsKey("HUM_COL"))
    {
        auto HUM_COL = doc["HUM_COL"];
        HUM_COLOR = getColorFromJsonVariant(HUM_COL, TEXTCOLOR_565);
    }

#ifdef ULANZI
    if (doc.containsKey("BAT_COL"))
    {
        auto BAT_COL = doc["BAT_COL"];
        BAT_COLOR = getColorFromJsonVariant(BAT_COL, TEXTCOLOR_565);
    }
#endif
    applyAllSettings();
    saveSettings();
}

String DisplayManager_::ledsAsJson()
{
    StaticJsonDocument<JSON_ARRAY_SIZE(MATRIX_WIDTH * MATRIX_HEIGHT)> jsonDoc;
    JsonArray jsonColors = jsonDoc.to<JsonArray>();
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            int index = matrix->XY(x, y);
            int r = leds[index].r;
            int g = leds[index].g;
            int b = leds[index].b;
            int color = (r << 16) | (g << 8) | b;
            jsonColors.add(color);
        }
    }
    String jsonString;
    serializeJson(jsonColors, jsonString);
    return jsonString;
}

String DisplayManager_::getAppsWithIcon()
{
    DynamicJsonDocument jsonDocument(1024);
    JsonArray jsonArray = jsonDocument.to<JsonArray>();
    for (const auto &app : Apps)
    {
        JsonObject appObject = jsonArray.createNestedObject();
        appObject["name"] = app.first;

        CustomApp *customApp = getCustomAppByName(app.first);
        if (customApp != nullptr)
        {
            appObject["icon"] = customApp->icon.name();
        }
    }
    String jsonString;
    serializeJson(jsonArray, jsonString);
    return jsonString;
}

CRGB DisplayManager_::getPixelColor(int16_t x, int16_t y)
{
    int index = matrix->XY(x, y);
    return leds[index];
}

void DisplayManager_::reorderApps(const String &jsonString)
{
    StaticJsonDocument<2048> jsonDocument;
    DeserializationError error = deserializeJson(jsonDocument, jsonString);
    if (error)
    {
        return;
    }

    JsonArray jsonArray = jsonDocument.as<JsonArray>();
    std::vector<std::pair<String, AppCallback>> reorderedApps;
    for (const String &appName : jsonArray)
    {
        for (const auto &app : Apps)
        {
            if (app.first == appName)
            {
                reorderedApps.push_back(app);
                break;
            }
        }
    }
    Apps = reorderedApps;
    ui->setApps(Apps);
    ui->forceResetState();
}

void DisplayManager_::processDrawInstructions(int16_t xOffset, int16_t yOffset, String &drawInstructions)
{
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, drawInstructions);

    if (error)
    {
        Serial.println("Error parsing JSON draw instructions");
        return;
    }

    if (!doc.is<JsonArray>())
    {
        Serial.println("Invalid JSON draw instructions format");
        return;
    }

    JsonArray instructions = doc.as<JsonArray>();
    for (JsonObject instruction : instructions)
    {
        for (auto kvp : instruction)
        {
            String command = kvp.key().c_str();

            JsonArray params = kvp.value().as<JsonArray>();
            if (command == "dp")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                auto color1 = params[2];
                uint16_t color = getColorFromJsonVariant(color1, TEXTCOLOR_565);
                matrix->drawPixel(x + xOffset, y + yOffset, color);
            }
            else if (command == "dl")
            {
                int x0 = params[0].as<int>();
                int y0 = params[1].as<int>();
                int x1 = params[2].as<int>();
                int y1 = params[3].as<int>();
                auto color2 = params[4];
                uint16_t color = getColorFromJsonVariant(color2, TEXTCOLOR_565);
                matrix->drawLine(x0 + xOffset, y0 + yOffset, x1 + xOffset, y1 + yOffset, color);
            }
            else if (command == "dr")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int w = params[2].as<int>();
                int h = params[3].as<int>();
                auto color3 = params[4];
                uint16_t color = getColorFromJsonVariant(color3, TEXTCOLOR_565);
                matrix->drawRect(x + xOffset, y + yOffset, w, h, color);
            }
            else if (command == "df")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int w = params[2].as<int>();
                int h = params[3].as<int>();
                auto color4 = params[4];
                uint16_t color = getColorFromJsonVariant(color4, TEXTCOLOR_565);
                matrix->fillRect(x + xOffset, y + yOffset, w, h, color);
            }
            else if (command == "dc")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int r = params[2].as<int>();
                auto color5 = params[3];
                uint16_t color = getColorFromJsonVariant(color5, TEXTCOLOR_565);
                matrix->drawCircle(x + xOffset, y + yOffset, r, color);
            }
            else if (command == "dfc")
            {
                double x = params[0].as<double>();
                double y = params[1].as<double>();
                double r = params[2].as<double>();
                auto color6 = params[3];
                uint16_t color = getColorFromJsonVariant(color6, TEXTCOLOR_565);
                matrix->fillCircle(x + xOffset, y + yOffset, r, color);
            }
            else if (command == "dt")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                String text = params[2].as<String>();
                auto color7 = params[3];
                uint16_t color = getColorFromJsonVariant(color7, TEXTCOLOR_565);
                matrix->setCursor(x + xOffset, y + yOffset + 5);
                matrix->setTextColor(color);
                matrix->print(utf8ascii(text));
            }
            else if (command == "db")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int w = params[2].as<int>();
                int h = params[3].as<int>();

                // Create an array to hold the color data
                uint16_t bitmap[w * h];

                // Assume that params[4] is a JsonArray
                JsonArray colorArray = params[4].as<JsonArray>();

                // Load the color data into the array
                int i = 0;
                for (JsonVariant v : colorArray)
                {
                    bitmap[i] = v.as<uint16_t>();
                    i++;
                }

                // Draw the bitmap
                matrix->drawRGBBitmap(x + xOffset, y + yOffset, bitmap, w, h);
            }
        }
    }
}

bool DisplayManager_::moodlight(const char *json)
{
    if (strcmp(json, "") == 0)
    {
        MOODLIGHT_MODE = false;
        return true;
    }

    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, json);
    if (error)
        return false;

    int brightness = doc["brightness"] | BRIGHTNESS;
    matrix->setBrightness(brightness);

    if (doc.containsKey("kelvin"))
    {
        int kelvin = doc["kelvin"];
        CRGB color;
        color = kelvinToRGB(kelvin);
        color.nscale8(brightness);
        for (int i = 0; i < 256; i++)
        {
            leds[i] = color;
        }
    }
    else if (doc.containsKey("color"))
    {
        auto c = doc["color"];
        uint16_t color565 = getColorFromJsonVariant(c, TEXTCOLOR_565);
        matrix->fillScreen(color565);
    }
    else
    {
        return true;
    }

    MOODLIGHT_MODE = true;

    matrix->show();
    return true;
}

int *DisplayManager_::getLedColors()
{
    int *ledColors = new int[MATRIX_WIDTH * MATRIX_HEIGHT];
    for (int y = 0; y < MATRIX_HEIGHT; y++)
    {
        for (int x = 0; x < MATRIX_WIDTH; x++)
        {
            int index = matrix->XY(x, y);
            int r = leds[index].r;
            int g = leds[index].g;
            int b = leds[index].b;
            ledColors[y * MATRIX_WIDTH + x] = (r << 16) | (g << 8) | b;
        }
    }
    return ledColors;
}

void DisplayManager_::sendBMP(Stream &stream)
{
    // Scaling factor and grid size
    int scaleFactor = 2;
    int gridSize = 1;

    // Calculate dimensions of the new scaled image
    int scaledW = w * scaleFactor + (w - 1) * gridSize;
    int scaledH = h * scaleFactor + (h - 1) * gridSize;

    // Calculate file size for bmp header
    int rowSize = 4 * ((3 * scaledW + 3) / 4);
    int fileSize = 54 + scaledH * rowSize;

    // Create image data
    char *img = (char *)malloc(3 * scaledW * scaledH);
    memset(img, 0, 3 * scaledW * scaledH); // initialize all pixels to black

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int index = matrix->XY(x, y);
            int r = leds[index].r;
            int g = leds[index].g;
            int b = leds[index].b;

            // Write the same color to the corresponding cells in the scaled image
            for (int i = 0; i < scaleFactor; i++)
            {
                for (int j = 0; j < scaleFactor; j++)
                {
                    int scaledX = x * (scaleFactor + gridSize) + i;
                    int scaledY = y * (scaleFactor + gridSize) + j;
                    int scaledIndex = scaledY * scaledW + scaledX;
                    img[scaledIndex * 3 + 0] = (unsigned char)b; // B
                    img[scaledIndex * 3 + 1] = (unsigned char)g; // G
                    img[scaledIndex * 3 + 2] = (unsigned char)r; // R
                }
            }
        }
    }

    // Create padding
    char bmpPad[rowSize - 3 * scaledW];
    for (int i = 0; i < sizeof(bmpPad); i++)
    { // fill with 0s
        bmpPad[i] = 0;
    }

    // Create file headers
    char bmpFileHeader[14] = {'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0};
    char bmpInfoHeader[40] = {40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0};

    bmpFileHeader[2] = (unsigned char)(fileSize);
    bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
    bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
    bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

    bmpInfoHeader[4] = (unsigned char)(scaledW);
    bmpInfoHeader[5] = (unsigned char)(scaledW >> 8);
    bmpInfoHeader[6] = (unsigned char)(scaledW >> 16);
    bmpInfoHeader[7] = (unsigned char)(scaledW >> 24);
    bmpInfoHeader[8] = (unsigned char)(scaledH);
    bmpInfoHeader[9] = (unsigned char)(scaledH >> 8);
    bmpInfoHeader[10] = (unsigned char)(scaledH >> 16);
    bmpInfoHeader[11] = (unsigned char)(scaledH >> 24);

    int payloadSize = sizeof(bmpFileHeader)   // size of file header
                      + sizeof(bmpInfoHeader) // size of info header
                      + scaledH * rowSize;    // size of image data (pixel data + padding)

    // Write the file
    stream.write(bmpFileHeader, sizeof(bmpFileHeader)); // write file header
    stream.write(bmpInfoHeader, sizeof(bmpInfoHeader)); // info header

    for (int i = 0; i < scaledH; i++)
    {                                                                       // iterate image array
        stream.write(img + (scaledW * (scaledH - i - 1) * 3), 3 * scaledW); // write pixel data
        stream.write(bmpPad, (4 - (scaledW * 3) % 4) % 4);                  // and padding as needed
    }

    // Clean up
    free(img);
}

CRGB *DisplayManager_::getLeds()
{
    return leds;
}

String DisplayManager_::getEffectNamesInJson()
{
    StaticJsonDocument<1024> doc;
    JsonArray array = doc.to<JsonArray>();
    for (int i = 0; i < numOfEffects; i++)
    {
        array.add(effects[i].name);
    }
    String result;
    serializeJson(array, result);
    return result;
}