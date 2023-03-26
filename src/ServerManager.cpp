#include "ServerManager.h"
#include "Globals.h"
#include <WebServer.h>
#include <esp-fs-webserver.h>
#include "icondownloader.h"
#include <Update.h>
#include <ESPmDNS.h>
#include "SPI.h"
#include <LittleFS.h>
#include <WiFi.h>
#include "DisplayManager.h"

WebServer server(80);
FSWebServer mws(LittleFS, server);
bool FSOPEN;
void startLittleFS()
{

    if (LittleFS.begin())
    {
        LittleFS.mkdir("/MELODIES");
        LittleFS.mkdir("/ICONS");
        FSOPEN = true;
    }
    else
    {
        Serial.println("ERROR on mounting LittleFS. It will be formmatted!");
        LittleFS.format();
        ESP.restart();
    }
}

// The getter for the instantiated singleton instance
ServerManager_ &ServerManager_::getInstance()
{
    static ServerManager_ instance;
    return instance;
}

// Initialize the global shared instance
ServerManager_ &ServerManager = ServerManager.getInstance();

void versionHandler()
{
    WebServerClass *webRequest = mws.getRequest();
    webRequest->send(200, "text/plain", VERSION);
}

void saveHandler()
{
    WebServerClass *webRequest = mws.getRequest();
    Serial.println("Save");
    ServerManager.getInstance().loadSettings();
    webRequest->send(200);
}

void ServerManager_::setup()
{

    if (!local_IP.fromString(NET_IP) || !gateway.fromString(NET_GW) || !subnet.fromString(NET_SN) || !primaryDNS.fromString(NET_PDNS) || !secondaryDNS.fromString(NET_SDNS))
        NET_STATIC = false;
    if (NET_STATIC)
    {
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    }
    IPAddress myIP = mws.startWiFi(150000, "AWTRIX LIGHT", "12345678");
    isConnected = !(myIP == IPAddress(192, 168, 4, 1));
    Serial.println(myIP.toString());

    if (isConnected)
    {
        mws.addOptionBox("Network");
        mws.addOption("Static IP", NET_STATIC);
        mws.addOption("Local IP", NET_IP);
        mws.addOption("Gateway", NET_GW);
        mws.addOption("Subnet", NET_SN);
        mws.addOption("Primary DNS", NET_PDNS);
        mws.addOption("Secondary DNS", NET_SDNS);
        mws.addOptionBox("MQTT");
        mws.addOption("Broker", MQTT_HOST);
        mws.addOption("Port", MQTT_PORT);
        mws.addOption("Username", MQTT_USER);
        mws.addOption("Password", MQTT_PASS);
        mws.addOption("Prefix", MQTT_PREFIX);
        mws.addOption("Homeassistant Discovery", HA_DISCOVERY);
        mws.addOptionBox("Time");
        mws.addOption("NTP Server", NTP_SERVER);
        mws.addOption("Timezone", NTP_TZ);
        mws.addHTML("<p>Find your timezone at <a href='https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv' target='_blank' rel='noopener noreferrer'>posix_tz_db</a>.</p>", "tz_link");
        mws.addOptionBox("Icons");
        mws.addHTML(custom_html, "icon_html");
        mws.addCSS(custom_css);
        mws.addJavascript(custom_script);
        mws.addOptionBox("General");
        mws.addOption("Uppercase letters", UPPERCASE_LETTERS);
        mws.addHandler("/save", HTTP_GET, saveHandler);
    }

    mws.addHandler("/version", HTTP_GET, versionHandler);
    mws.begin();

    if (!MDNS.begin(MQTT_PREFIX.c_str()))
    {
        Serial.println("Error starting mDNS");
        return;
    }

    configTzTime(NTP_TZ.c_str(), NTP_SERVER.c_str());
    tm timeInfo;
    getLocalTime(&timeInfo);
}

void ServerManager_::tick()
{
    mws.run();
}

uint16_t stringToColor(const String &str)
{
    // Aufteilen des Strings in seine Bestandteile
    int comma1 = str.indexOf(',');
    int comma2 = str.lastIndexOf(',');
    if (comma1 < 0 || comma2 < 0 || comma2 == comma1)
    {
        // Ungültiges Format
        return 0xFFFF;
    }
    String rStr = str.substring(0, comma1);
    String gStr = str.substring(comma1 + 1, comma2);
    String bStr = str.substring(comma2 + 1);

    // Konvertieren der Werte von Strings zu Zahlen
    int r = rStr.toInt();
    int g = gStr.toInt();
    int b = bStr.toInt();

    // Sicherheitsabfrage: Werte müssen zwischen 0 und 255 liegen
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
    {
        return 0xFFFF;
    }

    // Konvertieren der Werte zu 5-6-5 Bitformat
    uint16_t color = ((r >> 3) << 11) | ((g >> 2) & 0x3F) << 5 | (b >> 3);
    return color;
}

String colorToString(uint16_t color)
{
    // Konvertieren der Farbwerte von 5-6-5 Bitformat zu 8 Bit
    uint8_t r = (color >> 11) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;

    // Sicherheitsabfrage: Werte müssen zwischen 0 und 255 liegen
    if (r > 255 || g > 255 || b > 255)
    {
        return "#FFFFFF";
    }

    // Konvertieren der Farbwerte zu Strings und Zusammenführen
    String rStr = String(r);
    String gStr = String(g);
    String bStr = String(b);
    return rStr + "," + gStr + "," + bStr;
}

void ServerManager_::loadSettings()
{
    if (!FSOPEN)
        startLittleFS();

    if (LittleFS.exists("/config.json"))
    {
        File file = LittleFS.open("/config.json", "r");
        DynamicJsonDocument doc(file.size() * 1.33);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
            return;

        NTP_SERVER = doc["NTP Server"].as<String>();
        NTP_TZ = doc["Timezone"].as<String>();
        MQTT_HOST = doc["Broker"].as<String>();
        MQTT_PORT = doc["Port"].as<uint16_t>();
        MQTT_USER = doc["Username"].as<String>();
        MQTT_PASS = doc["Password"].as<String>();
        MQTT_PREFIX = doc["Prefix"].as<String>();
        NET_STATIC = doc["Static IP"];
        HA_DISCOVERY = doc["Homeassistant Discovery"];
        NET_IP = doc["Local IP"].as<String>();
        NET_GW = doc["Gateway"].as<String>();
        NET_SN = doc["Subnet"].as<String>();
        NET_PDNS = doc["Primary DNS"].as<String>();
        NET_SDNS = doc["Secondary DNS"].as<String>();
        UPPERCASE_LETTERS = doc["Uppercase letters"];
        file.close();
        DisplayManager.applyAllSettings();
        Serial.println(F("Configuration loaded"));
        return;
    }
    else
        Serial.println(F("Configuration file not exist"));
    return;
}

