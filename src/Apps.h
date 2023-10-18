#ifndef AppS_H
#define AppS_H

#include <map>
#include "MatrixDisplayUi.h"
#include "effects.h"

struct CustomApp
{
    bool hasCustomColor = false;
    uint8_t currentFrame = 0;
    String iconName;
    String drawInstructions;
    float scrollposition = 0;
    int16_t scrollDelay = 0;
    byte lifetimeMode = 0;
    String text;
    uint32_t color;
    File icon;
    bool isGif;
    bool rainbow;
    bool center;
    int fade = 0;
    int blink = 0;
    int effect = -1;
    long duration = 0;
    byte textCase = 0;
    int16_t repeat = 0;
    int16_t currentRepeat = 0;
    String name;
    byte pushIcon = 0;
    float iconPosition = 0;
    bool iconWasPushed = false;
    int barData[16] = {0};
    int lineData[16] = {0};
    int gradient[2] = {0};
    int barSize;
    int lineSize;
    long lastUpdate;
    uint64_t lifetime;
    std::vector<uint32_t> colors;
    std::vector<String> fragments;
    int textOffset;
    int iconOffset;
    int progress = -1;
    uint32_t pColor;
    uint32_t background = 0;
    uint32_t pbColor;
    float scrollSpeed = 100;
    bool topText = true;
    bool noScrolling = true;
    bool lifeTimeEnd = false;
};

extern std::vector<std::pair<String, AppCallback>> Apps;
extern String currentCustomApp;
extern std::map<String, CustomApp> customApps;
extern void (*customAppCallbacks[20])(FastLED_NeoMatrix *, MatrixDisplayUiState *, int16_t, int16_t, GifPlayer *);

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

CustomApp *getCustomAppByName(String name);

String getAppNameByFunction(AppCallback AppFunction);

String getAppNameAtIndex(int index);

int findAppIndexByName(const String &name);

const char *getTimeFormat();

void TimeApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);

void DateApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);

void TempApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);

uint32_t fadeColor(uint32_t color, uint32_t interval);

void StatusOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

void HumApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);

uint32_t TextEffect(uint32_t color, uint32_t fade, uint32_t blink);

#ifndef awtrix2_upgrade
void BatApp(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
#endif

void MenuOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

void ShowCustomApp(String name, FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);

void NotifyOverlay(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, GifPlayer *gifPlayer);

extern OverlayCallback overlays[];

// Unattractive to have a function for every customapp wich does the same, but currently still no other option found TODO

void CApp1(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp2(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp3(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp4(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp5(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp6(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp7(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp8(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp9(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp10(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp11(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp12(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp13(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp14(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp15(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp16(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp17(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp18(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp19(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
void CApp20(FastLED_NeoMatrix *matrix, MatrixDisplayUiState *state, int16_t x, int16_t y, GifPlayer *gifPlayer);
#endif