#ifndef LAYOUT_H
#define LAYOUT_H

#include <Arduino.h>
#include "Globals.h"

// Where custom apps and notifications put their icon, text and progress bar.
//
// 32x8:  icon on the left, text to its right on the same row (the classic AWTRIX layout).
// 16x16: icon centred in the top half, text on the bottom half; without an icon the text
//        is centred vertically.
struct ContentLayout
{
    int16_t iconX, iconY;   // top-left corner of the 8x8 icon
    int16_t textLeft;       // first column of the text area
    int16_t textWidth;      // columns available for the text
    int16_t textY;          // text baseline
    int16_t barX, barY;     // progress bar start column and row
    bool iconBesideText;    // icon and text share a row (push-icon animation, separator line)
};

inline ContentLayout contentLayout(bool hasIcon)
{
#ifdef AWTRIX_16X16
    if (hasIcon)
        return {4, 0, 0, 16, 14, 0, 15, false};
    return {0, 0, 0, 16, 10, 0, 15, false};
#else
    if (hasIcon)
        return {0, 0, 9, 24, 6, 8, 7, true};
    return {0, 0, 0, 32, 6, 0, 7, true};
#endif
}

// Baseline for a single line of text centred vertically on the canvas (6 on 32x8, 10 on 16x16)
#define TEXT_Y_CENTER (MATRIX_H / 2 + 2)

// Column where scrolling text restarts after it has scrolled out
#ifdef AWTRIX_16X16
#define SCROLL_RESTART_X 0
#else
#define SCROLL_RESTART_X 9
#endif

#endif // LAYOUT_H
