
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
 *   AWTRIX Light, a custom firmware for the Ulanzi clock                  *
 *                                                                         *
 *   Copyright (C) 2023  Stephan Mühl aka Blueforcer                       *
 *                                                                         *
 *   This work is licensed under a                                         *
 *   Creative Commons Attribution-NonCommercial-ShareAlike                 *
 *   4.0 International License.                                            *
 *                                                                         *
 *   More information:                                                     *
 *   https://github.com/Blueforcer/awtrix-light/blob/main/LICENSE.md       *
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

TaskHandle_t taskHandle;
volatile bool StopTask = false;

void BootAnimation(void *parameter)
{
  const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
  while (true)
  {
    if (StopTask)
    {
      break;
    }
    DisplayManager.HSVtext(4, 6, "AWTRIX", true);
    vTaskDelay(xDelay);
  }
  vTaskDelete(NULL);
}

void setup()
{
  PeripheryManager.setup();
  delay(500);
  Serial.begin(9600);
  loadSettings();
  ServerManager.loadSettings();
  DisplayManager.setup();
  DisplayManager.HSVtext(9, 6, VERSION, true);
  delay(500);
  PeripheryManager.playBootSound();
  xTaskCreatePinnedToCore(BootAnimation, "Task", 10000, NULL, 1, &taskHandle, 1);
  ServerManager.setup();

  if (ServerManager.isConnected)
  {
    MQTTManager.setup();
    DisplayManager.loadApps();
  }
  else
  {
    AP_MODE = true;
  }

  StopTask = true;
  delay(200);
  DisplayManager.clearMatrix();
}

void loop()
{
  ServerManager.tick();
  DisplayManager.tick();
  if (ServerManager.isConnected)
  {
    PeripheryManager.tick();
    MQTTManager.tick();
  }
}
