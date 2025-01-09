#include "Globals.h"
#include "Preferences.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "effects.h"
#include "Functions.h"
#include "MQTTManager.h"
Preferences Settings;

void saveWiFiCredentials(const String& ssid, const String& password) {
    Settings.begin("wifi", false);
    Settings.putString("ssid", ssid);
    Settings.putString("password", password);
    Settings.end();
}

void loadWiFiCredentials(String &ssid, String &password) {
    Settings.begin("wifi", true);
    ssid = Settings.getString("ssid", "");
    password = Settings.getString("password", "");
    Settings.end();
}

void convertSettings()
{

    if (!LittleFS.exists("/DoNotTouch.json") && !LittleFS.exists("/dev.json"))
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("No settings file found"));
        return;
    }

    Settings.begin("awtrix", false);
    if (LittleFS.exists("/DoNotTouch.json"))
    {
        File file = LittleFS.open("/DoNotTouch.json", "r");
        DynamicJsonDocument doc(file.size() * 1.33);
        DeserializationError error = deserializeJson(doc, file);

        if (doc["NTP Server"].is<String>())
            Settings.putString("NTP_SERVER", doc["NTP Server"].as<String>());
        if (doc["Timezone"].is<String>())
            Settings.putString("NTP_TZ", doc["Timezone"].as<String>());
        if (doc["Broker"].is<String>())
            Settings.putString("MQTT_HOST", doc["Broker"].as<String>());
        if (doc["Port"].is<uint16_t>())
            Settings.putUInt("MQTT_PORT", doc["Port"].as<uint16_t>());
        if (doc["Username"].is<String>())
            Settings.putString("MQTT_USER", doc["Username"].as<String>());
        if (doc["Password"].is<String>())
            Settings.putString("MQTT_PASS", doc["Password"].as<String>());
        if (doc["Prefix"].is<String>())
        {
            String prefix = doc["Prefix"].as<String>();
            prefix.trim();
            Settings.putString("MQTT_PREFIX", prefix);
        }
        if (doc["Static IP"].is<bool>())
            Settings.putBool("NET_STATIC", doc["Static IP"]);
        if (doc["Homeassistant Discovery"].is<bool>())
            Settings.putBool("HA_DISCOVERY", doc["Homeassistant Discovery"]);
        if (doc["Local IP"].is<String>())
            Settings.putString("NET_IP", doc["Local IP"].as<String>());
        if (doc["Gateway"].is<String>())
            Settings.putString("NET_GW", doc["Gateway"].as<String>());
        if (doc["Subnet"].is<String>())
            Settings.putString("NET_SN", doc["Subnet"].as<String>());
        if (doc["Primary DNS"].is<String>())
            Settings.putString("NET_PDNS", doc["Primary DNS"].as<String>());
        if (doc["Secondary DNS"].is<String>())
            Settings.putString("NET_SDNS", doc["Secondary DNS"].as<String>());
        if (doc["Auth Username"].is<String>())
            Settings.putString("AUTH_USER", doc["Auth Username"].as<String>());
        if (doc["Auth Password"].is<String>())
            Settings.putString("AUTH_PASS", doc["Auth Password"].as<String>());

        file.close();
        LittleFS.remove("/DoNotTouch.json");
        doc.clear();
    }

    if (LittleFS.exists("/dev.json"))
    {
        if (LittleFS.exists("/dev.json"))
        {
            File file = LittleFS.open("/dev.json", "r");
            DynamicJsonDocument doc(file.size() * 1.33);
            DeserializationError error = deserializeJson(doc, file);

            if (doc.containsKey("bootsound"))
                Settings.putString("BOOT_SOUND", doc["bootsound"].as<String>());
            if (doc.containsKey("sensor_reading"))
                Settings.putBool("SENSOR_READING", doc["sensor_reading"].as<bool>());
            if (doc.containsKey("dfplayer"))
                Settings.putBool("DFPLAYER_ACTIVE", doc["dfplayer"].as<bool>());
            if (doc.containsKey("matrix"))
                Settings.putUInt("MATRIX_LAYOUT", doc["matrix"]);
            if (doc.containsKey("mirror_screen"))
                Settings.putBool("MIRROR_SCREEN", doc["mirror_screen"].as<bool>());
            if (doc.containsKey("temp_offset"))
                Settings.putFloat("TEMP_OFFSET", doc["temp_offset"]);
            if (doc.containsKey("min_battery"))
                Settings.putFloat("MIN_BATTERY", doc["min_battery"]);
            if (doc.containsKey("max_battery"))
                Settings.putFloat("MAX_BATTERY", doc["max_battery"]);
            if (doc.containsKey("ap_timeout"))
                Settings.putUInt("AP_TIMEOUT", doc["ap_timeout"]);
            if (doc.containsKey("background_effect"))
                Settings.putUInt("B_EFFECT", getEffectIndex(doc["background_effect"].as<const char *>()));
            if (doc.containsKey("MIN_BRI"))
                Settings.putUInt("MIN_BRI", doc["MIN_BRI"]);
            if (doc.containsKey("MAX_BRI"))
                Settings.putUInt("MAX_BRI", doc["MAX_BRI"]);
            if (doc.containsKey("ldr_factor"))
                Settings.putFloat("LDR_FACTOR", doc["ldr_factor"].as<float>());
            if (doc.containsKey("ldr_gamma"))
                Settings.putFloat("LDR_GAMMA", doc["ldr_gamma"].as<float>());
            if (doc.containsKey("hum_offset"))
                Settings.putFloat("HUM_OFFSET", doc["hum_offset"]);
            if (doc.containsKey("ha_prefix"))
                Settings.putString("HA_PREFIX", doc["ha_prefix"].as<String>());
            if (doc.containsKey("stats_interval"))
                Settings.putLong("STATS_INTERVAL", doc["stats_interval"].as<long>());
            if (doc.containsKey("hostname"))
                Settings.putString("HOSTNAME", doc["hostname"].as<String>());
            if (doc.containsKey("buzzer_volume"))
                Settings.putBool("BUZ_VOL", doc["buzzer_volume"].as<bool>());
            if (doc.containsKey("web_port"))
                Settings.putUInt("WEB_PORT", doc["web_port"]);
            if (doc.containsKey("temp_dec_places"))
                Settings.putInt("TEMP_DECIMAL", doc["temp_dec_places"].as<int>());
            if (doc.containsKey("rotate_screen"))
                Settings.putBool("ROTATE_SCREEN", doc["rotate_screen"].as<bool>());
            if (doc.containsKey("debug_mode"))
                Settings.putBool("DEBUG_MODE", doc["debug_mode"].as<bool>());
            if (doc.containsKey("new_year"))
                Settings.putBool("NEWYEAR", doc["new_year"].as<bool>());
            if (doc.containsKey("swap_buttons"))
                Settings.putBool("SWAP_BUTTONS", doc["swap_buttons"].as<bool>());
            if (doc.containsKey("ldr_on_ground"))
                Settings.putBool("LDR_ON_GROUND", doc["ldr_on_ground"].as<bool>());
            if (doc.containsKey("button_callback"))
                Settings.putString("BUTTON_CALLBACK", doc["button_callback"].as<String>());
            if (doc.containsKey("C_CORRECTION"))
            {
                auto correction = doc["C_CORRECTION"];
                if (correction.is<JsonArray>() && correction.size() == 3)
                {
                    Settings.putUInt("C_CORRECTION", getColorFromJsonVariant(correction, 0));
                }
            }
            if (doc.containsKey("C_TEMPERATURE"))
            {
                auto temperature = doc["C_TEMPERATURE"];
                if (temperature.is<JsonArray>() && temperature.size() == 3)
                {
                    Settings.putUInt("C_TEMPERATURE", getColorFromJsonVariant(temperature, 0));
                }
            }
            doc.clear();
            file.close();

            LittleFS.remove("/dev.json");
        }

        Settings.end();

        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Webserver configuration loaded and saved to Settings"));

        return;
    }
    else if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Webserver configuration file does not exist"));

    return;
}

char *getID()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char *macStr = new char[24];
    snprintf(macStr, 24, "awtrix_%02x%02x%02x", mac[3], mac[4], mac[5]);
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Starting filesystem"));
    return macStr;
}

void startLittleFS()
{
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Starting filesystem"));
    if (LittleFS.begin())
    {
        if (LittleFS.exists("/config.json"))
        {
            LittleFS.rename("/config.json", "/DoNotTouch.json");
        }

#ifdef ULANZI
        LittleFS.mkdir("/MELODIES");
#endif
        LittleFS.mkdir("/ICONS");
        LittleFS.mkdir("/PALETTES");
        LittleFS.mkdir("/CUSTOMAPPS");
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Filesystem started"));
    }
    else
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Filesystem corrupt. Formatting..."));
        LittleFS.format();
        ESP.restart();
    }
}

String getSettingsAsJson()
{
    DynamicJsonDocument doc(4096);

    // Helligkeit (int)
    doc["BRI"] = BRIGHTNESS;
    // Automatische Helligkeit (bool)
    doc["ABRI"] = AUTO_BRIGHTNESS;
    // Großbuchstaben (bool)
    doc["UPPER"] = UPPERCASE_LETTERS;
    // Globale Textfarbe (uint32_t)
    doc["TCOL"] = TEXTCOLOR_888;
    // Kalenderfarbe Header (uint32_t)
    doc["CHCOL"] = CALENDAR_HEADER_COLOR;
    // Kalenderfarbe Text (uint32_t)
    doc["CTCOL"] = CALENDAR_TEXT_COLOR;
    // Kalenderfarbe Body (uint32_t)
    doc["CBCOL"] = CALENDAR_BODY_COLOR;
    // Übergangseffekt (uint8_t)
    doc["TEFF"] = TRANS_EFFECT;
    // Zeitmodus (uint8_t)
    doc["TMODE"] = TIME_MODE;
    // Farbe für Time App (uint32_t)
    doc["TIME_COL"] = TIME_COLOR;
    // Farbe für Date App (uint32_t)
    doc["DATE_COL"] = DATE_COLOR;
    // Farbe für Temperature App (uint32_t)
    doc["TEMP_COL"] = TEMP_COLOR;
    // Farbe für Huminity App (uint32_t)
    doc["HUM_COL"] = HUM_COLOR;
    // Farbe für Battery App (uint32_t)
    doc["BAT_COL"] = BAT_COLOR;
    // Farbe für Aktuellen Wochentag (uint32_t)
    doc["WDCA"] = WDC_ACTIVE;
    // Farbe für Inaktiven Wochentag (uint32_t)
    doc["WDCI"] = WDC_INACTIVE;
    // Automatischer Übergang (bool)
    doc["ATRANS"] = AUTO_TRANSITION;
    // Zeige Wochentag (bool)
    doc["WD"] = SHOW_WEEKDAY;
    // Transisitionszeit (int)
    doc["TSPEED"] = TIME_PER_TRANSITION;
    // Zeit pro App (int)
    doc["ATIME"] = TIME_PER_APP;
    // Zeitformat (String)
    doc["TFORMAT"] = TIME_FORMAT;
    // Datumsformat (String)
    doc["DFORMAT"] = DATE_FORMAT;
    // Woche startet am Montag (bool)
    doc["SOM"] = START_ON_MONDAY;
    // Tastensperre (bool)
    doc["BLOCKN"] = BLOCK_NAVIGATION;
    // Temperatur in Celsius (bool)
    doc["CEL"] = IS_CELSIUS;
    // Zeige Zeitapp (bool)
    doc["TIM"] = SHOW_TIME;
    // Zeige Datumapp (bool)
    doc["DAT"] = SHOW_DATE;
    // Zeige Temperaturapp (bool)
    doc["TEMP"] = SHOW_TEMP;
    // Zeige Luftfeuchtigkeitapp (bool)
    doc["HUM"] = SHOW_HUM;
    // Zeige Batterieapp (bool)
    doc["BAT"] = SHOW_BAT;
    // Matrixlayout (int)
    doc["MAT"] = MATRIX_LAYOUT;
    // Scrollgeschwindigkeit (int)
    doc["SSPEED"] = SCROLL_SPEED;
    // Sound aktiv (bool)
    doc["SOUND"] = SOUND_ACTIVE;
    // Soundlautstärke (uint8_t)
    doc["VOL"] = SOUND_VOLUME;
    // NTP Server (String)
    doc["NTP_SERVER"] = NTP_SERVER;
    // NTP Timezone (String)
    doc["NTP_TZ"] = NTP_TZ;
    // MQTT Broker (String)
    doc["MQTT_HOST"] = MQTT_HOST;
    // MQTT Port (uint16_t)
    doc["MQTT_PORT"] = MQTT_PORT;
    // MQTT User (String)
    doc["MQTT_USER"] = MQTT_USER;
    // MQTT Passwort (String)
    doc["MQTT_PASS"] = MQTT_PASS;
    // MQTT Prefix (String)
    doc["MQTT_PREFIX"] = MQTT_PREFIX;
    // Static IP (bool)
    doc["NET_STATIC"] = NET_STATIC;
    // Homeassistant Discovery (bool)
    doc["HA_DISCOVERY"] = HA_DISCOVERY;
    // Lokale IP (String)
    doc["NET_IP"] = NET_IP;
    // Gateway (String)
    doc["NET_GW"] = NET_GW;
    // Subnet (String)
    doc["NET_SN"] = NET_SN;
    // Primary DNS (String)
    doc["NET_PDNS"] = NET_PDNS;
    // Secondary DNS (String)
    doc["NET_SDNS"] = NET_SDNS;
    // Auth Username (String)
    doc["AUTH_USER"] = AUTH_USER;
    // Auth Passwort (String)
    doc["AUTH_PASS"] = AUTH_PASS;
    // Bootsound (String)
    doc["BOOT_SOUND"] = BOOT_SOUND;
    // Sensoren aktiv (bool)
    doc["SENSOR_READING"] = SENSOR_READING;
    // DFPlayer aktiv (bool)
    doc["DFPLAYER_ACTIVE"] = DFPLAYER_ACTIVE;
    // Matrix spiegeln (bool)
    doc["MIRROR_SCREEN"] = MIRROR_SCREEN;
    // Temperatur Offset (float)
    doc["TEMP_OFFSET"] = TEMP_OFFSET;
    // Minimale Batteriespannung (uint16_t)
    doc["MIN_BATTERY"] = MIN_BATTERY;
    // Maximale Batteriespannung (uint16_t)
    doc["MAX_BATTERY"] = MAX_BATTERY;
    // AP Timeout (uint32_t)
    doc["AP_TIMEOUT"] = AP_TIMEOUT;
    // Hintergrundeffekt (int)
    doc["B_EFFECT"] = BACKGROUND_EFFECT;
    // Minimale Helligkeit (uint8_t)
    doc["MIN_BRI"] = MIN_BRIGHTNESS;
    // Maximale Helligkeit (uint8_t)
    doc["MAX_BRI"] = MAX_BRIGHTNESS;
    // LDR Faktor (float)
    doc["LDR_FACTOR"] = LDR_FACTOR;
    // LDR Gamma (float)
    doc["LDR_GAMMA"] = LDR_GAMMA;
    // Luftfeuchtigkeit Offset (float)
    doc["HUM_OFFSET"] = HUM_OFFSET;
    // Homeassistant Prefix (String)
    doc["HA_PREFIX"] = HA_PREFIX;
    // Statistikintervall (long)
    doc["STATS_INTERVAL"] = STATS_INTERVAL;
    // Hostname (String)
    doc["HOSTNAME"] = HOSTNAME;
    // Buzzerlautstärke (bool)
    doc["BUZ_VOL"] = BUZ_VOL;
    // Webserver Port (int)
    doc["WEB_PORT"] = WEB_PORT;
    // Temperatur Dezimalstellen (int)
    doc["TEMP_DECIMAL"] = TEMP_DECIMAL_PLACES;
    // Bildschirm drehen (bool)
    doc["ROTATE_SCREEN"] = ROTATE_SCREEN;
    // Debugmodus (bool)
    doc["DEBUG_MODE"] = DEBUG_MODE;
    // Neujahrseffekt (bool)
    doc["NEWYEAR"] = NEWYEAR;
    // Tasten tauschen (bool)
    doc["SWAP_BUTTONS"] = SWAP_BUTTONS;
    // LDR auf Masse (bool)
    doc["LDR_ON_GROUND"] = LDR_ON_GROUND;
    // Button Callback (String)
    doc["BUTTON_CALLBACK"] = BUTTON_CALLBACK;
    // Farbkorrektur (uint32_t)
    doc["C_CORRECTION"] = COLOR_CORRECTION.as_uint32_t();
    // Farbtemperatur (uint32_t)
    doc["C_TEMPERATURE"] = COLOR_TEMPERATURE.as_uint32_t();
    // ID des Geräts (String)
    doc["UNIQUE_ID"] = uniqueID;

    String output;
    serializeJson(doc, output);
    return output;
}

void setSettingsFromJson(const String &json)
{
    DynamicJsonDocument doc(4096); // Speicher ggf. anpassen
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN("Failed to parse settings JSON");
        return;
    }
    Serial.println(json);
    Settings.begin("awtrix", false);
    // ==============================
    // 1) Display-Einstellungen
    // ==============================

    if (doc.containsKey("BRI"))
    {
        BRIGHTNESS = doc["BRI"].as<unsigned int>();
        Settings.putUInt("BRI", BRIGHTNESS);
    }
    if (doc.containsKey("ABRI"))
    {
        AUTO_BRIGHTNESS = doc["ABRI"].as<bool>();
        Settings.putBool("ABRI", AUTO_BRIGHTNESS);
    }
    if (doc.containsKey("UPPER"))
    {
        UPPERCASE_LETTERS = doc["UPPER"].as<bool>();
        Settings.putBool("UPPER", UPPERCASE_LETTERS);
    }
    if (doc.containsKey("TCOL"))
    {
        auto TCOL = doc["TCOL"];
        TEXTCOLOR_888 = getColorFromJsonVariant(TCOL, 0xFFFFFF);
        Settings.putUInt("TCOL", TEXTCOLOR_888);
    }
    if (doc.containsKey("CHCOL"))
    {
        auto CHCOL = doc["CHCOL"];
        CALENDAR_HEADER_COLOR = getColorFromJsonVariant(CHCOL, 0xFFFFFF);

        Settings.putUInt("CHCOL", CALENDAR_HEADER_COLOR);
    }
    if (doc.containsKey("CTCOL"))
    {
        auto CTCOL = doc["CTCOL"];
        CALENDAR_TEXT_COLOR = getColorFromJsonVariant(CTCOL, 0xFFFFFF);
        Settings.putUInt("CTCOL", CALENDAR_TEXT_COLOR);
    }
    if (doc.containsKey("CBCOL"))
    {
        auto CBCOL = doc["CBCOL"];
        CALENDAR_BODY_COLOR = getColorFromJsonVariant(CBCOL, 0xFFFFFF);
        Settings.putUInt("CBCOL", CALENDAR_BODY_COLOR);
    }
    if (doc.containsKey("TEFF"))
    {

        TRANS_EFFECT = doc["TEFF"].as<unsigned int>();
        Settings.putUInt("TEFF", TRANS_EFFECT);
    }
    if (doc.containsKey("TMODE"))
    {
        TIME_MODE = doc["TMODE"].as<unsigned int>();
        Settings.putUInt("TMODE", TIME_MODE);
    }
    if (doc.containsKey("TIME_COL"))
    {
        auto TIME_COL = doc["TIME_COL"];
        TIME_COLOR = getColorFromJsonVariant(TIME_COL, TEXTCOLOR_888);
        Settings.putUInt("TIME_COL", TIME_COLOR);
    }
    if (doc.containsKey("DATE_COL"))
    {
        auto DATE_COL = doc["DATE_COL"];
        DATE_COLOR = getColorFromJsonVariant(DATE_COL, TEXTCOLOR_888);
        Settings.putUInt("DATE_COL", DATE_COLOR);
    }
    if (doc.containsKey("TEMP_COL"))
    {
        auto TEMP_COL = doc["TEMP_COL"];
        TEMP_COLOR = getColorFromJsonVariant(TEMP_COL, TEXTCOLOR_888);
        Settings.putUInt("TEMP_COL", TEMP_COLOR);
    }
    if (doc.containsKey("HUM_COL"))
    {
        auto HUM_COL = doc["HUM_COL"];
        HUM_COLOR = getColorFromJsonVariant(HUM_COL, TEXTCOLOR_888);
        Settings.putUInt("HUM_COL", HUM_COLOR);
    }

    if (doc.containsKey("BAT_COL"))
    {
        auto B_COL = doc["BAT_COL"];
        BAT_COLOR = getColorFromJsonVariant(B_COL, 0xFFFFFF);
        Settings.putUInt("BAT_COL", BAT_COLOR);
    }

    if (doc.containsKey("WDCA"))
    {
        auto WDCA = doc["WDCA"];
        WDC_ACTIVE = getColorFromJsonVariant(WDCA, 0xFFFFFF);
        Settings.putUInt("WDCA", WDC_ACTIVE);
    }
    if (doc.containsKey("WDCI"))
    {
        auto WDCI = doc["WDCI"];
        WDC_INACTIVE = getColorFromJsonVariant(WDCI, 0x666666);
        Settings.putUInt("WDCI", WDC_INACTIVE);
    }
    if (doc.containsKey("ATRANS"))
    {
        AUTO_TRANSITION = doc["ATRANS"].as<bool>();
        Settings.putBool("ATRANS", AUTO_TRANSITION);
    }
    if (doc.containsKey("WD"))
    {
        SHOW_WEEKDAY = doc["WD"].as<bool>();
        Settings.putBool("WD", SHOW_WEEKDAY);
    }
    if (doc.containsKey("TSPEED"))
    {
        TIME_PER_TRANSITION = doc["TSPEED"].as<unsigned int>();
        Settings.putUInt("TSPEED", TIME_PER_TRANSITION);
    }
    if (doc.containsKey("ATIME"))
    {
        TIME_PER_APP = doc["ATIME"].as<unsigned int>();
        Settings.putUInt("ATIME", TIME_PER_APP);
    }
    if (doc.containsKey("TFORMAT"))
    {
        TIME_FORMAT = doc["TFORMAT"].as<String>();
        Settings.putString("TFORMAT", TIME_FORMAT);
    }
    if (doc.containsKey("DFORMAT"))
    {
        DATE_FORMAT = doc["DFORMAT"].as<String>();
        Settings.putString("DFORMAT", DATE_FORMAT);
    }
    if (doc.containsKey("SOM"))
    {
        START_ON_MONDAY = doc["SOM"].as<bool>();
        Settings.putBool("SOM", START_ON_MONDAY);
    }
    if (doc.containsKey("BLOCKN"))
    {
        BLOCK_NAVIGATION = doc["BLOCKN"].as<bool>();
        Settings.putBool("BLOCKN", BLOCK_NAVIGATION);
    }
    if (doc.containsKey("CEL"))
    {
        IS_CELSIUS = doc["CEL"].as<bool>();
        Settings.putBool("CEL", IS_CELSIUS);
    }
    if (doc.containsKey("TIM"))
    {
        SHOW_TIME = doc["TIM"].as<bool>();
        Settings.putBool("TIM", SHOW_TIME);
    }
    if (doc.containsKey("DAT"))
    {
        SHOW_DATE = doc["DAT"].as<bool>();
        Settings.putBool("DAT", SHOW_DATE);
    }
    if (doc.containsKey("TEMP"))
    {
        SHOW_TEMP = doc["TEMP"].as<bool>();
        Settings.putBool("TEMP", SHOW_TEMP);
    }
    if (doc.containsKey("HUM"))
    {
        SHOW_HUM = doc["HUM"].as<bool>();
        Settings.putBool("HUM", SHOW_HUM);
    }
    if (doc.containsKey("MAT"))
    {
        MATRIX_LAYOUT = doc["MAT"].as<unsigned int>();
        Settings.putUInt("MAT", MATRIX_LAYOUT);
    }
    if (doc.containsKey("SSPEED"))
    {
        SCROLL_SPEED = doc["SSPEED"].as<unsigned int>();
        Settings.putUInt("SSPEED", SCROLL_SPEED);
    }
    if (doc.containsKey("BAT"))
    {
        SHOW_BAT = doc["BAT"].as<bool>();
        Settings.putBool("BAT", SHOW_BAT);
    }

    if (doc.containsKey("SOUND"))
    {
        SOUND_ACTIVE = doc["SOUND"].as<bool>();
        Settings.putBool("SOUND", SOUND_ACTIVE);
    }
    if (doc.containsKey("VOL"))
    {
        SOUND_VOLUME = doc["VOL"].as<unsigned int>();
        Settings.putUInt("VOL", SOUND_VOLUME);
    }

    // ==============================
    // 2) Netzwerk / MQTT / NTP
    // ==============================
    if (doc.containsKey("NTP_SERVER"))
    {
        NTP_SERVER = doc["NTP_SERVER"].as<String>();
        Settings.putString("NTP_SERVER", NTP_SERVER);
    }
    if (doc.containsKey("NTP_TZ"))
    {
        NTP_TZ = doc["NTP_TZ"].as<String>();
        Settings.putString("NTP_TZ", NTP_TZ);
    }
    if (doc.containsKey("MQTT_HOST"))
    {
        MQTT_HOST = doc["MQTT_HOST"].as<String>();
        Settings.putString("MQTT_HOST", MQTT_HOST);
    }
    if (doc.containsKey("MQTT_PORT"))
    {
        MQTT_PORT = doc["MQTT_PORT"].as<unsigned int>();
        Settings.putUInt("MQTT_PORT", MQTT_PORT);
    }
    if (doc.containsKey("MQTT_USER"))
    {
        MQTT_USER = doc["MQTT_USER"].as<String>();
        Settings.putString("MQTT_USER", MQTT_USER);
    }
    if (doc.containsKey("MQTT_PASS"))
    {
        MQTT_PASS = doc["MQTT_PASS"].as<String>();
        Settings.putString("MQTT_PASS", MQTT_PASS);
    }
    if (doc.containsKey("MQTT_PREFIX"))
    {
        MQTT_PREFIX = doc["MQTT_PREFIX"].as<String>();
        Settings.putString("MQTT_PREFIX", MQTT_PREFIX);
    }
    if (doc.containsKey("NET_STATIC"))
    {
        NET_STATIC = doc["NET_STATIC"].as<bool>();
        Settings.putBool("NET_STATIC", NET_STATIC);
    }
    if (doc.containsKey("HA_DISCOVERY"))
    {
        HA_DISCOVERY = doc["HA_DISCOVERY"].as<bool>();
        Settings.putBool("HA_DISCOVERY", HA_DISCOVERY);
    }
    if (doc.containsKey("NET_IP"))
    {
        NET_IP = doc["NET_IP"].as<String>();
        Settings.putString("NET_IP", NET_IP);
    }
    if (doc.containsKey("NET_GW"))
    {
        NET_GW = doc["NET_GW"].as<String>();
        Settings.putString("NET_GW", NET_GW);
    }
    if (doc.containsKey("NET_SN"))
    {
        NET_SN = doc["NET_SN"].as<String>();
        Settings.putString("NET_SN", NET_SN);
    }
    if (doc.containsKey("NET_PDNS"))
    {
        NET_PDNS = doc["NET_PDNS"].as<String>();
        Settings.putString("NET_PDNS", NET_PDNS);
    }
    if (doc.containsKey("NET_SDNS"))
    {
        NET_SDNS = doc["NET_SDNS"].as<String>();
        Settings.putString("NET_SDNS", NET_SDNS);
    }
    if (doc.containsKey("AUTH_USER"))
    {
        AUTH_USER = doc["AUTH_USER"].as<String>();
        Settings.putString("AUTH_USER", AUTH_USER);
    }
    if (doc.containsKey("AUTH_PASS"))
    {
        AUTH_PASS = doc["AUTH_PASS"].as<String>();
        Settings.putString("AUTH_PASS", AUTH_PASS);
    }

    // ==============================
    // 3) Sonstige Einstellungen
    // ==============================
    if (doc.containsKey("BOOT_SOUND"))
    {
        BOOT_SOUND = doc["BOOT_SOUND"].as<String>();
        Settings.putString("BOOT_SOUND", BOOT_SOUND);
    }
    if (doc.containsKey("SENSOR_READING"))
    {
        SENSOR_READING = doc["SENSOR_READING"].as<bool>();
        Settings.putBool("SENSOR_READING", SENSOR_READING);
    }
    if (doc.containsKey("DFPLAYER_ACTIVE"))
    {
        DFPLAYER_ACTIVE = doc["DFPLAYER_ACTIVE"].as<bool>();
        Settings.putBool("DFPLAYER_ACTIVE", DFPLAYER_ACTIVE);
    }
    // MATRIX_LAYOUT haben wir oben schon mit MAT
    if (doc.containsKey("MATRIX_LAYOUT"))
    {
        MATRIX_LAYOUT = doc["MATRIX_LAYOUT"].as<unsigned int>();
        Settings.putUInt("MATRIX_LAYOUT", MATRIX_LAYOUT);
    }
    if (doc.containsKey("MIRROR_SCREEN"))
    {
        MIRROR_SCREEN = doc["MIRROR_SCREEN"].as<bool>();
        Settings.putBool("MIRROR_SCREEN", MIRROR_SCREEN);
    }
    if (doc.containsKey("TEMP_OFFSET"))
    {
        TEMP_OFFSET = doc["TEMP_OFFSET"].as<float>();
        Settings.putFloat("TEMP_OFFSET", TEMP_OFFSET);
    }
    if (doc.containsKey("MIN_BATTERY"))
    {
        MIN_BATTERY = doc["MIN_BATTERY"].as<float>();
        Settings.putFloat("MIN_BATTERY", MIN_BATTERY);
    }
    if (doc.containsKey("MAX_BATTERY"))
    {
        MAX_BATTERY = doc["MAX_BATTERY"].as<float>();
        Settings.putFloat("MAX_BATTERY", MAX_BATTERY);
    }
    if (doc.containsKey("AP_TIMEOUT"))
    {
        AP_TIMEOUT = doc["AP_TIMEOUT"].as<unsigned int>();
        Settings.putUInt("AP_TIMEOUT", AP_TIMEOUT);
    }
    if (doc.containsKey("B_EFFECT"))
    {
        BACKGROUND_EFFECT = doc["B_EFFECT"].as<unsigned int>();
        // Falls du eine Umwandlung brauchst (String -> Index),
        // dann z.B. BACKGROUND_EFFECT = getEffectIndex(...);
        Settings.putUInt("B_EFFECT", BACKGROUND_EFFECT);
    }
    if (doc.containsKey("MIN_BRI"))
    {
        MIN_BRIGHTNESS = doc["MIN_BRI"].as<unsigned int>();
        Settings.putUInt("MIN_BRI", MIN_BRIGHTNESS);
    }
    if (doc.containsKey("MAX_BRI"))
    {
        MAX_BRIGHTNESS = doc["MAX_BRI"].as<unsigned int>();
        Settings.putUInt("MAX_BRI", MAX_BRIGHTNESS);
    }
    if (doc.containsKey("LDR_FACTOR"))
    {
        LDR_FACTOR = doc["LDR_FACTOR"].as<float>();
        Settings.putFloat("LDR_FACTOR", LDR_FACTOR);
    }
    if (doc.containsKey("LDR_GAMMA"))
    {
        LDR_GAMMA = doc["LDR_GAMMA"].as<float>();
        Settings.putFloat("LDR_GAMMA", LDR_GAMMA);
    }
    if (doc.containsKey("HUM_OFFSET"))
    {
        HUM_OFFSET = doc["HUM_OFFSET"].as<unsigned int>();
        Settings.putUInt("HUM_OFFSET", HUM_OFFSET);
    }
    if (doc.containsKey("HA_PREFIX"))
    {
        HA_PREFIX = doc["HA_PREFIX"].as<String>();
        Settings.putString("HA_PREFIX", HA_PREFIX);
    }
    if (doc.containsKey("STATS_INTERVAL"))
    {
        STATS_INTERVAL = doc["STATS_INTERVAL"].as<long>();
        Settings.putLong("STATS_INTERVAL", STATS_INTERVAL);
    }
    if (doc.containsKey("HOSTNAME"))
    {
        HOSTNAME = doc["HOSTNAME"].as<String>();
        Settings.putString("HOSTNAME", HOSTNAME);
    }
    if (doc.containsKey("BUZ_VOL"))
    {
        BUZ_VOL = doc["BUZ_VOL"].as<bool>();
        Settings.putBool("BUZ_VOL", BUZ_VOL);
    }
    if (doc.containsKey("WEB_PORT"))
    {
        WEB_PORT = doc["WEB_PORT"].as<unsigned int>();
        Settings.putUInt("WEB_PORT", WEB_PORT);
    }
    if (doc.containsKey("TEMP_DECIMAL"))
    {
        TEMP_DECIMAL_PLACES = doc["TEMP_DECIMAL"].as<int>();
        Settings.putInt("TEMP_DECIMAL", TEMP_DECIMAL_PLACES);
    }
    if (doc.containsKey("ROTATE_SCREEN"))
    {
        ROTATE_SCREEN = doc["ROTATE_SCREEN"].as<bool>();
        Settings.putBool("ROTATE_SCREEN", ROTATE_SCREEN);
    }
    if (doc.containsKey("DEBUG_MODE"))
    {
        DEBUG_MODE = doc["DEBUG_MODE"].as<bool>();
        Settings.putBool("DEBUG_MODE", DEBUG_MODE);
    }
    if (doc.containsKey("NEWYEAR"))
    {
        NEWYEAR = doc["NEWYEAR"].as<bool>();
        Settings.putBool("NEWYEAR", NEWYEAR);
    }
    if (doc.containsKey("SWAP_BUTTONS"))
    {
        SWAP_BUTTONS = doc["SWAP_BUTTONS"].as<bool>();
        Settings.putBool("SWAP_BUTTONS", SWAP_BUTTONS);
    }
    if (doc.containsKey("LDR_ON_GROUND"))
    {
        LDR_ON_GROUND = doc["LDR_ON_GROUND"].as<bool>();
        Settings.putBool("LDR_ON_GROUND", LDR_ON_GROUND);
    }
    if (doc.containsKey("BUTTON_CALLBACK"))
    {
        BUTTON_CALLBACK = doc["BUTTON_CALLBACK"].as<String>();
        Settings.putString("BUTTON_CALLBACK", BUTTON_CALLBACK);
    }

    if (doc.containsKey("C_CORRECTION"))
    {
        auto correction = doc["C_CORRECTION"];
        uint32_t color = getColorFromJsonVariant(correction, 0);
        COLOR_CORRECTION = CRGB(color);
        Settings.putUInt("C_CORRECTION", color);
    }

    if (doc.containsKey("C_TEMPERATURE"))
    {
        auto correction = doc["C_TEMPERATURE"];
        uint32_t color = getColorFromJsonVariant(correction, 0);
        COLOR_TEMPERATURE = CRGB(color);
        Settings.putUInt("C_TEMPERATURE", color);
    }

    if (doc.containsKey("MQTT_ACTIVE"))
    {
        MQTT_ACTIVE = doc["MQTT_ACTIVE"].as<bool>();
        Settings.putBool("MQTT_ACTIVE", MQTT_ACTIVE);
        MQTTManager.connect(MQTT_ACTIVE);
    }

    Settings.end();
    DisplayManager.applyAllSettings();

    if (DEBUG_MODE)
        DEBUG_PRINTLN("Settings updated from JSON and stored immediately in Preferences.");
}

void formatSettings()
{
    Settings.begin("awtrix", false);
    Settings.clear();
    Settings.end();
}

void loadSettings()
{
    startLittleFS();
    convertSettings();
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Loading Usersettings"));
    Settings.begin("awtrix", false);

    BRIGHTNESS = Settings.getUInt("BRI", 120);
    AUTO_BRIGHTNESS = Settings.getBool("ABRI", false);
    UPPERCASE_LETTERS = Settings.getBool("UPPER", true);
    TEXTCOLOR_888 = Settings.getUInt("TCOL", 0xFFFFFF);
    CALENDAR_HEADER_COLOR = Settings.getUInt("CHCOL", 0xFF0000);
    CALENDAR_TEXT_COLOR = Settings.getUInt("CTCOL", 0x000000);
    CALENDAR_BODY_COLOR = Settings.getUInt("CBCOL", 0xFFFFFF);
    TRANS_EFFECT = Settings.getUInt("TEFF", 1);
    TIME_MODE = Settings.getUInt("TMODE", 1);
    TIME_COLOR = Settings.getUInt("TIME_COL", 0);
    DATE_COLOR = Settings.getUInt("DATE_COL", 0);
    TEMP_COLOR = Settings.getUInt("TEMP_COL", 0);
    HUM_COLOR = Settings.getUInt("HUM_COL", 0);
    BAT_COLOR = Settings.getUInt("BAT_COL", 0);
    WDC_ACTIVE = Settings.getUInt("WDCA", 0xFFFFFF);
    WDC_INACTIVE = Settings.getUInt("WDCI", 0x666666);
    AUTO_TRANSITION = Settings.getBool("ATRANS", true);
    SHOW_WEEKDAY = Settings.getBool("WD", true);
    TIME_PER_TRANSITION = Settings.getUInt("TSPEED", 400);
    TIME_PER_APP = Settings.getUInt("ATIME", 7000);
    TIME_FORMAT = Settings.getString("TFORMAT", "%H %M");
    DATE_FORMAT = Settings.getString("DFORMAT", "%d.%m.%y");
    START_ON_MONDAY = Settings.getBool("SOM", true);
    BLOCK_NAVIGATION = Settings.getBool("BLOCKN", false);
    IS_CELSIUS = Settings.getBool("CEL", true);
    SHOW_TIME = Settings.getBool("TIM", true);
    SHOW_DATE = Settings.getBool("DAT", false);
    SHOW_TEMP = Settings.getBool("TEMP", true);
    SHOW_HUM = Settings.getBool("HUM", true);
    MATRIX_LAYOUT = Settings.getUInt("MAT", 0);
    SCROLL_SPEED = Settings.getUInt("SSPEED", 100);
    SHOW_BAT = Settings.getBool("BAT", true);
    SOUND_ACTIVE = Settings.getBool("SOUND", true);
    SOUND_VOLUME = Settings.getUInt("VOL", 25);
    uniqueID = getID();
    NTP_SERVER = Settings.getString("NTP_SERVER", NTP_SERVER);
    NTP_TZ = Settings.getString("NTP_TZ", NTP_TZ);
    MQTT_HOST = Settings.getString("MQTT_HOST", MQTT_HOST);
    MQTT_PORT = Settings.getUInt("MQTT_PORT", MQTT_PORT);
    MQTT_USER = Settings.getString("MQTT_USER", MQTT_USER);
    MQTT_PASS = Settings.getString("MQTT_PASS", MQTT_PASS);
    MQTT_PREFIX = Settings.getString("MQTT_PREFIX", String(uniqueID));
    NET_STATIC = Settings.getBool("NET_STATIC", NET_STATIC);
    HA_DISCOVERY = Settings.getBool("HA_DISCOVERY", HA_DISCOVERY);
    NET_IP = Settings.getString("NET_IP", NET_IP);
    NET_GW = Settings.getString("NET_GW", NET_GW);
    NET_SN = Settings.getString("NET_SN", NET_SN);
    NET_PDNS = Settings.getString("NET_PDNS", NET_PDNS);
    NET_SDNS = Settings.getString("NET_SDNS", NET_SDNS);
    AUTH_USER = Settings.getString("AUTH_USER", AUTH_USER);
    AUTH_PASS = Settings.getString("AUTH_PASS", AUTH_PASS);
    BOOT_SOUND = Settings.getString("BOOT_SOUND", BOOT_SOUND);
    SENSOR_READING = Settings.getBool("SENSOR_READING", SENSOR_READING);
    DFPLAYER_ACTIVE = Settings.getBool("DFPLAYER_ACTIVE", DFPLAYER_ACTIVE);
    MATRIX_LAYOUT = Settings.getUInt("MATRIX_LAYOUT", MATRIX_LAYOUT);
    MIRROR_SCREEN = Settings.getBool("MIRROR_SCREEN", MIRROR_SCREEN);
    TEMP_OFFSET = Settings.getFloat("TEMP_OFFSET", TEMP_OFFSET);
    MIN_BATTERY = Settings.getFloat("MIN_BATTERY", MIN_BATTERY);
    MAX_BATTERY = Settings.getFloat("MAX_BATTERY", MAX_BATTERY);
    AP_TIMEOUT = Settings.getUInt("AP_TIMEOUT", AP_TIMEOUT);
    BACKGROUND_EFFECT = Settings.getUInt("B_EFFECT", BACKGROUND_EFFECT);
    MIN_BRIGHTNESS = Settings.getUInt("MIN_BRI", MIN_BRIGHTNESS);
    MAX_BRIGHTNESS = Settings.getUInt("MAX_BRI", MAX_BRIGHTNESS);
    LDR_FACTOR = Settings.getFloat("LDR_FACTOR", LDR_FACTOR);
    LDR_GAMMA = Settings.getFloat("LDR_GAMMA", LDR_GAMMA);
    HUM_OFFSET = Settings.getUInt("HUM_OFFSET", HUM_OFFSET);
    HA_PREFIX = Settings.getString("HA_PREFIX", HA_PREFIX);
    STATS_INTERVAL = Settings.getLong("STATS_INTERVAL", STATS_INTERVAL);
    HOSTNAME = Settings.getString("HOSTNAME", String(uniqueID));
    BUZ_VOL = Settings.getBool("BUZ_VOL", BUZ_VOL);
    WEB_PORT = Settings.getUInt("WEB_PORT", WEB_PORT);
    TEMP_DECIMAL_PLACES = Settings.getInt("TEMP_DECIMAL", TEMP_DECIMAL_PLACES);
    ROTATE_SCREEN = Settings.getBool("ROTATE_SCREEN", ROTATE_SCREEN);
    DEBUG_MODE = Settings.getBool("DEBUG_MODE", DEBUG_MODE);
    NEWYEAR = Settings.getBool("NEWYEAR", NEWYEAR);
    SWAP_BUTTONS = Settings.getBool("SWAP_BUTTONS", SWAP_BUTTONS);
    LDR_ON_GROUND = Settings.getBool("LDR_ON_GROUND", LDR_ON_GROUND);
    BUTTON_CALLBACK = Settings.getString("BUTTON_CALLBACK", BUTTON_CALLBACK);
    COLOR_CORRECTION = CRGB(Settings.getUInt("C_CORRECTION", COLOR_CORRECTION.as_uint32_t()));
    Serial.println(Settings.getUInt("C_CORRECTION", 0));
    Serial.println(COLOR_CORRECTION.as_uint32_t());
    COLOR_TEMPERATURE = CRGB(Settings.getUInt("C_TEMPERATURE", COLOR_TEMPERATURE.as_uint32_t()));
    MQTT_ACTIVE = Settings.getBool("MQTT_ACTIVE", MQTT_ACTIVE);
    Settings.end();
}

void saveSettings()
{
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Saving usersettings"));
    Settings.begin("awtrix", false);
    Settings.putUInt("CHCOL", CALENDAR_HEADER_COLOR);
    Settings.putUInt("CTCOL", CALENDAR_TEXT_COLOR);
    Settings.putUInt("CBCOL", CALENDAR_BODY_COLOR);
    Settings.putUInt("TEFF", TRANS_EFFECT);
    Settings.putUInt("BRI", BRIGHTNESS);
    Settings.putBool("WD", SHOW_WEEKDAY);
    Settings.putBool("ABRI", AUTO_BRIGHTNESS);
    Settings.putBool("BLOCKN", BLOCK_NAVIGATION);
    Settings.putBool("ATRANS", AUTO_TRANSITION);
    Settings.putBool("UPPER", UPPERCASE_LETTERS);
    Settings.putUInt("TCOL", TEXTCOLOR_888);
    Settings.putUInt("TMODE", TIME_MODE);
    Settings.putUInt("TIME_COL", TIME_COLOR);
    Settings.putUInt("DATE_COL", DATE_COLOR);
    Settings.putUInt("TEMP_COL", TEMP_COLOR);
    Settings.putUInt("HUM_COL", HUM_COLOR);
    Settings.putUInt("BAT_COL", BAT_COLOR);
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
    Settings.putUInt("SSPEED", SCROLL_SPEED);
    Settings.putBool("BAT", SHOW_BAT);
    Settings.putBool("SOUND", SOUND_ACTIVE);
    Settings.putUInt("VOL", SOUND_VOLUME);
    Settings.putString("NTP_SERVER", NTP_SERVER);
    Settings.putString("NTP_TZ", NTP_TZ);
    Settings.putString("MQTT_HOST", MQTT_HOST);
    Settings.putUInt("MQTT_PORT", MQTT_PORT);
    Settings.putString("MQTT_USER", MQTT_USER);
    Settings.putString("MQTT_PASS", MQTT_PASS);
    Settings.putString("MQTT_PREFIX", MQTT_PREFIX);
    Settings.putBool("NET_STATIC", NET_STATIC);
    Settings.putBool("HA_DISCOVERY", HA_DISCOVERY);
    Settings.putString("NET_IP", NET_IP);
    Settings.putString("NET_GW", NET_GW);
    Settings.putString("NET_SN", NET_SN);
    Settings.putString("NET_PDNS", NET_PDNS);
    Settings.putString("NET_SDNS", NET_SDNS);
    Settings.putString("AUTH_USER", AUTH_USER);
    Settings.putString("AUTH_PASS", AUTH_PASS);
    Settings.putString("BOOT_SOUND", BOOT_SOUND);
    Settings.putBool("SENSOR_READING", SENSOR_READING);
    Settings.putBool("DFPLAYER_ACTIVE", DFPLAYER_ACTIVE);
    Settings.putUInt("MATRIX_LAYOUT", MATRIX_LAYOUT);
    Settings.putBool("MIRROR_SCREEN", MIRROR_SCREEN);
    Settings.putFloat("TEMP_OFFSET", TEMP_OFFSET);
    Settings.putFloat("MIN_BATTERY", MIN_BATTERY);
    Settings.putFloat("MAX_BATTERY", MAX_BATTERY);
    Settings.putUInt("AP_TIMEOUT", AP_TIMEOUT);
    Settings.putUInt("B_EFFECT", BACKGROUND_EFFECT);
    Settings.putUInt("MIN_BRI", MIN_BRIGHTNESS);
    Settings.putUInt("MAX_BRI", MAX_BRIGHTNESS);
    Settings.putFloat("LDR_FACTOR", LDR_FACTOR);
    Settings.putFloat("LDR_GAMMA", LDR_GAMMA);
    Settings.putInt("HUM_OFFSET", HUM_OFFSET);
    Settings.putString("HA_PREFIX", HA_PREFIX);
    Settings.putLong("STATS_INTERVAL", STATS_INTERVAL);
    Settings.putString("HOSTNAME", HOSTNAME);
    Settings.putBool("BUZ_VOL", BUZ_VOL);
    Settings.putUInt("WEB_PORT", WEB_PORT);
    Settings.putInt("TEMP_DECIMAL", TEMP_DECIMAL_PLACES);
    Settings.putBool("ROTATE_SCREEN", ROTATE_SCREEN);
    Settings.putBool("DEBUG_MODE", DEBUG_MODE);
    Settings.putBool("NEWYEAR", NEWYEAR);
    Settings.putBool("SWAP_BUTTONS", SWAP_BUTTONS);
    Settings.putBool("LDR_ON_GROUND", LDR_ON_GROUND);
    Settings.putString("BUTTON_CALLBACK", BUTTON_CALLBACK);
    Settings.putUInt("C_CORRECTION", COLOR_CORRECTION.as_uint32_t());
    Settings.putUInt("C_TEMPERATURE", COLOR_TEMPERATURE.as_uint32_t());
    Settings.putBool("MQTT_ACTIVE", MQTT_ACTIVE);
    Settings.end();
    DisplayManager.applyAllSettings();
}

const char *uniqueID;
IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;
IPAddress primaryDNS;
IPAddress secondaryDNS;
const char *VERSION = "0.98";
String MQTT_HOST = "";
uint16_t MQTT_PORT = 1883;
String MQTT_USER;
String MQTT_PASS;
String MQTT_PREFIX;
bool MQTT_ACTIVE = true;
bool IO_BROKER = false;
bool NET_STATIC = false;
bool SHOW_TIME = true;
bool SHOW_DATE = true;
bool SHOW_WEATHER = true;
bool SHOW_BAT = true;
bool SHOW_TEMP = true;
bool SHOW_HUM = true;
bool SHOW_SECONDS = true;
bool SHOW_WEEKDAY = true;
String NET_IP = "192.168.178.100";
String NET_GW = "192.168.178.1";
String NET_SN = "255.255.255.0";
String NET_PDNS = "8.8.8.8";
String NET_SDNS = "1.1.1.1";
long TIME_PER_APP = 7000;
uint8_t MATRIX_FPS = 42;
int TIME_PER_TRANSITION = 400;
String NTP_SERVER = "de.pool.ntp.org";
String NTP_TZ = "CET-1CEST,M3.5.0,M10.5.0/3";
bool HA_DISCOVERY = false;
String HA_PREFIX = "homeassistant";
// Periphery
String CURRENT_APP;
float CURRENT_TEMP;
bool IS_CELSIUS;

uint8_t TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_NONE;

float CURRENT_HUM;
float CURRENT_LUX;
int BRIGHTNESS = 120;
int BRIGHTNESS_PERCENT;

uint16_t MIN_BATTERY = 475;
uint16_t MAX_BATTERY = 665;

#ifdef awtrix2_upgrade
float TEMP_OFFSET;
#else
float TEMP_OFFSET = -9;
uint8_t BATTERY_PERCENT = 0;
uint16_t BATTERY_RAW = 0;
#endif
float HUM_OFFSET;
uint16_t LDR_RAW;
String TIME_FORMAT = "%H:%M:%S";
String DATE_FORMAT = "%d.%m.%y";
int BACKGROUND_EFFECT = -1;
bool START_ON_MONDAY;

// Matrix States
bool AUTO_TRANSITION = false;
bool AUTO_BRIGHTNESS = true;
bool UPPERCASE_LETTERS = true;
bool AP_MODE;
bool MATRIX_OFF;
bool MIRROR_SCREEN = false;
uint32_t TEXTCOLOR_888 = 0xFFFFFF;
bool SOUND_ACTIVE;
String BOOT_SOUND = "";
int TEMP_DECIMAL_PLACES = 0;
uint8_t SOUND_VOLUME = 30;
int MATRIX_LAYOUT = 0;
bool UPDATE_AVAILABLE = false;
long RECEIVED_MESSAGES;
CRGB COLOR_CORRECTION = CRGB(125, 125, 125);
CRGB COLOR_TEMPERATURE = CRGB(255, 255, 255);
uint32_t WDC_ACTIVE;
uint32_t WDC_INACTIVE;
bool BLOCK_NAVIGATION = false;

float GAMMA = 0;
bool SENSOR_READING = true;
bool SENSORS_STABLE = false;
bool DFPLAYER_ACTIVE = false;
bool ROTATE_SCREEN = false;
uint8_t TIME_MODE = 1;
uint8_t SCROLL_SPEED = 100;
uint32_t TIME_COLOR = 0;
uint32_t CALENDAR_HEADER_COLOR = 0xFF0000;
uint32_t CALENDAR_TEXT_COLOR = 0x000000;
uint32_t CALENDAR_BODY_COLOR = 0xFFFFFF;
uint32_t DATE_COLOR = 0;
uint32_t BAT_COLOR = 0;
uint32_t TEMP_COLOR = 0;
uint32_t HUM_COLOR = 0;
bool ARTNET_MODE;
bool MOODLIGHT_MODE;
long STATS_INTERVAL = 10000;
bool DEBUG_MODE = true;
uint8_t MIN_BRIGHTNESS = 2;
uint8_t MAX_BRIGHTNESS = 160;
double movementFactor = 0.5;
int8_t TRANS_EFFECT = 1;
String AUTH_USER = "";
String AUTH_PASS = "awtrix";
String BUTTON_CALLBACK = "";
bool NEWYEAR = false;
bool SWAP_BUTTONS = false;
bool LDR_ON_GROUND = false;
float LDR_GAMMA = 3.0;
float LDR_FACTOR = 1.0;
bool GAME_ACTIVE = false;
uint32_t AP_TIMEOUT = 15;
int WEB_PORT = 80;
OverlayEffect GLOBAL_OVERLAY = NONE;
String HOSTNAME = "";
bool BUZ_VOL = false;