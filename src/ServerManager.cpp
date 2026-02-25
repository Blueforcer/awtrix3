#include "ServerManager.h"
#include "Globals.h"
#include <WebServer.h>
#include <esp-fs-webserver.h>
#include "htmls.h"
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

// --- File-scope state (internal linkage) ---

namespace
{
    constexpr int TCP_BUFFER_SIZE = 64;
    constexpr int UDP_BUFFER_SIZE = 256;
    constexpr unsigned int UDP_PORT = 4210;
    constexpr unsigned int UDP_RESPONSE_PORT = 4211;
    constexpr int TCP_PORT = 8080;
    constexpr size_t MAX_BODY_SIZE = 8192; // Matches MQTT_MAX_PACKET_SIZE

    WiFiUDP udp;
    char udpBuffer[UDP_BUFFER_SIZE];

    char tcpBuffer[TCP_BUFFER_SIZE];
    int tcpBufferIndex = 0;

    WiFiClient tcpClient;
    WebServer httpServer(80);
    FSWebServer mws(LittleFS, httpServer);

    WiFiServer tcpServer(TCP_PORT);

    // Button state tracking for deduplication
    bool btnStates[3] = {false, false, false};

    struct ButtonEvent
    {
        byte btn;
        bool state;
    };

    // FreeRTOS async HTTP task
    QueueHandle_t buttonQueue = nullptr;
    TaskHandle_t httpTaskHandle = nullptr;
    SemaphoreHandle_t callbackMutex = nullptr;
} // namespace

// --- Singleton ---

ServerManager_ &ServerManager_::getInstance()
{
    static ServerManager_ instance;
    return instance;
}

ServerManager_ &ServerManager = ServerManager_::getInstance();

// --- HTTP Route Handlers ---

static bool isBodyOversized()
{
    return mws.webserver->arg("plain").length() > MAX_BODY_SIZE;
}

static void sendOk()
{
    mws.webserver->send(200, F("text/plain"), F("OK"));
}

static void sendBadRequest()
{
    mws.webserver->send(400, F("text/plain"), F("BadRequest"));
}

static void sendJsonError()
{
    mws.webserver->send(500, F("text/plain"), F("ErrorParsingJson"));
}

void ServerManager_::registerHttpHandlers()
{
    mws.addHandler("/api/power", HTTP_POST, []()
                   {
        DisplayManager.powerStateParse(mws.webserver->arg("plain").c_str());
        sendOk(); });

    mws.addHandler("/api/sleep", HTTP_POST, []()
                   {
        sendOk();
        DisplayManager.setPower(false);
        PowerManager.sleepParser(mws.webserver->arg("plain").c_str()); });

    mws.addHandler("/api/loop", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.getAppsAsJson()); });

    mws.addHandler("/api/effects", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.getEffectNames()); });

    mws.addHandler("/api/transitions", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.getTransitionNames()); });

    mws.addHandler("/api/reboot", HTTP_ANY, []()
                   {
        sendOk();
        mws.webserver->client().flush();
        delay(200);
        ESP.restart(); });

    mws.addHandler("/api/rtttl", HTTP_POST, []()
                   {
        sendOk();
        PeripheryManager.playRTTTLString(mws.webserver->arg("plain").c_str()); });

    mws.addHandler("/api/sound", HTTP_POST, []()
                   {
        if (PeripheryManager.parseSound(mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            mws.webserver->send(404, F("text/plain"), F("FileNotFound")); });

    mws.addHandler("/api/moodlight", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        if (DisplayManager.moodlight(mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            sendJsonError(); });

    mws.addHandler("/api/notify", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        if (DisplayManager.generateNotification(1, mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            sendJsonError(); });

    mws.addHandler("/api/nextapp", HTTP_ANY, []()
                   { DisplayManager.nextApp(); sendOk(); });

    mws.addHandler("/fullscreen", HTTP_GET, []()
                   {
        String fps = mws.webserver->arg("fps");
        if (fps == "")
            fps = "30";
        String finalHTML = screenfull_html;
        finalHTML.replace("%%FPS%%", fps);
        mws.webserver->send(200, "text/html", finalHTML.c_str()); });

    mws.addHandler("/screen", HTTP_GET, []()
                   { mws.webserver->send(200, "text/html", screen_html); });

    mws.addHandler("/backup", HTTP_GET, []()
                   { mws.webserver->send(200, "text/html", backup_html); });

    mws.addHandler("/api/previousapp", HTTP_POST, []()
                   { DisplayManager.previousApp(); sendOk(); });

    mws.addHandler("/api/notify/dismiss", HTTP_ANY, []()
                   { DisplayManager.dismissNotify(); sendOk(); });

    mws.addHandler("/api/apps", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        DisplayManager.updateAppVector(mws.webserver->arg("plain").c_str());
        sendOk(); });

    mws.addHandler("/api/switch", HTTP_POST, []()
                   {
        if (DisplayManager.switchToApp(mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            mws.webserver->send(500, F("text/plain"), F("FAILED")); });

    mws.addHandler("/api/apps", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.getAppsWithIcon()); });

    mws.addHandler("/api/settings", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        DisplayManager.setNewSettings(mws.webserver->arg("plain").c_str());
        sendOk(); });

    mws.addHandler("/api/erase", HTTP_ANY, []()
                   {
        ServerManager.erase();
        sendOk();
        mws.webserver->client().flush();
        delay(200);
        ESP.restart(); });

    mws.addHandler("/api/resetSettings", HTTP_ANY, []()
                   {
        formatSettings();
        sendOk();
        mws.webserver->client().flush();
        delay(200);
        ESP.restart(); });

    mws.addHandler("/api/reorder", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        DisplayManager.reorderApps(mws.webserver->arg("plain").c_str());
        sendOk(); });

    mws.addHandler("/api/settings", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.getSettings()); });

    mws.addHandler("/api/custom", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        if (DisplayManager.parseCustomPage(mws.webserver->arg("name"), mws.webserver->arg("plain").c_str(), false))
            sendOk();
        else
            sendJsonError(); });

    mws.addHandler("/api/stats", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.getStats()); });

    mws.addHandler("/api/screen", HTTP_GET, []()
                   { mws.webserver->send(200, "application/json", DisplayManager.ledsAsJson()); });

    mws.addHandler("/api/indicator1", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        if (DisplayManager.indicatorParser(1, mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            sendJsonError(); });

    mws.addHandler("/api/indicator2", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        if (DisplayManager.indicatorParser(2, mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            sendJsonError(); });

    mws.addHandler("/api/indicator3", HTTP_POST, []()
                   {
        if (isBodyOversized()) { sendBadRequest(); return; }
        if (DisplayManager.indicatorParser(3, mws.webserver->arg("plain").c_str()))
            sendOk();
        else
            sendJsonError(); });

    mws.addHandler("/api/doupdate", HTTP_POST, []()
                   {
        if (UpdateManager.checkUpdate(true))
        {
            sendOk();
            UpdateManager.updateFirmware();
        }
        else
        {
            mws.webserver->send(404, F("text/plain"), F("NoUpdateFound"));
        } });

    mws.addHandler("/api/r2d2", HTTP_POST, []()
                   { PeripheryManager.r2d2(mws.webserver->arg("plain").c_str()); sendOk(); });
}

// --- Async HTTP worker task (runs on core 0) ---

void ServerManager_::httpWorkerTask(void *param)
{
    ButtonEvent event;
    const char *btnNames[] = {"left", "middle", "right"};

    for (;;)
    {
        if (xQueueReceive(buttonQueue, &event, portMAX_DELAY) != pdTRUE)
            continue;

        if (event.btn > 2)
            continue;

        // Copy BUTTON_CALLBACK under mutex for safety in case settings reload is added in the future
        String callbackUrl;
        if (xSemaphoreTake(callbackMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            callbackUrl = BUTTON_CALLBACK;
            xSemaphoreGive(callbackMutex);
        }

        if (callbackUrl.isEmpty())
            continue;

        char payload[128];
        snprintf(payload, sizeof(payload), "button=%s&state=%d&uid=%s",
                 btnNames[event.btn], event.state, uniqueID);

        HTTPClient http;
        http.setTimeout(3000);
        http.begin(callbackUrl);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.POST(payload);
        http.end();
    }
}

// --- Core methods ---

void ServerManager_::erase()
{
    DisplayManager.HSVtext(0, 6, "RESET", true, 0);
    wifi_config_t conf;
    memset(&conf, 0, sizeof(conf));
    esp_wifi_set_config(WIFI_IF_STA, &conf);
    LittleFS.format();
    delay(200);
    formatSettings();
    delay(200);
}

void ServerManager_::setup()
{
    esp_wifi_set_max_tx_power(80);
    esp_wifi_set_ps(WIFI_PS_NONE);

    if (!local_IP.fromString(NET_IP) || !gateway.fromString(NET_GW) || !subnet.fromString(NET_SN) || !primaryDNS.fromString(NET_PDNS) || !secondaryDNS.fromString(NET_SDNS))
        NET_STATIC = false;

    if (NET_STATIC)
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);

    WiFi.setHostname(HOSTNAME.c_str());
    myIP = mws.startWiFi(AP_TIMEOUT * 1000, HOSTNAME.c_str(), "12345678");
    isConnected = !(myIP == IPAddress(192, 168, 4, 1));

    if (DEBUG_MODE)
        DEBUG_PRINTF("My IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);

    mws.setAuth(AUTH_USER, AUTH_PASS);

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
        mws.addOptionBox("Auth");
        mws.addOption("Auth Username", AUTH_USER);
        mws.addOption("Auth Password", AUTH_PASS);
        mws.addHandler("/save", HTTP_POST, []()
                       { ServerManager.loadSettings(); mws.webserver->send(200); });
        registerHttpHandlers();
        udp.begin(UDP_PORT);
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Webserver loaded"));
    }

    mws.addHandler("/version", HTTP_GET, []()
                   { mws.webserver->send(200, F("text/plain"), VERSION); });
    mws.begin(WEB_PORT);

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

    configTzTime(NTP_TZ.c_str(), NTP_SERVER.c_str());
    tm timeInfo;
    getLocalTime(&timeInfo);

    tcpServer.begin();
    tcpServer.setNoDelay(true);

    // Start async HTTP worker for button callbacks
    callbackMutex = xSemaphoreCreateMutex();
    buttonQueue = xQueueCreate(8, sizeof(ButtonEvent));
    if (buttonQueue != nullptr && callbackMutex != nullptr)
    {
        xTaskCreatePinnedToCore(httpWorkerTask, "HTTPWorker", 8192, nullptr, 1, &httpTaskHandle, 0);
    }
}

void ServerManager_::handleUdpDiscovery()
{
    int packetSize = udp.parsePacket();
    if (!packetSize)
        return;

    int len = udp.read(udpBuffer, UDP_BUFFER_SIZE - 1);
    if (len > 0)
        udpBuffer[len] = '\0';

    if (strcmp(udpBuffer, "FIND_AWTRIX") != 0)
        return;

    udp.beginPacket(udp.remoteIP(), UDP_RESPONSE_PORT);
    if (WEB_PORT != 80)
    {
        char response[128];
        snprintf(response, sizeof(response), "%s:%d", HOSTNAME.c_str(), WEB_PORT);
        udp.print(response);
    }
    else
    {
        udp.print(HOSTNAME.c_str());
    }
    udp.endPacket();
}

void ServerManager_::handleTcpClient()
{
    if (!tcpClient || !tcpClient.connected())
    {
        if (tcpServer.hasClient())
        {
            if (tcpClient)
            {
                tcpClient.stop();
                if (DEBUG_MODE)
                    DEBUG_PRINTLN(F("Previous TCP client disconnected for new client"));
            }
            tcpClient = tcpServer.available();
            if (DEBUG_MODE)
                DEBUG_PRINTLN(F("New TCP client connected"));
        }
        return;
    }

    while (tcpClient.available())
    {
        char incomingByte = tcpClient.read();
        if (incomingByte == '\n')
        {
            tcpBuffer[tcpBufferIndex] = '\0';
            GameManager.ControllerInput(tcpBuffer);
            tcpBufferIndex = 0;
        }
        else if (incomingByte != '\r')
        {
            if (tcpBufferIndex < TCP_BUFFER_SIZE - 1)
            {
                tcpBuffer[tcpBufferIndex++] = incomingByte;
            }
            else
            {
                if (DEBUG_MODE)
                    DEBUG_PRINTLN(F("TCP buffer overflow, dropping data"));
                tcpBufferIndex = 0;
            }
        }
    }
}

void ServerManager_::tick()
{
    mws.run();

    if (!AP_MODE)
        handleUdpDiscovery();

    handleTcpClient();
}

void ServerManager_::sendTCP(String message)
{
    if (tcpClient && tcpClient.connected())
        tcpClient.print(message);
}

void ServerManager_::loadSettings()
{
    if (!LittleFS.exists("/DoNotTouch.json"))
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Webserver configuration file not exist"));
        return;
    }

    File file = LittleFS.open("/DoNotTouch.json", "r");
    DynamicJsonDocument doc(file.size() * 2 + 64);
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        if (DEBUG_MODE)
        {
            DEBUG_PRINTLN(F("Failed to parse webserver config"));
            DEBUG_PRINTLN(error.c_str());
        }
        file.close();
        return;
    }

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
}

void ServerManager_::sendButton(byte btn, bool state)
{
    if (btn > 2 || buttonQueue == nullptr)
        return;

    if (btnStates[btn] == state)
        return;
    btnStates[btn] = state;

    ButtonEvent event = {btn, state};
    xQueueSend(buttonQueue, &event, 0); // Non-blocking
}
