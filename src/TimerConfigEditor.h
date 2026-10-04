#ifndef TimerConfigEditor_h
#define TimerConfigEditor_h

#include <Arduino.h>

// The on-device duration editor (HH/MM/SS wheels), a display-free value object. It
// owns the editing working state — the field cursor and the three edit buffers —
// the cap-aware adjust math, and the button hold-to-repeat. It has no DisplayManager
// dependency and no hardware reference: tick(nowMs, buttonState) receives the
// current time and injected button presses, so the repeat cadence is testable
// without a real clock or buttons. Duration flows in via enter() and back out via
// exit(); the caller commits the result through setDuration().
//
// There is no no-input auto-apply timeout (the TIMER menu — the editor's only
// host — is timeout-free); only hold-to-repeat remains.
class TimerConfigEditor
{
public:
    // Begin editing: decompose durationSec into HH/MM/SS, start on the HH field,
    // become active. HH is clamped to 99 so the field stays two-digit-editable
    // (the caller is responsible for any run-state clamp before handing it in).
    void enter(uint32_t durationSec);

    // Finish editing: become inactive and return the edited duration in seconds.
    // The caller commits it (TimerManager::setDuration).
    uint32_t exit();

    // Advance the highlighted field: HH -> MM -> SS -> HH.
    void cycleField();

    // Inc/dec the current field by one with wrapping. The wrap ceiling is the
    // per-field hard max (99 for HH, 59 for MM/SS) further capped by the headroom
    // left under the TIMER_MAX_DURATION global given the other two fields.
    void adjust(int delta);

    // Injected per-tick button state: raw "pressed" reads (PeripheryManager
    // buttonL/R isPressed()). The editor owns the long-press threshold and repeat
    // cadence, deriving held time from the nowMs handed to tick().
    // An explicit two-arg constructor (alongside a defaulted default ctor) keeps the
    // member defaults but makes construction standard-independent: a class with default
    // member initializers is not an aggregate under C++11, so the call site's two-arg
    // brace-init (TimerManager.cpp) would otherwise need the C++14 relaxed-aggregate
    // rule and break the gnu++11 device build.
    struct ButtonState
    {
        bool leftPressed  = false;
        bool rightPressed = false;
        ButtonState() = default;
        ButtonState(bool l, bool r) : leftPressed(l), rightPressed(r) {}
    };

    // Drive button hold-to-repeat: 500 ms long-press threshold then 250 ms cadence
    // (left = -1 / right = +1). No timeout — the editor never auto-applies.
    void tick(unsigned long nowMs, ButtonState buttons);

    bool    isActive() const { return active_; }
    uint8_t field()    const { return field_; }
    uint8_t hh()       const { return hh_; }
    uint8_t mm()       const { return mm_; }
    uint8_t ss()       const { return ss_; }

private:
    // Drive one button's hold-to-repeat for the current field. pressStartMs/repeatMs
    // are that button's bookkeeping (0 = unpressed / no step yet); delta is the
    // field step (-1 left, +1 right).
    void repeatHeld(bool pressed, unsigned long nowMs,
                    unsigned long &pressStartMs, unsigned long &repeatMs, int delta);

    bool    active_ = false;
    uint8_t field_  = 0;   // 0 = HH, 1 = MM, 2 = SS
    uint8_t hh_     = 0;
    uint8_t mm_     = 0;
    uint8_t ss_     = 0;

    // Per-button hold-to-repeat bookkeeping. 0 = unpressed / no repeat yet.
    unsigned long leftPressStartMs_  = 0;
    unsigned long rightPressStartMs_ = 0;
    unsigned long leftRepeatMs_      = 0;
    unsigned long rightRepeatMs_     = 0;
};

#endif
