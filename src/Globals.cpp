#include "Globals.h"
#include "Preferences.h"
#include <WiFi.h>
#include <ArduinoJson.h>

#include <LittleFS.h>

Preferences Settings;

char *getID()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char *macStr = new char[24];
    snprintf(macStr, 24, "awtrix_%02x%02x%02x", mac[3], mac[4], mac[5]);
    return macStr;
}

void startLittleFS()
{
    if (LittleFS.begin())
    {
#ifdef ULANZI
        LittleFS.mkdir("/MELODIES");
#endif
        LittleFS.mkdir("/ICONS");
    }
    else
    {
        Serial.println("ERROR on mounting LittleFS. It will be formmatted!");
        LittleFS.format();
        ESP.restart();
    }
}

void loadDevSettings()
{
    Serial.println("loadSettings");
    if (LittleFS.exists("/dev.json"))
    {
        File file = LittleFS.open("/dev.json", "r");
        DynamicJsonDocument doc(128);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            Serial.println(F("Failed to read dev settings"));
            return;
        }

        if (doc.containsKey("bootsound"))
        {
            BOOT_SOUND = doc["bootsound"].as<String>();
        }

        if (doc.containsKey("matrix"))
        {
            MATRIX_LAYOUT = doc["matrix"];
        }

        if (doc.containsKey("uppercase"))
        {
            UPPERCASE_LETTERS = doc["uppercase"].as<bool>();
        }

        if (doc.containsKey("temp_dec_places"))
        {
            TEMP_DECIMAL_PLACES = doc["temp_dec_places"].as<int>();
        }

        if (doc.containsKey("gamma"))
        {
            GAMMA = doc["gamma"].as<float>();
        }

        if (doc.containsKey("color_correction"))
        {
            auto correction = doc["color_correction"];
            if (correction.is<JsonArray>() && correction.size() == 3)
            {
                uint8_t r = correction[0];
                uint8_t g = correction[1];
                uint8_t b = correction[2];
                COLOR_CORRECTION.setRGB(r, g, b);
            }
        }

        if (doc.containsKey("color_temperature"))
        {
            auto temperature = doc["color_temperature"];
            if (temperature.is<JsonArray>() && temperature.size() == 3)
            {
                uint8_t r = temperature[0];
                uint8_t g = temperature[1];
                uint8_t b = temperature[2];
                COLOR_TEMPERATURE.setRGB(r, g, b);
            }
        }
        file.close();
    }
}

void loadSettings()
{
    startLittleFS();
    Settings.begin("awtrix", false);
    MATRIX_FPS = Settings.getUInt("FPS", 23);
    BRIGHTNESS = Settings.getUInt("BRI", 120);
    AUTO_BRIGHTNESS = Settings.getBool("ABRI", false);
    TEXTCOLOR_565 = Settings.getUInt("TCOL", 0xFFFF);
    WDC_ACTIVE = Settings.getUInt("WDCA", 0xFFFF);
    WDC_INACTIVE = Settings.getUInt("WDCI", 0x6B6D);
    AUTO_TRANSITION = Settings.getBool("ATRANS", true);
    SHOW_WEEKDAY = Settings.getBool("WD", true);
    TIME_PER_TRANSITION = Settings.getUInt("TSPEED", 400);
    TIME_PER_APP = Settings.getUInt("ATIME", 7000);
    TIME_FORMAT = Settings.getString("TFORMAT", "%H:%M:%S");
    DATE_FORMAT = Settings.getString("DFORMAT", "%d.%m.%y");
    START_ON_MONDAY = Settings.getBool("SOM", true);
    IS_CELSIUS = Settings.getBool("CEL", true);
    SHOW_TIME = Settings.getBool("TIM", true);
    SHOW_DATE = Settings.getBool("DAT", true);
    SHOW_TEMP = Settings.getBool("TEMP", true);
    SHOW_HUM = Settings.getBool("HUM", true);
    MATRIX_LAYOUT = Settings.getUInt("MAT", 0);
#ifdef ULANZI
    SHOW_BAT = Settings.getBool("BAT", true);
#endif
    SOUND_ACTIVE = Settings.getBool("SOUND", true);
#ifndef ULANZI
    VOLUME_PERCENT = Settings.getUInt("VOL", 50);
    VOLUME = map(VOLUME_PERCENT, 0, 100, 0, 30);
#endif
    Settings.end();
    uniqueID = getID();
    MQTT_PREFIX = String(uniqueID);
    loadDevSettings();
}

void saveSettings()
{
    Settings.begin("awtrix", false);
    Settings.putUInt("FPS", MATRIX_FPS);
    Settings.putUInt("BRI", BRIGHTNESS);
    Settings.putBool("WD", SHOW_WEEKDAY);
    Settings.putBool("ABRI", AUTO_BRIGHTNESS);
    Settings.putBool("ATRANS", AUTO_TRANSITION);
    Settings.putUInt("TCOL", TEXTCOLOR_565);
    Settings.putUInt("WDCA", WDC_ACTIVE);
    Settings.putUInt("WDCI", WDC_INACTIVE);
    Settings.putUInt("TSPEED", TIME_PER_TRANSITION);
    Settings.putUInt("ATIME", TIME_PER_APP);
    Settings.putString("TFORMAT", TIME_FORMAT);
    Settings.putString("DFORMAT", DATE_FORMAT);
    Settings.putBool("SOM", START_ON_MONDAY);
    Settings.putBool("CEL", IS_CELSIUS);
    Settings.putBool("TIM", SHOW_TIME);
    Settings.putBool("DAT", SHOW_DATE);
    Settings.putBool("TEMP", SHOW_TEMP);
    Settings.putBool("HUM", SHOW_HUM);
    Settings.putUInt("MAT", MATRIX_LAYOUT);
#ifdef ULANZI
    Settings.putBool("BAT", SHOW_BAT);
#endif
    Settings.putBool("SOUND", SOUND_ACTIVE);
#ifndef ULANZI
    Settings.putUInt("VOL", VOLUME_PERCENT);
#endif
    Settings.end();
}

const char *uniqueID;
IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;
IPAddress primaryDNS;
IPAddress secondaryDNS;
const char *VERSION = "0.54";
String MQTT_HOST = "";
uint16_t MQTT_PORT = 1883;
String MQTT_USER;
String MQTT_PASS;
String MQTT_PREFIX;
String CITY = "Berlin,de";
bool IO_BROKER = false;
bool NET_STATIC = false;
bool SHOW_TIME = true;
bool SHOW_DATE = true;
bool SHOW_WEATHER = true;
#ifdef ULANZI
bool SHOW_BAT = true;
#endif
bool SHOW_TEMP = true;
bool SHOW_HUM = true;
bool SHOW_SECONDS = true;
bool SHOW_WEEKDAY = true;
String NET_IP = "192.168.178.10";
String NET_GW = "192.168.178.1";
String NET_SN = "255.255.255.0";
String NET_PDNS = "8.8.8.8";
String NET_SDNS = "1.1.1.1";
bool UPDATE_CHECK = true;
int TIME_PER_APP = 7000;
uint8_t MATRIX_FPS = 23;
int TIME_PER_TRANSITION = 400;
String NTP_SERVER = "de.pool.ntp.org";
String NTP_TZ = "CET-1CEST,M3.5.0,M10.5.0/3";
bool HA_DISCOVERY = false;

// Periphery
String CURRENT_APP;
float CURRENT_TEMP;
bool IS_CELSIUS;
float CURRENT_HUM;
float CURRENT_LUX;
int BRIGHTNESS = 120;
int BRIGHTNESS_PERCENT;
#ifdef ULANZI
uint8_t BATTERY_PERCENT;
uint16_t BATTERY_RAW;
#endif
uint16_t LDR_RAW;
String TIME_FORMAT = "%H:%M:%S";
String DATE_FORMAT = "%d.%m.%y";
bool START_ON_MONDAY;

String ALARM_SOUND;
uint8_t SNOOZE_TIME;

String TIMER_SOUND;

// Matrix States
bool AUTO_TRANSITION = false;
bool AUTO_BRIGHTNESS = true;
bool UPPERCASE_LETTERS = true;
bool AP_MODE;
bool MATRIX_OFF;
bool TIMER_ACTIVE;
bool ALARM_ACTIVE;
uint16_t TEXTCOLOR_565;
bool SOUND_ACTIVE;
String BOOT_SOUND = "";
int TEMP_DECIMAL_PLACES = 0;
#ifndef ULANZI
uint8_t VOLUME_PERCENT;
uint8_t VOLUME;
#endif
int MATRIX_LAYOUT = 0;
bool UPDATE_AVAILABLE = false;
long RECEIVED_MESSAGES;
CRGB COLOR_CORRECTION;
CRGB COLOR_TEMPERATURE;
uint16_t WDC_ACTIVE;
uint16_t WDC_INACTIVE;
bool BLOCK_NAVIGATION=false;
float GAMMA = 0;