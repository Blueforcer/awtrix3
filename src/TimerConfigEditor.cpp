#ifndef AWTRIX_DISABLE_TIMER
#include "TimerConfigEditor.h"

#include "Globals.h"        // TIMER_MAX_DURATION (cap math)
#include "TimerSettings.h"  // timerSecondsToHMS / timerHmsToSeconds (pure math free functions)

namespace {
    // Hold-to-repeat timing (was TimerManager.cpp's anon namespace).
    constexpr unsigned long kBtnLongPressMs = 500;   // hold this long before auto-repeat begins
    constexpr unsigned long kBtnRepeatMs    = 250;   // cadence between auto-repeat steps
}

void TimerConfigEditor::enter(uint32_t durationSec)
{
    uint32_t h, m, s;
    timerSecondsToHMS(durationSec, h, m, s);
    if (h > 99) h = 99;
    hh_    = (uint8_t)h;
    mm_    = (uint8_t)m;
    ss_    = (uint8_t)s;
    field_ = 0;
    active_ = true;
}

uint32_t TimerConfigEditor::exit()
{
    active_ = false;
    return timerHmsToSeconds(hh_, mm_, ss_);
}

void TimerConfigEditor::cycleField()
{
    if (!active_) return;
    field_ = (field_ + 1) % 3;
}

void TimerConfigEditor::adjust(int delta)
{
    if (!active_) return;

    const uint8_t  hardMax = (field_ == 0) ? 99 : 59;
    const uint32_t perUnit = (field_ == 0) ? 3600UL : (field_ == 1) ? 60UL : 1UL;
    const uint32_t otherSec = (field_ == 0)
        ? (uint32_t)mm_ * 60UL + (uint32_t)ss_
        : (field_ == 1)
            ? (uint32_t)hh_ * 3600UL + (uint32_t)ss_
            : (uint32_t)hh_ * 3600UL + (uint32_t)mm_ * 60UL;

    uint8_t maxVal = hardMax;
    if (TIMER_MAX_DURATION > 0)
    {
        uint32_t headroom = (TIMER_MAX_DURATION > otherSec) ? (TIMER_MAX_DURATION - otherSec) : 0;
        uint32_t room     = headroom / perUnit;
        if (room < maxVal) maxVal = (uint8_t)room;
    }

    uint8_t cur = (field_ == 0) ? hh_ : (field_ == 1 ? mm_ : ss_);
    if (cur > maxVal) cur = maxVal;
    int next = (int)cur + (delta >= 0 ? 1 : -1);
    if (next < 0) next = maxVal;
    else if (next > (int)maxVal) next = 0;
    if      (field_ == 0) hh_ = (uint8_t)next;
    else if (field_ == 1) mm_ = (uint8_t)next;
    else                  ss_ = (uint8_t)next;
}

void TimerConfigEditor::tick(unsigned long nowMs, ButtonState buttons)
{
    if (!active_) return;

    // Hold-to-repeat: derive held time from nowMs vs a per-button press-start.
    // While held past the long-press threshold, step once immediately, then once
    // per repeat-cadence window. The editor never auto-applies on idle.
    repeatHeld(buttons.leftPressed,  nowMs, leftPressStartMs_,  leftRepeatMs_,  -1);
    repeatHeld(buttons.rightPressed, nowMs, rightPressStartMs_, rightRepeatMs_, +1);
}

void TimerConfigEditor::repeatHeld(bool pressed, unsigned long nowMs,
                                   unsigned long &pressStartMs, unsigned long &repeatMs,
                                   int delta)
{
    if (!pressed)
    {
        pressStartMs = 0;
        repeatMs     = 0;
        return;
    }

    if (pressStartMs == 0) pressStartMs = nowMs;   // rising edge: start the hold clock
    if (nowMs - pressStartMs < kBtnLongPressMs) return;

    if (repeatMs == 0 || (nowMs - repeatMs) >= kBtnRepeatMs)
    {
        adjust(delta);
        repeatMs = nowMs;
    }
}
#endif // AWTRIX_DISABLE_TIMER
