#include <UpdateManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include "DisplayManager.h"
#include <Ticker.h>
#include "Globals.h"

#define URL_fw_Version "https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/version"

#ifdef ULANZI
#define URL_fw_Bin "https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/ulanzi_flasher/firmware/firmware.bin"
#else
#define URL_fw_Bin "https://raw.githubusercontent.com/Blueforcer/awtrix-light/main/docs/awtrix2_flasher/firmware/firmware.bin"
#endif

// The getter for the instantiated singleton instance
UpdateManager_ &UpdateManager_::getInstance()
{
    static UpdateManager_ instance;
    return instance;
}

// Initialize the global shared instance
UpdateManager_ &UpdateManager = UpdateManager.getInstance();

void update_started()
{
    
}

void update_finished()
{

}

void update_progress(int cur, int total)
{
    DisplayManager.clear();
    int progress = (cur * 100) / total;
    char progressStr[5];
    snprintf(progressStr, 5, "%d%%", progress);
     DisplayManager.resetTextColor();
    DisplayManager.printText(0, 6, progressStr, true, false);
    DisplayManager.drawProgressBar(0, 7, progress, 0xCE59, 0x07E0);
    DisplayManager.show();
}

void update_error(int err)
{
    DisplayManager.clear();
    DisplayManager.printText(0, 6, "FAIL", true, true);
    DisplayManager.show();
}

void UpdateManager_::updateFirmware()
{
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);

    httpUpdate.onStart(update_started);
    httpUpdate.onEnd(update_finished);
    httpUpdate.onProgress(update_progress);
    httpUpdate.onError(update_error);

    t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        if (DEBUG_MODE) DEBUG_PRINTF("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        if (DEBUG_MODE) DEBUG_PRINTLN(F("HTTP_UPDATE_NO_UPDATES"));
        break;

    case HTTP_UPDATE_OK:
        if (DEBUG_MODE) DEBUG_PRINTLN(F("HTTP_UPDATE_OK"));
        break;
    }
}

bool UpdateManager_::checkUpdate(bool withScreen)
{
    if (withScreen)
    {
        DisplayManager.clear();
        DisplayManager.resetTextColor();
        DisplayManager.printText(0, 6, "CHECK", true, true);
        DisplayManager.show();
    }

    String payload;
    int httpCode = -1; // Setzen Sie den Standardwert auf einen ungültigen HTTP-Code
    String fwurl = "";
    fwurl += URL_fw_Version;
    fwurl += "?";
    fwurl += String(rand());
    if (DEBUG_MODE) DEBUG_PRINTLN(F("Check firmwareversion"));

    static WiFiClientSecure client; // Statische Variable

    client.setCACert(rootCACertificate);
    HTTPClient https;

    if (https.begin(client, fwurl))
    { // HTTPS
        delay(100);
        httpCode = https.GET();
        delay(100);
        if (httpCode == HTTP_CODE_OK) // if version received
        {
            payload = https.getString(); // save received version
        }
        else
        {
            if (DEBUG_MODE) DEBUG_PRINTLN(F("Error in downloading version file"));
            if (withScreen)
            {
                DisplayManager.clear();
                 DisplayManager.resetTextColor();
                DisplayManager.printText(0, 6, "ERR CNCT", true, true);
                DisplayManager.show();
                delay(1000);
            }
        }
        https.end();
    }

    if (httpCode == HTTP_CODE_OK) // if version received
    {
        payload.trim();
        if (payload.equals(VERSION))
        {
            UPDATE_AVAILABLE = false;
            if (DEBUG_MODE) DEBUG_PRINTF("\nDevice already on latest firmware version: %s\n", VERSION);
            if (withScreen)
            {
                DisplayManager.clear();
                 DisplayManager.resetTextColor();
                DisplayManager.printText(0, 6, "NO UP :(", true, true);
                DisplayManager.show();
                delay(1000);
            }
            return 0;
        }
        else
        {
            if (DEBUG_MODE) DEBUG_PRINTLN(F("New firmwareversion found!"));
            UPDATE_AVAILABLE = true;
            return 1;
        }
    }
    UPDATE_AVAILABLE = false;
    return 0;
}

void checkUpdateNoReturn()
{
    Serial.println("Check Update");
    UpdateManager.getInstance().checkUpdate(false);
}

void UpdateManager_::setup()
{
   
}