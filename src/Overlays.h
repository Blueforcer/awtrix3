#ifndef Overlays_H
#define Overlays_H

#include "MatrixDisplayUi.h"

uint32_t TextEffect(uint32_t color, uint32_t fade, uint32_t blink);

struct Notification
{
    bool center;
    String drawInstructions;
    float scrollposition = 34;
    int16_t scrollDelay = 0;
    String text;
    uint32_t color;
    bool soundPlayed = false;
    File icon;
    bool rainbow;
    bool isGif;
    int fade = 0;
    int blink = 0;
    int iconOffset;
    unsigned long startime = 0;
    long duration = 0;
    int16_t repeat = -1;
    bool hold = false;
    byte textCase = 0;
    byte pushIcon = 0;
    float iconPosition = 0;
    bool iconWasPushed = false;
    int barData[16] = {0};
    int lineData[16] = {0};
    int barSize;
    int lineSize;
    std::vector<uint32_t> colors;
    std::vector<String> fragments;
    int textOffset;
    int progress = -1;
    uint32_t pColor;
    int effect = -1;
    uint32_t background = 0;
    uint32_t pbColor;
    bool wakeup;
    float scrollSpeed = 100;
    bool topText = true;
    bool noScrolling = true;
    String sound;
    bool loopSound;
    String rtttl;
    int gradient[2] = {0};
};
extern std::vector<Notification> notifications;
extern bool notifyFlag;

void StatusOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

void MenuOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

void NotifyOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

extern OverlayCallback overlays[];
#endif