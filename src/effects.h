#ifndef EFFECTS_H
#define EFFECTS_H

#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>
#include "DisplayManager.h"


enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct EffectSettings
{
    double speed;
    CRGBPalette16 palette;
    Direction direction;
    EffectSettings(double s = 2, CRGBPalette16 p = OceanColors_p, Direction d = LEFT) : speed(s), palette(p), direction(d) {}
};

typedef void (*EffectFunc)(FastLED_NeoMatrix *, int16_t, int16_t, EffectSettings *);

struct Effect
{
    String name;
    EffectFunc func;
    EffectSettings settings;
};

const int numOfEffects = 20;
extern Effect effects[];
void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, u_int8_t index);
int getEffectIndex(String name);
void updateEffectSettings(u_int8_t index, String json);
#endif