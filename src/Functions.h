#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <map>
#include <FastLED_NeoMatrix.h>
#include <Globals.h>

std::map<char, uint16_t> CharMap = {
    {32, 2}, {33, 2}, {34, 4}, {35, 4}, {36, 4}, {37, 4}, {38, 4}, {39, 2}, {40, 3}, {41, 3}, {42, 4}, {43, 4}, {44, 3}, {45, 4}, {46, 2}, {47, 4}, {48, 4}, {49, 4}, {50, 4}, {51, 4}, {52, 4}, {53, 4}, {54, 4}, {55, 4}, {56, 4}, {57, 4}, {58, 2}, {59, 3}, {60, 4}, {61, 4}, {62, 4}, {63, 4}, {64, 4}, {65, 4}, {66, 4}, {67, 4}, {68, 4}, {69, 4}, {70, 4}, {71, 4}, {72, 4}, {73, 2}, {74, 4}, {75, 4}, {76, 4}, {77, 6}, {78, 5}, {79, 4}, {80, 4}, {81, 5}, {82, 4}, {83, 4}, {84, 4}, {85, 4}, {86, 4}, {87, 6}, {88, 4}, {89, 4}, {90, 4}, {91, 4}, {92, 4}, {93, 4}, {94, 4}, {95, 4}, {96, 3}, {97, 4}, {98, 4}, {99, 4}, {100, 4}, {101, 4}, {102, 4}, {103, 4}, {104, 4}, {105, 2}, {106, 4}, {107, 4}, {108, 4}, {109, 4}, {110, 4}, {111, 4}, {112, 4}, {113, 4}, {114, 4}, {115, 4}, {116, 4}, {117, 4}, {118, 4}, {119, 4}, {120, 4}, {121, 4}, {122, 4}, {123, 4}, {124, 2}, {125, 4}, {126, 4}, {161, 2}, {162, 4}, {163, 4}, {164, 4}, {165, 4}, {166, 2}, {167, 4}, {168, 4}, {169, 4}, {170, 4}, {171, 3}, {172, 4}, {173, 3}, {174, 4}, {175, 4}, {176, 4}, {177, 4}, {178, 4}, {179, 4}, {180, 3}, {181, 4}, {182, 4}, {183, 4}, {184, 4}, {185, 2}, {186, 4}, {187, 3}, {188, 4}, {189, 4}, {190, 4}, {191, 4}, {192, 4}, {193, 4}, {194, 4}, {195, 4}, {196, 4}, {197, 4}, {198, 4}, {199, 4}, {200, 4}, {201, 4}, {202, 4}, {203, 4}, {204, 4}, {205, 4}, {206, 4}, {207, 4}, {208, 4}, {209, 4}, {210, 4}, {211, 4}, {212, 4}, {213, 4}, {214, 4}, {215, 4}, {216, 4}, {217, 4}, {218, 4}, {219, 4}, {220, 4}, {221, 4}, {222, 4}, {223, 4}, {224, 4}, {225, 4}, {226, 4}, {227, 4}, {228, 4}, {229, 4}, {230, 4}, {231, 4}, {232, 4}, {233, 4}, {234, 4}, {235, 4}, {236, 3}, {237, 3}, {238, 4}, {239, 4}, {240, 4}, {241, 4}, {242, 4}, {243, 4}, {244, 4}, {245, 4}, {246, 4}, {247, 4}, {248, 4}, {249, 4}, {250, 4}, {251, 4}, {252, 4}, {253, 4}, {254, 4}, {255, 4}, {285, 2}, {338, 4}, {339, 4}, {352, 4}, {353, 4}, {376, 4}, {381, 4}, {382, 4}, {3748, 2}, {5024, 2}, {8226, 2}, {8230, 4}, {8364, 4}, {65533, 4}};

//---------------------------------------------------------------
// This is the gamma lookup for mapping 255 brightness levels
// The lookup table would be similar but have slightly shifted
// numbers for different gammas (gamma 2.0, 2.2, 2.5, etc.)
const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


CRGB kelvinToRGB(int kelvin) {
  float temperature = kelvin / 100.0;
  float red, green, blue;

  if (temperature <= 66) {
    red = 255;
    green = temperature;
    green = 99.4708025861 * log(green) - 161.1195681661;
  } else {
    red = temperature - 60;
    red = 329.698727446 * pow(red, -0.1332047592);
    green = temperature - 60;
    green = 288.1221695283 * pow(green, -0.0755148492);
  }

  if (temperature >= 66) {
    blue = 255;
  } else if (temperature <= 19) {
    blue = 0;
  } else {
    blue = temperature - 10;
    blue = 138.5177312231 * log(blue) - 305.0447927307;
  }

  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  return CRGB((uint8_t)red, (uint8_t)green, (uint8_t)blue);
}

CRGB applyGammaCorrection(const CRGB& color) {
  CRGB correctedColor;
  correctedColor.r = pgm_read_byte(&gamma8[color.r]);
  correctedColor.g = pgm_read_byte(&gamma8[color.g]);
  correctedColor.b = pgm_read_byte(&gamma8[color.b]);
  return correctedColor;
}

uint32_t hsvToRgb(uint8_t h, uint8_t s, uint8_t v)
{
    CHSV hsv(h, s, v);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return ((uint16_t)(rgb.r & 0xF8) << 8) |
           ((uint16_t)(rgb.g & 0xFC) << 3) |
           (rgb.b >> 3);
}

uint16_t hexToRgb565(String hexValue, uint16_t defaultColor)
{
    hexValue.replace("#", "");
    uint8_t r = strtol(hexValue.substring(0, 2).c_str(), NULL, 16);
    uint8_t g = strtol(hexValue.substring(2, 4).c_str(), NULL, 16);
    uint8_t b = strtol(hexValue.substring(4, 6).c_str(), NULL, 16);
    if ((errno == ERANGE) || (r > 255) || (g > 255) || (b > 255))
    {
        return defaultColor;
    }
    uint16_t color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    return color;
}


uint16_t getColorFromJsonVariant(JsonVariant colorVariant, uint16_t defaultColor)
{
    if (colorVariant.is<String>())
    {
        return hexToRgb565(colorVariant.as<String>(), defaultColor);
    }
    else if (colorVariant.is<JsonArray>())
    {
        JsonArray colorArray = colorVariant.as<JsonArray>();
        if (colorArray.size() == 3) // RGB
        {
            uint8_t r = colorArray[0];
            uint8_t g = colorArray[1];
            uint8_t b = colorArray[2];
            return (r << 11) | (g << 5) | b;
        }
        else if (colorArray.size() == 4 && colorArray[0] == "HSV") // HSV
        {
            uint8_t h = colorArray[1];
            uint8_t s = colorArray[2];
            uint8_t v = colorArray[3];
            CRGB rgb;
            hsv2rgb_spectrum(CHSV(h, s, v), rgb);
            return (rgb.red << 11) | (rgb.green << 5) | rgb.blue;
        }
    }

    return defaultColor;
}


uint16_t getTextWidth(const char *text, byte textCase)
{
    uint16_t width = 0;
    for (const char *c = text; *c != '\0'; ++c)
    {
        char current_char = *c;
        if ((UPPERCASE_LETTERS && textCase == 0) || textCase == 1)
        {
            current_char = toupper(current_char);
        }
        if (CharMap.count(current_char) > 0)
        {
            width += CharMap[current_char];
        }
        else
        {
            width += 4;
        }
    }
    return width;
}

static byte c1;
byte utf8ascii(byte ascii)
{
    if (ascii < 128)
    {
        c1 = 0;
        return (ascii);
    }
    byte last = c1;
    c1 = ascii;
    switch (last)
    {
    case 0xC2:
        return (ascii)-34;
        break;
    case 0xC3:
        return (ascii | 0xC0) - 34;
        break;
    case 0x82:
        if (ascii == 0xAC)
            return (0xEA);
    }
    return (0);
}

String utf8ascii(String s)
{
    String r = "";
    char c;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        c = utf8ascii(s.charAt(i));
        if (c != 0)
            r += c;
    }
    return r;
}
#endif