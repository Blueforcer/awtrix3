
/*    ___           ___                       ___                       ___
     /  /\         /__/\          ___        /  /\        ___          /__/|
    /  /::\       _\_ \:\        /  /\      /  /::\      /  /\        |  |:|
   /  /:/\:\     /__/\ \:\      /  /:/     /  /:/\:\    /  /:/        |  |:|
  /  /:/~/::\   _\_ \:\ \:\    /  /:/     /  /:/~/:/   /__/::\      __|__|:|
 /__/:/ /:/\:\ /__/\ \:\ \:\  /  /::\    /__/:/ /:/___ \__\/\:\__  /__/::::\____
 \  \:\/:/__\/ \  \:\ \:\/:/ /__/:/\:\   \  \:\/:::::/    \  \:\/\    ~\~~\::::/
  \  \::/       \  \:\ \::/  \__\/  \:\   \  \::/~~~~      \__\::/     |~~|:|~~
   \  \:\        \  \:\/:/        \  \:\   \  \:\          /__/:/      |  |:|
    \  \:\        \  \::/          \__\/    \  \:\         \__\/       |  |:|
     \__\/         \__\/                     \__\/                     |__|/

 ***************************************************************************
 *                                                                         *
 *   AWTRIX 3, a custom firmware for the Ulanzi clock                  *
 *                                                                         *
 *   Copyright (C) 2024  Stephan Mühl aka Blueforcer                       *
 *                                                                         *
 *   This work is licensed under a                                         *
 *   Creative Commons Attribution-NonCommercial-ShareAlike                 *
 *   4.0 International License.                                            *
 *                                                                         *
 *   More information:                                                     *
 *   https://github.com/Blueforcer/awtrix3/blob/main/LICENSE.md       *
 *                                                                         *
 *   This firmware is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 *                                                                         *
 ***************************************************************************/


#include <Arduino.h>
#include "DisplayManager.h"
#include "PeripheryManager.h"
#include "MQTTManager.h"
#include "ServerManager.h"
#include "Globals.h"
#include "UpdateManager.h"
#include "timer.h"

#ifndef ESP8266
TaskHandle_t taskHandle;
#endif

// Build with -D AWTRIX_HEAP_LOG to print free heap during boot and every 2 s.
#if defined(AWTRIX_HEAP_LOG) && defined(ESP8266)
#define HEAP_LOG(step) Serial.printf("[heap] %-14s free %5u  max block %5u\n", step, ESP.getFreeHeap(), ESP.getMaxFreeBlockSize())
#else
#define HEAP_LOG(step)
#endif
volatile bool StopTask = false;
bool stopBoot;

#ifndef ESP8266
void BootAnimation(void *parameter)
{
  const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
  while (true)
  {
    if (StopTask)
    {
      break;
    }
    DisplayManager.HSVtext(4, 6, "AWTRIX", true, 0);
    vTaskDelay(xDelay);
  }
  vTaskDelete(NULL);
}
#endif

void setup()
{
#ifndef ESP8266
  // Buzzer pin on the Ulanzi. On ESP8266 GPIO15 is D8 (right button / boot strap).
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
#endif
  delay(2000);
  Serial.begin(115200);
  HEAP_LOG("start");
  loadSettings();
  HEAP_LOG("settings");
  PeripheryManager.setup();
  HEAP_LOG("periphery");
  ServerManager.loadSettings();
  DisplayManager.setup();
  HEAP_LOG("display");
  DisplayManager.HSVtext(9, 6, VERSION, true, 0);
  delay(500);
#ifdef ESP8266
  // No second core / RTOS task on ESP8266: show a static boot text while WiFi connects
  DisplayManager.HSVtext(4, 6, "AWTRIX", true, 0);
#else
  xTaskCreatePinnedToCore(BootAnimation, "Task", 10000, NULL, 1, &taskHandle, 0);
#endif
  ServerManager.setup();
  HEAP_LOG("wifi+server");
  if (ServerManager.isConnected)
  {
    // timer_init();
    DisplayManager.loadNativeApps();
    DisplayManager.loadCustomApps();
    UpdateManager.setup();
    DisplayManager.startArtnet();
    StopTask = true;
    float x = 4;
    String textForDisplay = "AWTRIX   " + ServerManager.myIP.toString();

    if (WEB_PORT != 80)
    {
      textForDisplay += ":" + String(WEB_PORT);
    }

    int textLength = textForDisplay.length() * 4;
    while (x >= -textLength)
    {
      DisplayManager.HSVtext(x, 6, textForDisplay.c_str(), true, 0);
      x -= 0.18;
#ifdef ESP8266
      yield(); // keep the software watchdog happy during the long scroll
#endif
    }

    
      if (MQTT_HOST != "")
      {
        DisplayManager.HSVtext(4, 6, "MQTT...", true, 0);
        MQTTManager.setup();
        MQTTManager.tick();
      }
    
  }
  else
  {
    AP_MODE = true;
    StopTask = true;
  }
  delay(200);
  DisplayManager.setBrightness(BRIGHTNESS);
  HEAP_LOG("setup done");
}

void loop()
{
#if defined(AWTRIX_HEAP_LOG) && defined(ESP8266)
  static unsigned long lastHeapLog = 0;
  static unsigned long lastLoop = 0, maxLoopUs = 0;
  unsigned long nowUs = micros();
  if (lastLoop && nowUs - lastLoop > maxLoopUs)
    maxLoopUs = nowUs - lastLoop;
  lastLoop = nowUs;
  if (millis() - lastHeapLog > 2000)
  {
    lastHeapLog = millis();
    HEAP_LOG("loop");
    Serial.printf("[loop] longest pass %lu us\n", maxLoopUs);
    maxLoopUs = 0;
  }
#endif
  timer_tick();
  ServerManager.tick();
  DisplayManager.tick();
  PeripheryManager.tick();
  if (ServerManager.isConnected)
  {
    MQTTManager.tick();
  }
}
