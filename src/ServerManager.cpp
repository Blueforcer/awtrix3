#include "ServerManager.h"
#include "Globals.h"
#include <ESPAsyncWebServer.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include "DisplayManager.h"
#include "UpdateManager.h"
#include "PeripheryManager.h"
#include "PowerManager.h"
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include "Games/GameManager.h"
#include <EEPROM.h>
#include "htmls.h"

WiFiUDP udp;

unsigned int localUdpPort = 4210;
char incomingPacket[255];

// Pufferdefinition
#define BUFFER_SIZE 64
char dataBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// Aktueller verbundener Client
WiFiClient currentClient = WiFiClient();
AsyncWebServer server(80);

// Erstelle eine Server-Instanz
WiFiServer TCPserver(8080);

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
    // WebServerClass *webRequest = mws.getRequest();
    // webRequest->send(200, F("text/plain"), VERSION);
}

void ServerManager_::erase()
{
    DisplayManager.HSVtext(0, 6, "RESET", true, 0);
    wifi_config_t conf;
    memset(&conf, 0, sizeof(conf)); // Set all the bytes in the structure to 0
                                    // esp_wifi_set_config(WIFI_IF_STA, &conf);
    LittleFS.format();
    delay(200);
    formatSettings();
    delay(200);
}

void saveHandler()
{
    // WebServerClass *webRequest = mws.getRequest();
    // ServerManager.getInstance().loadSettings();
    // webRequest->send(200);
}

void addHandler()
{
      server.on("/",  HTTP_GET, [](AsyncWebServerRequest *request)
      { 
        request->send(200, "text/html", html);
      });
    
    server.on("/api/power", HTTP_POST, [](AsyncWebServerRequest *request)
              { DisplayManager.powerStateParse(request->arg("plain").c_str()); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/sleep", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  request->send(200, F("text/plain"), F("OK"));
                  DisplayManager.setPower(false);
                  PowerManager.sleepParser(request->arg("plain").c_str()); });

    server.on("/api/loop", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getAppsAsJson().c_str()); });

    server.on("/api/effects", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getEffectNames().c_str()); });

    server.on("/api/transitions", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getTransitionNames().c_str()); });

    server.on("/api/reboot", HTTP_ANY, [](AsyncWebServerRequest *request)
              { request->send(200, F("text/plain"), F("OK")); delay(200); ESP.restart(); });

    server.on("/api/rtttl", HTTP_POST, [](AsyncWebServerRequest *request)
              { request->send(200, F("text/plain"), F("OK")); PeripheryManager.playRTTTLString(request->arg("plain").c_str()); });

    server.on("/api/sound", HTTP_POST, [](AsyncWebServerRequest *request)
              { if (PeripheryManager.parseSound(request->arg("plain").c_str())){
                    request->send(200, F("text/plain"), F("OK")); 
                   }else{
                    request->send(404, F("text/plain"), F("FileNotFound"));  
                   }; });

    server.on("/api/moodlight", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                    if (DisplayManager.moodlight(request->arg("plain").c_str()))
                    {
                        request->send(200, F("text/plain"), F("OK"));
                    }
                    else
                    {
                        request->send(500, F("text/plain"), F("ErrorParsingJson"));
                    } });

    server.on("/api/notify", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                       if (DisplayManager.generateNotification(1, request->arg("plain").c_str()))
                       {
                        request->send(200, F("text/plain"), F("OK"));
                       }else{
                        request->send(500, F("text/plain"), F("ErrorParsingJson"));
                       } });

    server.on("/api/nextapp", HTTP_ANY, [](AsyncWebServerRequest *request)
              { DisplayManager.nextApp(); request->send(200, F("text/plain"), F("OK")); });

    server.on("/fullscreen", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String fps = request->arg("fps");
    if (fps == "") {
        fps = "30"; 
    }
    String finalHTML = screenfull_html; 
    finalHTML.replace("%%FPS%%", fps);

    request->send(200, "text/html", finalHTML.c_str()); });

    server.on("/screen", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", screen_html); });

    server.on("/backup", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", backup_html); });

    server.on("/api/previousapp", HTTP_POST, [](AsyncWebServerRequest *request)
              { DisplayManager.previousApp(); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/notify/dismiss", HTTP_ANY, [](AsyncWebServerRequest *request)
              { DisplayManager.dismissNotify(); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/apps", HTTP_POST, [](AsyncWebServerRequest *request)
              { DisplayManager.updateAppVector(request->arg("plain").c_str()); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/switch", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (DisplayManager.switchToApp(request->arg("plain").c_str()))
        {
            request->send(200, F("text/plain"), F("OK"));
        }
        else
        {
            request->send(500, F("text/plain"), F("FAILED"));
        } });

    server.on("/api/apps", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getAppsWithIcon().c_str()); });

    server.on("/api/settings", HTTP_POST, [](AsyncWebServerRequest *request)
              { DisplayManager.setNewSettings(request->arg("plain").c_str()); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/erase", HTTP_ANY, [](AsyncWebServerRequest *request)
              { ServerManager.erase();  request->send(200, F("text/plain"), F("OK")); delay(200); ESP.restart(); });

    server.on("/api/resetSettings", HTTP_ANY, [](AsyncWebServerRequest *request)
              { formatSettings();   request->send(200, F("text/plain"), F("OK")); delay(200); ESP.restart(); });

    server.on("/api/reorder", HTTP_POST, [](AsyncWebServerRequest *request)
              { DisplayManager.reorderApps(request->arg("plain").c_str()); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getSettings().c_str()); });

    server.on("/api/custom", HTTP_POST, [](AsyncWebServerRequest *request)
              { 
                    if (DisplayManager.parseCustomPage(request->arg("name"), request->arg("plain").c_str(), false)){
                        request->send(200, F("text/plain"), F("OK")); 
                    }else{
                        request->send(500, F("text/plain"), F("ErrorParsingJson")); 
                    } });

    server.on("/api/stats", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getStats().c_str()); });

    server.on("/api/screen", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.ledsAsJson().c_str()); });

    server.on("/api/indicator1", HTTP_POST, [](AsyncWebServerRequest *request)
              { 
                    if (DisplayManager.indicatorParser(1, request->arg("plain").c_str())){
                     request->send(200, F("text/plain"), F("OK")); 
                    }else{
                         request->send(500, F("text/plain"), F("ErrorParsingJson")); 
                    } });

    server.on("/api/indicator2", HTTP_POST, [](AsyncWebServerRequest *request)
              { 
                    if (DisplayManager.indicatorParser(2, request->arg("plain").c_str())){
                     request->send(200, F("text/plain"), F("OK")); 
                    }else{
                         request->send(500, F("text/plain"), F("ErrorParsingJson")); 
                    } });

    server.on("/api/indicator3", HTTP_POST, [](AsyncWebServerRequest *request)
              { 
                    if (DisplayManager.indicatorParser(3, request->arg("plain").c_str())){
                     request->send(200, F("text/plain"), F("OK")); 
                    }else{
                         request->send(500, F("text/plain"), F("ErrorParsingJson")); 
                    } });

    server.on("/api/doupdate", HTTP_POST, [](AsyncWebServerRequest *request)
              { 
                    if (UpdateManager.checkUpdate(true)){
                        request->send(200, F("text/plain"), F("OK"));
                        UpdateManager.updateFirmware();
                    }else{
                        request->send(404, F("text/plain"), "NoUpdateFound");    
                    } });

    server.on("/api/r2d2", HTTP_POST, [](AsyncWebServerRequest *request)
              { PeripheryManager.r2d2(request->arg("plain").c_str()); request->send(200, F("text/plain"), F("OK")); });

    server.on("/version", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, F("text/plain"), VERSION); });
}

void ServerManager_::setup()
{
    if (!local_IP.fromString(NET_IP) || !gateway.fromString(NET_GW) || !subnet.fromString(NET_SN) || !primaryDNS.fromString(NET_PDNS) || !secondaryDNS.fromString(NET_SDNS))
        NET_STATIC = false;
    if (NET_STATIC)
    {
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    }
    WiFi.setHostname(HOSTNAME.c_str()); // define hostname

    WiFi.mode(WIFI_STA);
    WiFi.begin("Kindergarten", "53825382");
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.print("IP Address: ");
    myIP = (WiFi.localIP());
    AP_MODE = false;
    // myIP = mws.startWiFi(AP_TIMEOUT * 1000, HOSTNAME.c_str(), "12345678");
    isConnected = !(myIP == IPAddress(192, 168, 4, 1));
    if (DEBUG_MODE)
        DEBUG_PRINTF("My IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
    // mws.setAuth(AUTH_USER, AUTH_PASS);
    if (isConnected)
    {

        addHandler();
        udp.begin(localUdpPort);
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Webserver loaded"));
    }

    if (!MDNS.begin(HOSTNAME))
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Error starting mDNS"));
    }
    else
    {
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("awtrix", "tcp", 80);
        MDNS.addServiceTxt("awtrix", "tcp", "id", uniqueID);
        MDNS.addServiceTxt("awtrix", "tcp", "name", HOSTNAME.c_str());
        MDNS.addServiceTxt("awtrix", "tcp", "type", "awtrix3");
    }
  server.begin();
    configTzTime(NTP_TZ.c_str(), NTP_SERVER.c_str());
    tm timeInfo;
    getLocalTime(&timeInfo);
    TCPserver.begin();
    TCPserver.setNoDelay(true);
}

void ServerManager_::tick()
{

    if (!AP_MODE)
    {
        int packetSize = udp.parsePacket();
        if (packetSize)
        {
            int len = udp.read(incomingPacket, 255);
            if (len > 0)
            {
                incomingPacket[len] = 0;
            }
            if (strcmp(incomingPacket, "FIND_AWTRIX") == 0)
            {
                udp.beginPacket(udp.remoteIP(), 4211);
                if (WEB_PORT != 80)
                {
                    char buffer[128];
                    sprintf(buffer, "%s:%d", HOSTNAME.c_str(), WEB_PORT);
                    udp.printf(buffer);
                }
                else
                {
                    udp.printf(HOSTNAME.c_str());
                }

                udp.endPacket();
            }
        }
    }

    if (!currentClient || !currentClient.connected())
    {
        if (TCPserver.hasClient())
        {
            if (currentClient)
            {
                currentClient.stop();
                Serial.println("Vorheriger Client getrennt, um neuen Client zu akzeptieren.");
            }
            currentClient = TCPserver.available();
            Serial.println("Neuer Client verbunden.");
        }
    }

    if (currentClient && currentClient.connected())
    {
        while (currentClient.available())
        {
            char incomingByte = currentClient.read();
            if (incomingByte == '\n')
            {
                dataBuffer[bufferIndex] = '\0';
                GameManager.ControllerInput(dataBuffer);
                bufferIndex = 0;
            }
            else if (incomingByte != '\r')
            {
                if (bufferIndex < BUFFER_SIZE - 1)
                {
                    dataBuffer[bufferIndex++] = incomingByte;
                }
                else
                {
                    bufferIndex = 0;
                }
            }
        }
    }
}

void ServerManager_::sendTCP(String message)
{
    if (currentClient && currentClient.connected())
    {
        currentClient.print(message);
    }
}

void ServerManager_::loadSettings()
{
    if (LittleFS.exists("/DoNotTouch.json"))
    {
        File file = LittleFS.open("/DoNotTouch.json", "r");
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
        MQTT_PREFIX.trim();
        NET_STATIC = doc["Static IP"];
        HA_DISCOVERY = doc["Homeassistant Discovery"];
        NET_IP = doc["Local IP"].as<String>();
        NET_GW = doc["Gateway"].as<String>();
        NET_SN = doc["Subnet"].as<String>();
        NET_PDNS = doc["Primary DNS"].as<String>();
        NET_SDNS = doc["Secondary DNS"].as<String>();
        if (doc["Auth Username"].is<String>())
            AUTH_USER = doc["Auth Username"].as<String>();
        if (doc["Auth Password"].is<String>())
            AUTH_PASS = doc["Auth Password"].as<String>();

        file.close();
        DisplayManager.applyAllSettings();
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Webserver configuration loaded"));
        doc.clear();
        return;
    }
    else if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Webserver configuration file not exist"));
    return;
}

void ServerManager_::sendButton(byte btn, bool state)
{
    if (BUTTON_CALLBACK == "")
        return;
    static bool btn0State, btn1State, btn2State;
    String payload;
    switch (btn)
    {
    case 0:
        if (btn0State != state)
        {
            btn0State = state;
            payload = "button=left&state=" + String(state);
        }
        break;
    case 1:
        if (btn1State != state)
        {
            btn1State = state;
            payload = "button=middle&state=" + String(state);
        }
        break;
    case 2:
        if (btn2State != state)
        {
            btn2State = state;
            payload = "button=right&state=" + String(state);
        }
        break;
    default:
        return;
    }
    if (!payload.isEmpty())
    {
        HTTPClient http;
        http.begin(BUTTON_CALLBACK);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.POST(payload);
        http.end();
    }
}
