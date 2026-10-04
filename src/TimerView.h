#ifndef TimerView_h
#define TimerView_h

#include <Arduino.h>

#include "TimerEnums.h"   // TimerState (carried by TimerSnapshot, no TimerManager dependency)

// The explicit, per-frame input to TimerViewModel::compute(): a plain-data
// snapshot of everything the view reads, captured once from TimerManager by the
// painter (src/Apps.cpp). It carries no methods and no TimerManager dependency,
// so compute() is a pure function of this value.
struct TimerSnapshot
{
    TimerState    state;        // lifecycle state (Idle/Running/Paused/Finished)
    uint32_t      duration;     // configured duration (seconds)
    uint32_t      remaining;    // live remaining (seconds); compute uses it only when !Idle
    uint32_t      runDuration;  // bar-denominator snapshot captured when the run began
    bool          iconEnabled;  // TIMER_ICON_ENABLED: gates the icon + full-panel reflow
    unsigned long nowMs;        // millis(); drives the 500 ms Finished blink only
};

// A pure, per-frame description of what the Timer app should draw. It holds no
// pixels and no font metrics: it is fully determined by TimerManager state, the
// current time (for the Finished blink), and the icon-enabled display flag (which
// gates the icon and, when off, reflows text+bar to the full panel). TimerApp
// (src/Apps.cpp) is its painter — it maps this struct to DisplayManager / matrix
// calls and owns the font-dependent text centering. Keeping the view display-free
// keeps the bar geometry, blink cadence and display-string selection
// independently checkable instead of hidden in the renderer.
struct TimerView
{
    enum class Screen : uint8_t { Config, Finished, Time };
    Screen screen;

    // Whether to draw the timer icon (false on Config, or when icon_enabled is off).
    // When false, text and bar reflow to span the full 32px panel.
    bool    showIcon;

    // Text to draw, centered by the painter within [textRegionX0, +textRegionW).
    char    text[12];
    bool    showText;        // false only during the Finished-blink "off" phase
    int16_t textRegionX0;    // app-local left edge of the centering region
    int16_t textRegionW;     // width of the centering region

    // Progress bar (Time screen, running/paused, duration > 0). Right-anchored,
    // drains from the left: invariant barStartX + barLen == panel width (32). Its
    // max length depends on showIcon (icon on -> 23px right of the icon; icon off
    // -> the full 32px panel).
    bool    showBar;
    uint8_t barLen;          // 0 .. (kBarMaxLen, or kScreenW when the icon is hidden)
    int16_t barStartX;       // app-local start column

    // Background track behind the bar: the FULL bar extent, painted in
    // bar_bg_color before the foreground segment. Unlike showBar it PERSISTS for the
    // whole Running/Paused window -- including the final stretch where barLen rounds
    // to 0 and showBar is false -- so the trough stays visible like the app's
    // drawProgressBar background. Same reflow as the bar: icon on -> (9, 23), icon
    // off -> (0, 32).
    bool    showBarTrack;
    uint8_t barTrackLen;     // kBarMaxLen, or kScreenW when the icon is hidden
    int16_t barTrackStartX;  // app-local start column (bar origin at full length)

    // Config-mode field underline (Config screen only).
    bool    showUnderline;
    uint8_t underlineField;  // 0 = HH, 1 = MM, 2 = SS (painter maps to X)
};

namespace TimerViewModel
{
    // Compute the view from an explicit snapshot of the timer state. Every value
    // compute() reads comes from `s` — it never touches the TimerManager singleton.
    // `s.nowMs` drives the 500 ms Finished blink only; `s.iconEnabled`
    // (TIMER_ICON_ENABLED) gates the icon and, when false, reflows text + bar to
    // the full panel.
    TimerView compute(const TimerSnapshot &s);

    // The compact on-screen format, fitted to the 24px text region:
    //   < 1h   -> "M:SS"   (305  -> "5:05")
    //   1..9h  -> "H:MM"   (seconds dropped to fit; 3661 -> "1:01")
    //   >= 10h -> "HH:MM"  (36000 -> "10:00")
    // Distinct from timerFormatHMS (the "H:MM:SS" wire string, which
    // always carries seconds).
    void formatTimerDisplay(uint32_t seconds, char *out, size_t outLen);
}

#endif
