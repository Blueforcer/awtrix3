#ifndef AWTRIX_DISABLE_TIMER
#include "TimerEnums.h"

// One row per enum value, in enum-value order so the array index IS the enum
// value. Wire spellings are the canonical MQTT/HTTP/sync contract; menu/ha are the
// on-device and Home Assistant labels; aliases are extra accepted input spellings
// (parse only -- never emitted).
const TimerEnumCodec TIMER_BUZZER_CODEC[] = {
    {"off",       "OFF", "Off",       nullptr},
    {"end",       "END", "End",       nullptr},
    {"countdown", "CDN", "Countdown", nullptr},
};
const size_t TIMER_BUZZER_CODEC_COUNT =
    sizeof(TIMER_BUZZER_CODEC) / sizeof(TIMER_BUZZER_CODEC[0]);

const TimerEnumCodec TIMER_FINISHED_CODEC[] = {
    {"auto-clear", "CLEAR",    "Auto-clear", "autoclear"},
    {"hold",       "HOLD",     "Hold",       nullptr},
    {"re-alert",   "RE-ALERT", "Re-alert",   "realert"},
};
const size_t TIMER_FINISHED_CODEC_COUNT =
    sizeof(TIMER_FINISHED_CODEC) / sizeof(TIMER_FINISHED_CODEC[0]);

// Adding an enum value without giving it a table row (or vice versa) fails the
// build here -- the table count is pinned to the enum's COUNT sentinel.
static_assert(sizeof(TIMER_BUZZER_CODEC) / sizeof(TIMER_BUZZER_CODEC[0]) ==
                  static_cast<size_t>(BuzzerMode::COUNT),
              "TIMER_BUZZER_CODEC must have one row per BuzzerMode value");
static_assert(sizeof(TIMER_FINISHED_CODEC) / sizeof(TIMER_FINISHED_CODEC[0]) ==
                  static_cast<size_t>(FinishedMode::COUNT),
              "TIMER_FINISHED_CODEC must have one row per FinishedMode value");

const TimerEnumCodec &buzzerCodec(BuzzerMode m)
{
    uint8_t i = static_cast<uint8_t>(m);
    if (i >= TIMER_BUZZER_CODEC_COUNT) i = 0;
    return TIMER_BUZZER_CODEC[i];
}

const TimerEnumCodec &finishedCodec(FinishedMode m)
{
    uint8_t i = static_cast<uint8_t>(m);
    if (i >= TIMER_FINISHED_CODEC_COUNT) i = 0;
    return TIMER_FINISHED_CODEC[i];
}

bool timerEnumParse(const TimerEnumCodec *table, size_t count,
                    const String &s, uint8_t &outIndex)
{
    String in = s;
    in.toLowerCase();
    for (size_t i = 0; i < count; ++i)
    {
        if (in == table[i].wire) { outIndex = (uint8_t)i; return true; }
        // aliases is a single extra spelling today; compare case-insensitively too.
        if (table[i].aliases && in == table[i].aliases) { outIndex = (uint8_t)i; return true; }
    }
    return false;
}
#endif // AWTRIX_DISABLE_TIMER
