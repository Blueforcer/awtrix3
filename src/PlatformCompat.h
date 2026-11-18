#ifndef PLATFORM_COMPAT_H
#define PLATFORM_COMPAT_H

// Pulls in the WiFi / HTTP client headers for the current chip (ESP32 or ESP8266).
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#endif // PLATFORM_COMPAT_H
