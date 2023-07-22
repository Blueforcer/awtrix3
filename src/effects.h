#ifndef EFFECTS_H
#define EFFECTS_H

#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>

typedef void (*EffectFunc)(FastLED_NeoMatrix *, int16_t, int16_t);

struct Effect
{
    String name;
    EffectFunc func;
};
extern Effect effects[]; // Extern, da die Implementierung in der .cpp-Datei liegt
void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, String name);
#endif