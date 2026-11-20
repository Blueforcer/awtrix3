#pragma once

// Current weather pushed in over HTTP (/api/weather) or MQTT ([PREFIX]/weather).
// The 16x16 build shows it on the top half of the time app, with the time below.
#ifdef AWTRIX_16X16
#include <Arduino.h>

// Parses {"temp":4.6,"condition":"rainy"} or {"temp":4.6,"code":61,"is_day":1}.
// An empty payload or {} clears the weather. Returns false on invalid JSON or a missing temp.
bool weatherParser(const char *json);

// True while pushed weather is present and not older than its lifetime
bool weatherAvailable();

// Weather icon and temperature on the top 8 rows
void drawWeather16(int16_t x, int16_t y);
#endif
