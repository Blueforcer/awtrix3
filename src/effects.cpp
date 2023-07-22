#include "effects.h"

void Pacifica(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    uint8_t speed = 2;
    static uint32_t sPacificaTime = 0;
    sPacificaTime += speed;
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint16_t ulx = (sPacificaTime / 8) + (i * 16);
            uint16_t uly = (sPacificaTime / 4) + (j * 16);
            uint16_t v = 0;
            v += sin16(ulx * 6 + sPacificaTime / 2) / 8 + 127;
            v += sin16(uly * 9 + sPacificaTime / 2) / 8 + 127;
            v += sin16(ulx * 7 + uly * 2 - sPacificaTime) / 16;
            v = v / 3;
            CRGB color = ColorFromPalette(OceanColors_p, v);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void TheaterChase(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint16_t j = 0;
    static uint16_t callCount = 0;
    callCount++;

    if (callCount % 3 == 0)
    {
        j = (j + 1) % 3;
    }

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t k = 0; k < rows; k++)
        {
            if ((i + j) % 3 == 0)
            {
                uint8_t colorIndex = (i * 256 / cols) & 255;
                CRGB color = ColorFromPalette(RainbowColors_p, colorIndex);
                matrix->drawPixel(x + i, y + k, color);
            }
            else
            {
                matrix->drawPixel(x + i, y + k, matrix->Color(0, 0, 0));
            }
        }
    }
}

void Plasma(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
 const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t time = 0;

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint8_t value = sin8(i*10 + time) + sin8(j*10 + time/2) + sin8((i+j)*10 + time/3) / 3;
            CRGB color = ColorFromPalette(RainbowColors_p, value);
            matrix->drawPixel(x + i, y + j, color);
        }
    }

    // Increment time to create the animation
    time += 2;
}


Effect effects[] = {
    {"Pacifica", Pacifica},
    {"TheaterChase", TheaterChase},
    {"Plasma", Plasma}};

void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, String name)
{
    for (uint8_t i = 0; i < sizeof(effects) / sizeof(effects[0]); i++)
    {
        if (effects[i].name == name)
        {
            effects[i].func(matrix, x, y);
            break;
        }
    }
}