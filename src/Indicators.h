#ifndef INDICATORS_H
#define INDICATORS_H
#include <cstdint>

/// NOTE from the external view, all indicators are indexed from 1 to match the rest of the project. internally mapped
static const int INDICATOR_COUNT = 3; // you can increase this if you want more pixels. Must match the static declarations in the .cpp

// ADVANCED:  added features for per pixel operations.
//   when accessing standard, not perpixel versions, set sets all pixels. get returns the first. Emulates solid color.
// -- pixels are 0 indexed. but use this flag to set/get ALL pixels at once. 

const int ALL_PIXELS = -1; 

constexpr bool DEFAULT_STATE = false;  // boot time default
constexpr int DEFAULT_BLINK = 0;
constexpr int DEFAULT_FADE = 0;

constexpr uint32_t DEFAULT_1_COLOR = 0xFF0000;  // R 
constexpr uint32_t DEFAULT_2_COLOR = 0x00FF00;  // G
constexpr uint32_t DEFAULT_3_COLOR = 0x0000FF;  // B

struct Coord {  // coordinate of a single indicator pixel
    int x;
    int y;
};

struct IndicatorPixelData // data for a single pixel
{
  Coord coord;
  uint32_t color;
  bool state;
  int blink;
  int fade;

  // default values
  IndicatorPixelData(Coord c, uint32_t col) : coord(c), color(col), state(DEFAULT_STATE), blink(DEFAULT_BLINK), fade(DEFAULT_FADE) {}
};

struct Indicator {   // an indicator consists of a number of pixels
    int pixelCount;
    IndicatorPixelData* data;
};

Indicator* getIndicatorPtr(int indicator);  // returns a pointer to an indicator. careful of pixel count for data array length

//// solid indicators, all same state
bool getIndicatorState(int indicator);
void setIndicatorState(int indicator, bool state);

uint32_t getIndicatorColor(int indicator);
void setIndicatorColor(int indicator, uint32_t color);  

int getIndicatorBlink(int indicator);
void setIndicatorBlink(int indicator, int blink); // all at once

int getIndicatorFade(int indicator);
void setIndicatorFade(int indicator, int fade);  // all at once

//// per pixel operations

bool getIndicatorState(int indicator, int pixel);  
void setIndicatorState(int indicator, int pixel, bool state);

uint32_t getIndicatorColor(int indicator, int pixel);
void setIndicatorColor(int indicator, int pixel, uint32_t color);

int getIndicatorBlink(int indicator, int pixel);
void setIndicatorBlink(int indicator, int pixel, int blink);

int getIndicatorFade(int indicator, int pixel);
void setIndicatorFade(int indicator, int pixel, int fade);
#endif // INDICATORS_H
