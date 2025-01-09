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
#include "esp_wifi.h"
#include <ESPAsyncHTTPUpdateServer.h>
#include <uri/UriRegex.h>
#include <DNSServer.h>

WiFiUDP udp;

DNSServer dnsServer;
unsigned int localUdpPort = 4210;
char incomingPacket[255];
ESPAsyncHTTPUpdateServer updateServer;
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

bool deleteRecursive(String path)
{
    File file = LittleFS.open(path.c_str(), "r");
    if (!file.isDirectory())
    {
        file.close();
        return LittleFS.remove(path.c_str());
    }

    File child = file.openNextFile();
    while (child)
    {
        String childPath = path + "/" + child.name();
        if (child.isDirectory())
        {
            child.close();
            deleteRecursive(childPath);
        }
        else
        {
            child.close();
            LittleFS.remove(childPath.c_str());
        }
        child = file.openNextFile();
    }

    file.close();
    return LittleFS.rmdir(path.c_str());
}

bool createDirFromPath(const String &path)
{
    String dir;
    int p1 = 0;
    int p2 = 0;
    while (p2 != -1)
    {
        p2 = path.indexOf("/", p1 + 1);
        dir += path.substring(p1, p2);

        // Prüfen ob es ein gültiges Verzeichnis ist
        if (dir.indexOf(".") == -1)
        {
            if (!LittleFS.exists(dir))
            {
                if (LittleFS.mkdir(dir))
                {
                    Serial.printf("Folder %s created\n", dir.c_str());
                }
                else
                {
                    Serial.printf("Error. Folder %s not created\n", dir.c_str());
                    return false;
                }
            }
        }
        p1 = p2;
    }
    return true;
}

void checkForUnsupportedPath(String &filename, String &error)
{
    if (!filename.startsWith("/"))
    {
        error += F(" !! NO_LEADING_SLASH !! ");
    }
    if (filename.indexOf("//") != -1)
    {
        error += F(" !! DOUBLE_SLASH !! ");
    }
    if (filename.endsWith("/"))
    {
        error += F(" ! TRAILING_SLASH ! ");
    }
    Serial.printf("Check path: %s: %s\n", filename.c_str(), error.c_str());
}

void addHandler()
{

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                         {
    String bodyContent = String((char*)data);

  if (request->url() == "/api/system") {
     String bodyContent = String((char*)data);
     setSettingsFromJson(bodyContent);
     request->send(200, "text/plain", "true");

       } else if  (request->url() == "/api/power") {
            DisplayManager.powerStateParse(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } else if (request->url() == "/api/sleep") {
            DisplayManager.setPower(false);
            PowerManager.sleepParser(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } else if (request->url() == "/api/rtttl") {
            PeripheryManager.playRTTTLString(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } else if (request->url() == "/api/sound") {
            if (PeripheryManager.parseSound(bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(404, "text/plain", "FileNotFound");
            }
        } else if (request->url() == "/api/moodlight") {
            if (DisplayManager.moodlight(bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "ErrorParsingJson");
            }
        } else if (request->url() == "/api/notify") {
            if (DisplayManager.generateNotification(1, bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "ErrorParsingJson");
            }
        } else if (request->url() == "/api/apps") {
            DisplayManager.updateAppVector(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } else if (request->url() == "/api/switch") {
            if (DisplayManager.switchToApp(bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "FAILED");
            }
        } else if (request->url() == "/api/settings") {
            DisplayManager.setNewSettings(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } else if (request->url() == "/api/reorder") {
            DisplayManager.reorderApps(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } else if (request->url() == "/api/custom") {
            if (DisplayManager.parseCustomPage(request->arg("name"), bodyContent.c_str(), false)) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "ErrorParsingJson");
            }
        } else if (request->url() == "/api/indicator1") {
            if (DisplayManager.indicatorParser(1, bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "ErrorParsingJson");
            }
        } else if (request->url() == "/api/indicator2") {
            if (DisplayManager.indicatorParser(2, bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "ErrorParsingJson");
            }
        } else if (request->url() == "/api/indicator3") {
            if (DisplayManager.indicatorParser(3, bodyContent.c_str())) {
                request->send(200, "text/plain", "OK");
            } else {
                request->send(500, "text/plain", "ErrorParsingJson");
            }
        } else if (request->url() == "/api/r2d2") {
            PeripheryManager.r2d2(bodyContent.c_str());
            request->send(200, "text/plain", "OK");
        } });

    server.on("/api/system", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String settingsJson = getSettingsAsJson();
    request->send(200, "application/json", settingsJson); });

    server.on("/api/system", HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              { request->send(204); });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", html); });

    server.on("/api/loop", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getAppsAsJson().c_str()); });

    server.on("/api/effects", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getEffectNames().c_str()); });

    server.on("/api/transitions", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getTransitionNames().c_str()); });

    server.on("/api/reboot", HTTP_ANY, [](AsyncWebServerRequest *request)
              { request->send(200, F("text/plain"), F("OK")); delay(200); ESP.restart(); });

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

    server.on("/api/apps", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getAppsWithIcon().c_str()); });

    server.on("/api/erase", HTTP_ANY, [](AsyncWebServerRequest *request)
              { ServerManager.erase();  request->send(200, F("text/plain"), F("OK")); delay(200); ESP.restart(); });

    server.on("/api/resetSettings", HTTP_ANY, [](AsyncWebServerRequest *request)
              { formatSettings();   request->send(200, F("text/plain"), F("OK")); delay(200); ESP.restart(); });

    server.on("/api/reorder", HTTP_POST, [](AsyncWebServerRequest *request)
              { DisplayManager.reorderApps(request->arg("plain").c_str()); request->send(200, F("text/plain"), F("OK")); });

    server.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getSettings().c_str()); });

    server.on("/api/stats", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.getStats().c_str()); });

    server.on("/api/screen", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "application/json", DisplayManager.ledsAsJson().c_str()); });

    server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request)
              {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        String newSSID = request->getParam("ssid", true)->value();
        String newPass = request->getParam("password", true)->value();

        // Alte Zugangsdaten sichern
        String currentSSID = WiFi.SSID();
        String currentPass = WiFi.psk();

        // Verbindung mit neuen Daten testen
        WiFi.begin(newSSID.c_str(), newPass.c_str());
        Serial.print(F("Testing connection with new credentials: "));
        Serial.println(newSSID);

        if (WiFi.waitForConnectResult(10000) == WL_CONNECTED) { // 10 Sekunden warten
            Serial.println(F("New credentials are valid, saving..."));

            // Neue Daten speichern (z.B. in NVS oder Preferences)
            saveWiFiCredentials(newSSID, newPass);

            // Antwort senden
            request->send(200, "application/json", "{\"status\":\"success\",\"message\":\"WiFi credentials updated.\"}");
        } else {
            Serial.println(F("New credentials are invalid, reverting to old credentials."));
            
            // Verbindung mit alten Zugangsdaten wiederherstellen
            WiFi.begin(currentSSID.c_str(), currentPass.c_str());

            // Antwort senden
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid WiFi credentials.\"}");
        }
    } else {
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing ssid or password.\"}");
    } });

    server.on("/api/doupdate", HTTP_POST, [](AsyncWebServerRequest *request)
              { 
                    if (UpdateManager.checkUpdate(true)){
                        request->send(200, F("text/plain"), F("OK"));
                        UpdateManager.updateFirmware();
                    }else{
                        request->send(404, F("text/plain"), "NoUpdateFound");    
                    } });

    server.on("/version", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, F("text/plain"), VERSION); });

    updateServer.setup(&server, "", "");
    // Route, die die Icon-Daten bereitstellt
    server.serveStatic("/ICONS", LittleFS, "/ICONS");

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String status = "{\"type\":\"";
    status += "LittleFS";
    status += "\",\"isOk\":";
    status += LittleFS.begin() ? "true" : "false";
    status += ",\"totalBytes\":";
    status += LittleFS.totalBytes();
    status += ",\"usedBytes\":";
    status += LittleFS.usedBytes();
    status += "}";
    request->send(200, "application/json", status); });

    // List endpoint - entspricht handleFileList
    server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    if (!request->hasParam("dir")) {
        request->send(400, F("text/plain"), F("DIR ARG MISSING"));
        return;
    }

    String path = request->arg("dir");
    if (path != "/" && !LittleFS.exists(path)) {
        request->send(400, F("text/plain"), F("BAD PATH"));
        return;
    }

    File root = LittleFS.open(path, "r");
    String output = "[";
    if (root && root.isDirectory()) {
        File file = root.openNextFile();
        bool firstFile = true;
        while (file) {
            if (!firstFile) {
                output += ",";
            }
            firstFile = false;
            
            String filename = file.name();
            if (filename.lastIndexOf("/") > -1) {
                filename.remove(0, filename.lastIndexOf("/") + 1);
            }
            
            output += "{\"type\":\"";
            output += (file.isDirectory()) ? "dir" : "file";
            output += "\",\"size\":\"";
            output += file.size();
            output += "\",\"name\":\"";
            output += filename;
            output += "\"}";
            
            file = root.openNextFile();
        }
    }
    output += "]";
    request->send(200, "application/json", output); });

    // Edit endpoint - GET - entspricht handleGetEdit
    server.on("/edit", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    if (!request->hasParam("path")) {
        request->send(400, F("text/plain"), F("PATH ARG MISSING"));
        return;
    }

    String path = request->arg("path");
    if (path.isEmpty() || path == "/") {
        request->send(400, F("text/plain"), F("BAD PATH"));
        return;
    }

    if (!LittleFS.exists(path)) {
        request->send(404, F("text/plain"), F("FILE NOT FOUND"));
        return;
    }

    File file = LittleFS.open(path, "r");
    if (!file || file.isDirectory()) {
        request->send(500, F("text/plain"), F("FILE OPEN FAILED"));
        return;
    }

    request->send(LittleFS, path, "text/plain"); });

    server.on("/edit", HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
    AsyncWebServerResponse *response = request->beginResponse(204);
    request->send(response); });

    server.on("/list", HTTP_OPTIONS, [](AsyncWebServerRequest *request)
              {
    AsyncWebServerResponse *response = request->beginResponse(204);
    request->send(response); });

    server.on("/edit", HTTP_PUT, [](AsyncWebServerRequest *request)
              {
    String path = request->arg("path");
    if (path.isEmpty()) {
        request->send(400, F("text/plain"), F("PATH ARG MISSING"));
        return;
    }
    if (path == "/") {
        request->send(400, F("text/plain"), F("BAD PATH"));
        return;
    }

    String src = request->arg("src");
    if (src.isEmpty()) {
        // Keine Quelle angegeben: Neue Datei/Verzeichnis erstellen
        if (path.endsWith("/")) {
            // Verzeichnis erstellen
            path.remove(path.length() - 1);
            if (!LittleFS.mkdir(path)) {
                request->send(500, F("text/plain"), F("MKDIR FAILED"));
                return;
            }
        } else {
            // Datei erstellen
            File file = LittleFS.open(path, "w");
            if (!file) {
                request->send(500, F("text/plain"), F("CREATE FAILED"));
                return;
            }
            file.close();
        }
        request->send(200, F("text/plain"), path);
    } else {
        // Quelle angegeben: Umbenennen/Verschieben
        if (src == "/") {
            request->send(400, F("text/plain"), F("BAD SRC"));
            return;
        }
        if (!LittleFS.exists(src)) {
            request->send(400, F("text/plain"), F("SRC FILE NOT FOUND"));
            return;
        }

        if (path.endsWith("/")) {
            path.remove(path.length() - 1);
        }
        if (src.endsWith("/")) {
            src.remove(src.length() - 1);
        }

        if (!LittleFS.rename(src, path)) {
            request->send(500, F("text/plain"), F("RENAME FAILED"));
            return;
        }
        request->send(200, F("text/plain"), F("OK"));
    } });

    // Edit endpoint - DELETE - entspricht handleFileDelete
    server.on("/edit", HTTP_DELETE, [](AsyncWebServerRequest *request)
              {
    String path = request->arg("path");
    if (path.isEmpty() || path == "/") {
        request->send(400, F("text/plain"), F("BAD PATH"));
        return;
    }

    if (!LittleFS.exists(path)) {
        request->send(404, F("text/plain"), F("FILE NOT FOUND"));
        return;
    }

    // Rekursives Löschen implementieren
    bool success = deleteRecursive(path);
    if (success) {
        request->send(200, F("text/plain"), F("OK"));
    } else {
        request->send(500, F("text/plain"), F("DELETE FAILED"));
    } });

    server.on("/edit", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        // Handler für die Abschluss-Antwort
        request->send(200, F("text/plain"), F("OK")); }, [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
              {
        static File uploadFile;
        static String uploadFilename;
        
        if (!index) {
            // Upload startet
            uploadFilename = filename;
            
            // Stelle sicher, dass Pfade immer mit "/" beginnen
            if (!uploadFilename.startsWith("/")) {
                uploadFilename = "/" + uploadFilename;
            }

            // Pfad überprüfen
            String pathError;
            checkForUnsupportedPath(uploadFilename, pathError);
            
            if (pathError.length() > 0) {
                request->send(500, F("text/plain"), F("INVALID FILENAME"));
                return;
            }

            // Verzeichnisstruktur erstellen
            if (!createDirFromPath(uploadFilename)) {
                request->send(500, F("text/plain"), F("FAILED TO CREATE DIRECTORY"));
                return;
            }

            Serial.printf("Upload Start: %s\n", uploadFilename.c_str());
            uploadFile = LittleFS.open(uploadFilename, "w");
            
            if (!uploadFile) {
                request->send(500, F("text/plain"), F("CREATE FAILED"));
                return;
            }
        }

        // Schreibe Daten
        if (uploadFile && len) {
            if (uploadFile.write(data, len) != len) {
                Serial.println(F("Write Failed"));
                request->send(500, F("text/plain"), F("WRITE FAILED"));
                return;
            }
        }

        if (final) {
            // Upload beendet
            if (uploadFile) {
                uploadFile.close();
            }
            Serial.printf("Upload Complete: %s, Size: %u\n", uploadFilename.c_str(), index + len);
        } });
}

void connectToWiFi(String ssid, String password, AsyncWebServerRequest *request)
{
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print(F("Connecting to: "));
    Serial.println(ssid);

    unsigned long startAttemptTime = millis();
    const unsigned long connectionTimeout = 5000; // 5 Sekunden

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < connectionTimeout)
    {
        Serial.print(F("."));
        delay(100); // Kurze Pause, um den Watchdog zu entlasten
        yield();    // Andere Tasks ausführen lassen
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(F("Connected successfully!"));
        request->send(200, "text/html", "Connected successfully! Restarting...");
        delay(1000);
        ESP.restart(); // Neustart für Client-Modus
    }
    else
    {
        Serial.println(F("Connection failed."));
        request->send(200, "text/html", "Failed to connect. Please try again.");
        WiFi.disconnect(); // Verbindung zurücksetzen
    }
}

void startAccessPoint()
{
    Serial.println(F("Starting Access Point..."));
    WiFi.softAP(uniqueID, "12345678");

    IPAddress LIP(192, 168, 4, 1);
    IPAddress GW(192, 168, 4, 1);
    IPAddress SN(255, 255, 255, 0);
    if (!WiFi.softAPConfig(LIP, GW, SN))
    {
        Serial.println(F("AP Config failed!"));
        return;
    }

    Serial.print(F("Access Point started. IP Address: "));
    Serial.println(WiFi.softAPIP());

    // DNS-Anfragen auf die AP-IP umleiten
    dnsServer.start(53, "*", LIP);

    // Webseite bereitstellen
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", wifipage); });

    // Verbindung mit neuem WLAN einrichten
    server.on("/connect", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            String ssid = request->getParam("ssid", true)->value();
            String password = request->getParam("password", true)->value();
            saveWiFiCredentials(ssid, password);
            request->send(200, "text/html", "alert('Saved credentials. Restarting...');");
            delay(1000);
            esp_restart();
        } else {
            request->send(400, "text/html", "Missing SSID or Password.");
        } });

    server.begin();
}

void ServerManager_::setup()
{
    if (!local_IP.fromString(NET_IP) || !gateway.fromString(NET_GW) || !subnet.fromString(NET_SN) || !primaryDNS.fromString(NET_PDNS))
        NET_STATIC = false;
    if (NET_STATIC)
    {
        WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    }
    WiFi.setHostname(HOSTNAME.c_str()); // define hostname
    WiFi.mode(WIFI_STA);
    String ssid, password;
    loadWiFiCredentials(ssid, password);

    if (ssid.length() > 0)
    {
        WiFi.begin(ssid.c_str(), password.c_str());
        Serial.print(F("Connecting to "));
        Serial.println(ssid);

        if (WiFi.waitForConnectResult(10000) != WL_CONNECTED)
        {
            Serial.println(F("WiFi Failed!"));
            startAccessPoint();
            AP_MODE = true;
            return;
        }

        Serial.print(F("IP Address: "));
        myIP = (WiFi.localIP());
    }
    else
    {
        Serial.println(F("No WiFi credentials found"));
        AP_MODE = true;
        startAccessPoint();
        return;
    }

    isConnected = !(myIP == IPAddress(192, 168, 4, 1));
    if (DEBUG_MODE)
        DEBUG_PRINTF("My IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
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
    }else{
        dnsServer.processNextRequest();
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
