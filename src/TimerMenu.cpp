#ifndef AWTRIX_DISABLE_TIMER
#include "TimerMenu.h"

#include "TimerManager.h"
#include "TimerSettings.h"

namespace
{
    // Enum hooks. The setters defer the NVS write (persist=false): the TIMER menu
    // applies + publishes live during scroll and persists on the long-press
    // commit, where MenuManager opens the PersistBatch guard. Non-capturing
    // lambdas decay to the row's function pointers.
    uint8_t getBuzzer()        { return (uint8_t)TimerManager.getBuzzerMode(); }
    void    setBuzzer(uint8_t v) { TimerManager.setBuzzerMode((BuzzerMode)v, /*persist=*/false); }
    uint8_t getFinished()      { return (uint8_t)TimerManager.getFinishedMode(); }
    void    setFinished(uint8_t v) { TimerManager.setFinishedMode((FinishedMode)v, /*persist=*/false); }
}

// idx order is the on-screen list order (left/right walks it, wrapping). DURATION
// is first (the HH:MM:SS wheel, delegating to TimerConfigEditor); MAIN is the lone
// Navigation row and sits last (the device reads it as the back-to-main item).
//   kind, name, cmdKey, step, codec, labelCount, getEnum, setEnum
// The two EnumCycle slots read their bare leaf value from the per-enum codec
// table's menu column -- no private label copy to drift from it.
const TimerMenuSlot TIMER_MENU_SLOTS[] = {
    {TimerMenuKind::Duration,     "DURATION",  nullptr,             0, nullptr,              0, nullptr,     nullptr},
    {TimerMenuKind::EnumCycle,    "BUZZER",    nullptr,             0, TIMER_BUZZER_CODEC,   (uint8_t)BuzzerMode::COUNT,   getBuzzer,   setBuzzer},
    {TimerMenuKind::SteppedRange, "COUNTDOWN", "countdown_seconds", 1, nullptr,              0, nullptr,     nullptr},
    {TimerMenuKind::EnumCycle,    "FINISH",    nullptr,             0, TIMER_FINISHED_CODEC, (uint8_t)FinishedMode::COUNT, getFinished, setFinished},
    {TimerMenuKind::SteppedRange, "CLEAR DELAY",       "finished_hold",    5, nullptr,              0, nullptr,     nullptr},
    {TimerMenuKind::SteppedRange, "RE-ALERT INTERVAL", "realert_interval", 5, nullptr,              0, nullptr,     nullptr},
    {TimerMenuKind::BoolToggle,   "ICON",              "icon_enabled",     0, nullptr,              0, nullptr,     nullptr},
    {TimerMenuKind::BoolToggle,   "PROGRESS BAR",      "bar_enabled",      0, nullptr,              0, nullptr,     nullptr},
    {TimerMenuKind::Navigation,   "MAIN",      nullptr,             0, nullptr,              0, nullptr,     nullptr},
};

const size_t TIMER_MENU_SLOT_COUNT = sizeof(TIMER_MENU_SLOTS) / sizeof(TIMER_MENU_SLOTS[0]);

String timerMenuName(uint8_t slot)
{
    if (slot >= TIMER_MENU_SLOT_COUNT) return String();
    const char *n = TIMER_MENU_SLOTS[slot].name;
    return n ? String(n) : String();
}

String timerMenuValue(uint8_t slot)
{
    if (slot >= TIMER_MENU_SLOT_COUNT) return String();
    const TimerMenuSlot &s = TIMER_MENU_SLOTS[slot];
    switch (s.kind)
    {
        case TimerMenuKind::Duration:
        {
            // The committed duration as a zero-padded HH:MM:SS clock (matches the
            // wheel), independent of any live edit in the leaf engine.
            return timerClock(TimerManager.getDuration(), ClockStyle::Padded);
        }
        case TimerMenuKind::EnumCycle:
            return String(s.codec[s.getEnum()].menu);
        case TimerMenuKind::SteppedRange:
        {
            const TimerSettingDesc *d = timerSettingByCmdKey(s.cmdKey);
            uint16_t v = d ? *static_cast<uint16_t *>(d->storage) : 0;
            return String(v);
        }
        case TimerMenuKind::BoolToggle:
        {
            const TimerSettingDesc *d = timerSettingByCmdKey(s.cmdKey);
            bool on = d && *static_cast<bool *>(d->storage);
            return String(on ? "ON" : "OFF");
        }
        case TimerMenuKind::Navigation:
            return String();
    }
    return String();
}

void timerMenuAdjust(uint8_t slot, int dir)
{
    if (slot >= TIMER_MENU_SLOT_COUNT) return;
    const TimerMenuSlot &s = TIMER_MENU_SLOTS[slot];
    switch (s.kind)
    {
        case TimerMenuKind::Duration:
            break;  // the duration wheel is driven by the edit engine, not adjust
        case TimerMenuKind::EnumCycle:
        {
            uint8_t cur  = s.getEnum();
            uint8_t next = (dir > 0)
                ? (uint8_t)((cur + 1) % s.labelCount)
                : (uint8_t)((cur + s.labelCount - 1) % s.labelCount);
            s.setEnum(next);
            break;
        }
        case TimerMenuKind::SteppedRange:
        {
            const TimerSettingDesc *d = timerSettingByCmdKey(s.cmdKey);
            if (!d) break;
            uint16_t &v = *static_cast<uint16_t *>(d->storage);
            if (dir > 0) v = (v + s.step <= d->hi) ? (uint16_t)(v + s.step) : (uint16_t)d->hi;
            else         v = (v >= d->lo + s.step) ? (uint16_t)(v - s.step) : (uint16_t)d->lo;
            break;
        }
        case TimerMenuKind::BoolToggle:
        {
            const TimerSettingDesc *d = timerSettingByCmdKey(s.cmdKey);
            if (!d) break;
            bool &b = *static_cast<bool *>(d->storage);
            b = !b;
            break;
        }
        case TimerMenuKind::Navigation:
            break;  // no value to adjust
    }
}

TimerNavLeaf timerMenuLeafKind(uint8_t slot, TimerState st)
{
    if (slot >= TIMER_MENU_SLOT_COUNT) return TimerNavLeaf::Value;
    if (TIMER_MENU_SLOTS[slot].kind != TimerMenuKind::Duration)
        return TimerNavLeaf::Value;
    // Duration: editable HH:MM:SS wheel only while Idle; read-only otherwise.
    return (st == TimerState::Idle)
               ? TimerNavLeaf::DurationEditable
               : TimerNavLeaf::DurationReadOnly;
}
#endif // AWTRIX_DISABLE_TIMER
