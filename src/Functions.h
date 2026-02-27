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

uint32_t TextEffect(uint32_t color, uint32_t fade, uint32_t blink);

uint32_t fadeColor(uint32_t color, uint32_t interval);

uint32_t getColorField(JsonObject doc, const char *key, uint32_t defaultColor);

void parseGradient(JsonObject doc, int gradient[2]);

void parseBarLineData(JsonObject doc, int barData[16], int &barSize, int lineData[16], int &lineSize, uint32_t &barBG, bool autoscale);

bool parseCRGB(JsonObject doc, const char *key, CRGB &out);

#endif