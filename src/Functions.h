#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <map>
#include <FastLED_NeoMatrix.h>
#include <Globals.h>
#include <ArduinoJson.h>

CRGB kelvinToRGB(int kelvin);

uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v);

uint32_t hexToUint32(const char *hexString);

uint32_t getColorFromJsonVariant(JsonVariant colorVariant, uint32_t defaultColor);

double roundToDecimalPlaces(double value, int places);

float getTextWidth(const char *text, byte textCase);

byte utf8ascii(byte ascii);

String utf8ascii(String s);
#endif