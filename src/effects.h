#ifndef EFFECTS_H
#define EFFECTS_H

#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <ArduinoJson.h>
#include "DisplayManager.h"

struct EffectSettings
{
    double speed;
    CRGBPalette16 palette;
    bool blend;
    EffectSettings(double s = 2, CRGBPalette16 p = OceanColors_p, bool b = false) : speed(s), palette(p), blend(b) {}
};

typedef void (*EffectFunc)(FastLED_NeoMatrix *, int16_t, int16_t, EffectSettings *);

struct Effect
{
    String name;
    EffectFunc func;
    EffectSettings settings;
};

enum OverlayEffect
{
    NONE,
    DRIZZLE,
    RAIN,
    SNOW,
    STORM,
    THUNDER,
    FROST,
};



#ifdef AWTRIX_16X16
// The effects are drawn for 32x8 and aren't available on the 16x16 build. Leaving the
// table out lets the linker drop the effects and their data (~2.5 KB RAM).
const int numOfEffects = 0;
#else
const int numOfEffects = 19;
extern Effect effects[];
#endif
void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, u_int8_t index);
int getEffectIndex(String name);
void updateEffectSettings(u_int8_t index, String json);
void EffectOverlay(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, OverlayEffect effect);
OverlayEffect getOverlay(String overlay);
#endif