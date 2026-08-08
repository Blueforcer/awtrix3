
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
#ifndef AWTRIX_DISABLE_TIMER
#include "TimerHaHost.h"
#endif
#include "ServerManager.h"
#include "Globals.h"
#include "UpdateManager.h"
#include "timer.h"
#ifndef AWTRIX_DISABLE_TIMER
#include "TimerManager.h"
#endif

TaskHandle_t taskHandle;
volatile bool StopTask = false;
bool stopBoot;

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

void setup()
{
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  delay(2000);
  Serial.begin(115200);
  loadSettings();
  PeripheryManager.setup();
  ServerManager.loadSettings();
  DisplayManager.setup();
#ifndef AWTRIX_DISABLE_TIMER
  TimerManager.setup();
#endif
  DisplayManager.HSVtext(9, 6, VERSION, true, 0);
  delay(500);
  xTaskCreatePinnedToCore(BootAnimation, "Task", 10000, NULL, 1, &taskHandle, 0);
  ServerManager.setup();
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
    }

    
      if (MQTT_HOST != "")
      {
        DisplayManager.HSVtext(4, 6, "MQTT...", true, 0);
#ifndef AWTRIX_DISABLE_TIMER
        TimerHaHost.reconcile();
#endif
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
}

void loop()
{
  timer_tick();
  ServerManager.tick();
  DisplayManager.tick();
#ifndef AWTRIX_DISABLE_TIMER
  TimerManager.tick();
  TimerManager.tickPresence(millis());   // peer presence beacon + registry aging
#endif
  PeripheryManager.tick();
  if (ServerManager.isConnected)
  {
    MQTTManager.tick();
#ifndef AWTRIX_DISABLE_TIMER
    TimerHaHost.refreshTargets(millis());   // dynamic HA Targets select republish
#endif
  }
}
