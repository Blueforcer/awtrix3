#ifndef TimerEnums_h
#define TimerEnums_h

#include <Arduino.h>

// The Timer's two label-bearing enums and their per-enum CODEC TABLE: the fifth
// member of the descriptor-table family (alongside TIMER_SETTINGS_DESCS,
// TIMER_MEMBER_CONFIG_DESCS, TIMER_HA_DESCRIPTORS, TIMER_MENU_SLOTS).
//
// One row per enum value, INDEXED BY THE ENUM'S NUMERIC VALUE -- the enum value
// *is* the row index (the convention kBuzzerLabels/kFinishedLabels already used,
// now made authoritative). Each row carries every spelling of that value:
//   * wire    -- canonical MQTT/HTTP/sync string (the wire contract)
//   * menu    -- on-device TIMER-menu label
//   * ha      -- Home Assistant select-option label
//   * aliases -- extra accepted INPUT spellings (parse only), nullptr if none
//
// This file is a findable leaf module: it depends only on Arduino.h, so it can be
// reused by TimerManager, TimerMenu and TimerHa without forming a dependency cycle
// (TimerManager.h includes THIS header for the enums, never the reverse).
//
// B1 boundary is unchanged: this only consolidates how labels /
// strings are ENCODED. The MQTT raw-uint8_t-index publish path is untouched -- the
// published index is the enum value, which is exactly this table's row index.

// Timer lifecycle state. Lives here (not in TimerManager.h) so display-side value
// types like TimerSnapshot can carry it without depending on the manager singleton.
enum class TimerState : uint8_t { Idle = 0, Running = 1, Paused = 2, Finished = 3 };

enum class BuzzerMode   : uint8_t { Off = 0, End = 1, Countdown = 2, COUNT };
enum class FinishedMode : uint8_t { AutoClear = 0, Hold = 1, ReAlert = 2, COUNT };

struct TimerEnumCodec
{
    const char *wire;     // canonical wire string (MQTT/HTTP/sync)
    const char *menu;     // on-device BARE leaf value (e.g. "END", not "BZR END")
    const char *ha;       // Home Assistant select-option label
    const char *aliases;  // extra accepted input spelling(s), nullptr if none
};

extern const TimerEnumCodec TIMER_BUZZER_CODEC[];
extern const size_t         TIMER_BUZZER_CODEC_COUNT;
extern const TimerEnumCodec TIMER_FINISHED_CODEC[];
extern const size_t         TIMER_FINISHED_CODEC_COUNT;

// Row read by enum value (the value IS the row index). Bounds-clamped to row 0 so
// a stray cast can never index out of bounds.
const TimerEnumCodec &buzzerCodec(BuzzerMode m);
const TimerEnumCodec &finishedCodec(FinishedMode m);

// Case-insensitive scan: match `s` against a row's canonical wire spelling OR any
// of its aliases. On a hit, sets `outIndex` to the matching row (= enum value) and
// returns true; otherwise leaves `outIndex` untouched and returns false.
bool timerEnumParse(const TimerEnumCodec *table, size_t count,
                    const String &s, uint8_t &outIndex);

#endif
