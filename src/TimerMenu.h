#ifndef TimerMenu_h
#define TimerMenu_h

#include <Arduino.h>

#include "TimerEnums.h"
#include "TimerMenuNav.h"   // TimerNavLeaf (the leaf-kind the device hands the nav SM)

// TIMER menu slot table: the data model behind the on-device TIMER global menu's
// drill-in list (the third member of the Timer descriptor-table family, alongside
// TIMER_SETTINGS_DESCS and TIMER_HA_DESCRIPTORS). One row per list item; the
// TimerMenuNav state machine walks it and MenuManager keeps only the drawing +
// commit. This header is display-free (no DisplayManager) so the name/value/adjust
// logic is testable in isolation.
//
// Two slot families, mirroring the B1 boundary:
//   * table-backed slots (SteppedRange / BoolToggle) reuse their TIMER_SETTINGS_DESCS
//     row by cmdKey for storage + range -- they cannot drift from the settings table.
//   * the two enum slots (buzzer / finished) are member-backed: they carry bespoke
//     getEnum/setEnum hooks (like the settings table's `bespoke` fn pointers). Their
//     setEnum defers the NVS write to the menu commit (setBuzzerMode(m, persist=false)).

// Slot kinds. The value kinds (EnumCycle / SteppedRange / BoolToggle) drill into a
// leaf editor; Duration drills into the HH:MM:SS wheel (delegating to the existing
// TimerConfigEditor edit engine, no settings-storage row); Navigation is the lone
// non-value row (MAIN) that walks the device back to the main menu.
enum class TimerMenuKind : uint8_t { Duration, EnumCycle, SteppedRange, BoolToggle, Navigation };

struct TimerMenuSlot
{
    TimerMenuKind      kind;
    const char        *name;         // list label (the item name): "BUZZER", "COUNTDOWN", "MAIN"
    const char        *cmdKey;       // SteppedRange/BoolToggle: -> TIMER_SETTINGS_DESCS (storage + lo/hi)
    uint16_t           step;         // SteppedRange step
    const TimerEnumCodec *codec;     // EnumCycle: codec table; .menu column is the bare leaf value
    uint8_t            labelCount;   // EnumCycle modulus
    uint8_t          (*getEnum)();   // EnumCycle only
    void             (*setEnum)(uint8_t);  // EnumCycle only (routes via TimerManager setter)
};

extern const TimerMenuSlot TIMER_MENU_SLOTS[];
extern const size_t        TIMER_MENU_SLOT_COUNT;

// The slot's LIST LABEL (the item name shown while walking the list), e.g.
// "BUZZER", "COUNTDOWN", "MAIN". Returns "" for an out-of-range slot index.
String timerMenuName(uint8_t slot);

// The slot's BARE LEAF VALUE (shown while editing the leaf), e.g. "END", "10",
// "ON". The item name already gives the context, so no prefix. The Duration row
// returns the current duration as a zero-padded "HH:MM:SS" clock string;
// Navigation rows (MAIN) have no value and return "". Returns "" for an
// out-of-range index.
String timerMenuValue(uint8_t slot);

// How the leaf for `slot` behaves, given the timer's current state `st`. The
// Duration row is an editable HH:MM:SS wheel only when the timer is Idle; while
// Running/Paused it is read-only. Every other row is a
// plain Value leaf. The device hands the result to TimerMenuNav, so the Idle-only
// gating decision lives in display-free code.
TimerNavLeaf timerMenuLeafKind(uint8_t slot, TimerState st);

// Adjust the slot by one step in the given direction (dir > 0 = right/increment,
// dir <= 0 = left/decrement). Stepped ranges saturate at the descriptor's lo/hi;
// enums wrap; bools toggle (either direction). No-op for a Navigation row or an
// out-of-range index.
void timerMenuAdjust(uint8_t slot, int dir);

#endif
