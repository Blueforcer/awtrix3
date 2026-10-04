#include "Indicators.h"

///---- note, if optimizing, some opportunity here to reduce the passing of the structs on the stack. return / use pointers instead

// ============= Data section   // changes to number of indicators must match in .h INDICATOR_COUNT

IndicatorPixelData _indicator1[] = {
    { {31,0}, DEFAULT_1_COLOR}, // partially fills and uses default value constructor
    { {30,0}, DEFAULT_1_COLOR},
    { {31,1}, DEFAULT_1_COLOR}
};

IndicatorPixelData _indicator2[] = 
{
  { {31,3}, DEFAULT_2_COLOR}, // partially fills and uses default value constructor
  { {31,4}, DEFAULT_2_COLOR}
};

IndicatorPixelData _indicator3[] = 
{
  { {31,6}, DEFAULT_3_COLOR}, // partially fills and uses default value constructor
  { {31,7}, DEFAULT_3_COLOR},
  { {30,7}, DEFAULT_3_COLOR}
};

Indicator indicators[] = {
  {3, _indicator1},  // num of pixels hard coded. could probably do a macro but, meh.
  {2, _indicator2},
  {3, _indicator3}
};


//  ============= Helper fn section to simplify later functions
void setColor(IndicatorPixelData *pd, uint32_t color) { pd->color = color;}  // these are just setters so I can use fn points to reuse code instead of repeating all the fns.
void setState(IndicatorPixelData *pd, bool state) { pd->state = state;}
void setBlink(IndicatorPixelData *pd, int blink) { pd->blink = blink;}
void setFade(IndicatorPixelData *pd, int fade) { pd->fade = fade;}

Indicator* getIndicatorPtr(int indicator) {
  indicator--; // 1-based indexing -> 0 based
  if (indicator >= INDICATOR_COUNT || indicator < 0) indicator = 0; // default to a safe, but wrong, pixel instead of generating NULL pointers
  return &(indicators[indicator]);
}

IndicatorPixelData getPixelData(int indicator, int pixel) {
  Indicator* ind = getIndicatorPtr(indicator);
  if (pixel == ALL_PIXELS || pixel < 0 || pixel >= ind->pixelCount) pixel = 0; // default to first
  return ind->data[pixel];
}

template <typename T>
void setPixelData( void (*fieldSetter)(IndicatorPixelData*, T), int indicator, int pixel, T value) {
  Indicator* ind = getIndicatorPtr(indicator);

  if (pixel == ALL_PIXELS)
  {
    for (int pixel=0; pixel < ind->pixelCount; pixel++) 
    {
      (*fieldSetter)( &(ind->data[pixel]), value);
    }
  } else
  {
    if (pixel < 0 || pixel >= ind->pixelCount)
      pixel = 0; // default to first)
    (*fieldSetter)( &(ind->data[pixel]), value);
  }
}

//=============== on off state
bool getIndicatorState(int indicator, int pixel)              { return getPixelData(indicator, pixel).state; }
bool getIndicatorState(int indicator)                         { return getPixelData(indicator, ALL_PIXELS).state; }
void setIndicatorState(int indicator, int pixel, bool state)  { setPixelData(&setState, indicator, pixel, state);  }
void setIndicatorState(int indicator, bool state)             { setPixelData(&setState, indicator, ALL_PIXELS, state); }

// //=============== color
uint32_t getIndicatorColor(int indicator, int pixel)              { return getPixelData(indicator, pixel).color; }
uint32_t getIndicatorColor(int indicator)                         { return getPixelData(indicator, ALL_PIXELS).color; }
void setIndicatorColor(int indicator, int pixel, uint32_t color)  { setPixelData(&setColor, indicator, pixel, color); }
void setIndicatorColor(int indicator, uint32_t color)             { setPixelData(&setColor, indicator, ALL_PIXELS, color); }

// //=============== blink
int getIndicatorBlink(int indicator, int pixel)              { return getPixelData(indicator, pixel).blink; }
int getIndicatorBlink(int indicator)                         { return getPixelData(indicator, ALL_PIXELS).blink; }
void setIndicatorBlink(int indicator, int pixel, int blink)  { setPixelData(&setBlink, indicator, pixel, blink);  }
void setIndicatorBlink(int indicator, int blink)             { setPixelData(&setBlink, indicator, ALL_PIXELS, blink); }

// //=============== fade
int getIndicatorFade(int indicator, int pixel)             { return getPixelData(indicator, pixel).fade; }
int getIndicatorFade(int indicator)                        { return getPixelData(indicator, ALL_PIXELS).fade; }
void setIndicatorFade(int indicator, int pixel, int fade)  { setPixelData(&setFade, indicator, pixel, fade);  }
void setIndicatorFade(int indicator, int fade)             { setPixelData(&setFade, indicator, ALL_PIXELS, fade); }