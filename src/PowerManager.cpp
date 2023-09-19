#include <PowerManager.h>
#include <ArduinoJson.h>
#include "Globals.h"

#define uS_TO_S_FACTOR 1000000

// The getter for the instantiated singleton instance
PowerManager_ &PowerManager_::getInstance()
{
    static PowerManager_ instance;
    return instance;
}

// Initialize the global shared instance
PowerManager_ &PowerManager = PowerManager.getInstance();

void PowerManager_::setup()
{
   
}

void PowerManager_::sleepParser(const char *json)
{
  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    if (DEBUG_MODE)
      DEBUG_PRINTLN(F("Failed to parse json"));
    return;
  }

  if (doc.containsKey("sleep"))
  {
    uint32_t seconds = doc["sleep"].as<uint32_t>();
    sleep(seconds);
  }
}

void PowerManager_::sleep(uint32_t seconds)
{
  esp_sleep_enable_timer_wakeup(seconds * uS_TO_S_FACTOR);
  Serial.print("Going to sleep...\n");
  esp_deep_sleep_start();
}