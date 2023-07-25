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
#include "UpdateManager.h"
#include "PeripheryManager.h"

WebServer server(80);
FSWebServer mws(LittleFS, server);

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
    webRequest->send(200, F("text/plain"), VERSION);
}

void handleBmpRequest()
{
    // Setze den Inhaltstyp auf BMP
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send_P(200, "image/bmp", "");
    int *ledColors = DisplayManager.getLedColors();

    // Schreibe BMP-Header
    uint16_t fileHeader[7] = {0x4D42, 0x1136, 0x0000, 0x0000, 0x0036, 0x0000, 0x0028};
    server.sendContent_P((char *)&fileHeader, sizeof(fileHeader));
    uint32_t header[9] = {32, 8, 0x00200001, 0x0000, 0x0000, 0x1136, 0x0B13, 0x0B13, 0x0000};
    server.sendContent_P((char *)&header, sizeof(header));

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            int color = ledColors[y * 32 + x];
            char pixel[3] = {static_cast<char>(color & 0xFF), static_cast<char>((color >> 8) & 0xFF), static_cast<char>((color >> 16) & 0xFF)};
            server.sendContent_P(pixel, sizeof(pixel));
        }
    }
    delete[] ledColors; // Lösche das Array

    server.sendContent("");
}

void saveHandler()
{
    WebServerClass *webRequest = mws.getRequest();
    ServerManager.getInstance().loadSettings();
    webRequest->send(200);
}

void addHandler()
{
    mws.addHandler("/api/power", HTTP_POST, []()
                   { DisplayManager.powerStateParse(mws.webserver->arg("plain").c_str()); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/api/reboot", HTTP_POST, []()
                   { mws.webserver->send(200,F("text/plain"),F("OK")); delay(200); ESP.restart(); });
    mws.addHandler("/api/sound", HTTP_POST, []()
                   { if (PeripheryManager.parseSound(mws.webserver->arg("plain").c_str())){
                    mws.webserver->send(200,F("text/plain"),F("OK")); 
                   }else{
                    mws.webserver->send(404,F("text/plain"),F("FileNotFound"));  
                   }; });
    mws.addHandler("/api/moodlight", HTTP_POST, []()
                   {
                    if (DisplayManager.moodlight(mws.webserver->arg("plain").c_str()))
                    {
                        mws.webserver->send(200, F(F("text/plain")), F("OK"));
                    }
                    else
                    {
                        mws.webserver->send(500, F("text/plain"), F("ErrorParsingJson"));
                    } });
    mws.addHandler("/api/notify", HTTP_POST, []()
                   {
                       if (DisplayManager.generateNotification(1,mws.webserver->arg("plain").c_str()))
                       {
                        mws.webserver->send(200, F("text/plain"), F("OK"));
                       }else{
                        mws.webserver->send(500, F("text/plain"), F("ErrorParsingJson"));
                       } });
    mws.addHandler("/api/nextapp", HTTP_POST, []()
                   { DisplayManager.nextApp(); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/screenshot.bmp", HTTP_GET, [&]()
                   {

    WiFiClient client = mws.webserver->client();
    mws.webserver->sendHeader("Content-Type", "image/bmp");
    DisplayManager.sendBMP(client); });
    mws.addHandler("/api/previousapp", HTTP_POST, []()
                   { DisplayManager.previousApp(); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/api/timer", HTTP_POST, []()
                   { DisplayManager.gererateTimer(mws.webserver->arg("plain").c_str()); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/api/notify/dismiss", HTTP_POST, []()
                   { DisplayManager.dismissNotify(); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/api/apps", HTTP_POST, []()
                   { DisplayManager.updateAppVector(mws.webserver->arg("plain").c_str()); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler(
        "/api/switch", HTTP_POST, []()
        {
        if (DisplayManager.switchToApp(mws.webserver->arg("plain").c_str()))
        {
            mws.webserver->send(200, F("text/plain"), F("OK"));
        }
        else
        {
            mws.webserver->send(500, F("text/plain"), F("FAILED"));
        } });
    mws.addHandler("/api/apps", HTTP_GET, []()
                   { mws.webserver->send_P(200, "application/json", DisplayManager.getAppsWithIcon().c_str()); });
    mws.addHandler("/api/settings", HTTP_POST, []()
                   { DisplayManager.setNewSettings(mws.webserver->arg("plain").c_str()); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/api/reorder", HTTP_POST, []()
                   { DisplayManager.reorderApps(mws.webserver->arg("plain").c_str()); mws.webserver->send(200,F("text/plain"),F("OK")); });
    mws.addHandler("/api/settings", HTTP_GET, []()
                   { mws.webserver->send_P(200, "application/json", DisplayManager.getSettings().c_str()); });
    mws.addHandler("/api/custom", HTTP_POST, []()
                   { 
                    if (DisplayManager.parseCustomPage(mws.webserver->arg("name"),mws.webserver->arg("plain").c_str())){
                        mws.webserver->send(200,F("text/plain"),F("OK")); 
                    }else{
                        mws.webserver->send(500,F("text/plain"),F("ErrorParsingJson")); 
                    } });
    mws.addHandler("/api/stats", HTTP_GET, []()
                   { mws.webserver->send_P(200, "application/json", DisplayManager.getStats().c_str()); });
    mws.addHandler("/api/screen", HTTP_GET, []()
                   { mws.webserver->send_P(200, "application/json", DisplayManager.ledsAsJson().c_str()); });
    mws.addHandler("/api/indicator1", HTTP_POST, []()
                   { 
                    if (DisplayManager.indicatorParser(1,mws.webserver->arg("plain").c_str())){
                     mws.webserver->send(200,F("text/plain"),F("OK")); 
                    }else{
                         mws.webserver->send(500,F("text/plain"),F("ErrorParsingJson")); 
                    } });
    mws.addHandler("/api/indicator2", HTTP_POST, []()
                   { 
                    if (DisplayManager.indicatorParser(2,mws.webserver->arg("plain").c_str())){
                     mws.webserver->send(200,F("text/plain"),F("OK")); 
                    }else{
                         mws.webserver->send(500,F("text/plain"),F("ErrorParsingJson")); 
                    } });
    mws.addHandler("/api/indicator3", HTTP_POST, []()
                   { 
                    if (DisplayManager.indicatorParser(3,mws.webserver->arg("plain").c_str())){
                     mws.webserver->send(200,F("text/plain"),F("OK")); 
                    }else{
                         mws.webserver->send(500,F("text/plain"),F("ErrorParsingJson")); 
                    } });
    mws.addHandler("/api/doupdate", HTTP_POST, []()
                   { 
                    if (UpdateManager.checkUpdate(true)){
                        mws.webserver->send(200,F("text/plain"),F("OK"));
                        UpdateManager.updateFirmware();
                    }else{
                        mws.webserver->send(404,F("text/plain"),"NoUpdateFound");    
                    } });
}

void ServerManager_::setup()
{
    if (!local_IP.fromString(NET_IP) || !gateway.fromString(NET_GW) || !subnet.fromString(NET_SN) || !primaryDNS.fromString(NET_PDNS) || !secondaryDNS.fromString(NET_SDNS))
        NET_STATIC = false;
    if (NET_STATIC)
    {
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    }
    WiFi.setHostname(uniqueID); // define hostname
    myIP = mws.startWiFi(15000, uniqueID, "12345678");
    isConnected = !(myIP == IPAddress(192, 168, 4, 1));
    DEBUG_PRINTF("My IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
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
        mws.addHandler("/save", HTTP_POST, saveHandler);
        addHandler();

        DEBUG_PRINTLN(F("Webserver loaded"));
    }
    mws.addHandler("/version", HTTP_GET, versionHandler);
    mws.begin();

    if (!MDNS.begin(uniqueID))
    {
        DEBUG_PRINTLN(F("Error starting mDNS"));
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
    int comma1 = str.indexOf(',');
    int comma2 = str.lastIndexOf(',');
    if (comma1 < 0 || comma2 < 0 || comma2 == comma1)
    {
        return 0xFFFF;
    }
    String rStr = str.substring(0, comma1);
    String gStr = str.substring(comma1 + 1, comma2);
    String bStr = str.substring(comma2 + 1);

    int r = rStr.toInt();
    int g = gStr.toInt();
    int b = bStr.toInt();

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
    {
        return 0xFFFF;
    }

    uint16_t color = ((r >> 3) << 11) | ((g >> 2) & 0x3F) << 5 | (b >> 3);
    return color;
}

String colorToString(uint16_t color)
{
    uint8_t r = (color >> 11) << 3;
    uint8_t g = ((color >> 5) & 0x3F) << 2;
    uint8_t b = (color & 0x1F) << 3;
    if (r > 255 || g > 255 || b > 255)
    {
        return "#FFFFFF";
    }
    String rStr = String(r);
    String gStr = String(g);
    String bStr = String(b);
    return rStr + "," + gStr + "," + bStr;
}

void ServerManager_::loadSettings()
{
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
        file.close();
        DisplayManager.applyAllSettings();
        DEBUG_PRINTLN(F("Webserver configuration loaded"));
        return;
    }
    else
        DEBUG_PRINTLN(F("Webserver configuration file not exist"));
    return;
}
