#ifndef EFFECTS_H
#define EFFECTS_H

#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>
#include "DisplayManager.h"

typedef void (*EffectFunc)(FastLED_NeoMatrix *, int16_t, int16_t);

struct Effect
{
    String name;
    EffectFunc func;
};
extern Effect effects[];
void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, int index);
int getEffectIndex(String name);
#endif