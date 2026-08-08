#ifndef AWTRIX_DISABLE_TIMER
#include "TimerSettings.h"

#include <string.h>
#include <Preferences.h>

#include "TimerManager.h"   // TimerManager singleton: the member-config setters/getters
#include "MQTTManager.h"    // the (topic, payload) wire seam the publish hooks emit on
#include "TimerHa.h"        // TimerHaEntity slots for the hooks' canonical topics

// The IMPURE half of the descriptor-table family. Everything here touches the
// TimerManager singleton, the MQTT wire seam, or Preferences -- so it is split out
// of the pure TimerSettings.cpp, which stays dependency-light. The PURE half (the
// descriptor tables, parsers, member VALIDATORS) lives in TimerSettings.cpp; this file
// carries the member apply/emit/publish hooks, the full TIMER_MEMBER_CONFIG_DESCS
// table, the NVS round-trip, and the HTTP full-config dump.

// ===========================================================================
// NVS round-trip for the whole settings table (caller brackets begin()/end()).
// ===========================================================================
void timerSettingsLoadNvs(Preferences &prefs)
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        switch (d.type)
        {
            case TcType::U16:  *static_cast<uint16_t *>(d.storage) = (uint16_t)prefs.getUInt(d.nvsKey, d.dfltNum); break;
            case TcType::U32:  *static_cast<uint32_t *>(d.storage) = prefs.getUInt(d.nvsKey, d.dfltNum);           break;
            case TcType::Bool: *static_cast<bool *>    (d.storage) = prefs.getBool(d.nvsKey, d.dfltNum != 0);      break;
            case TcType::Str:  *static_cast<String *>  (d.storage) = prefs.getString(d.nvsKey, d.dfltStr);         break;
        }
    }
}

void timerSettingsSaveNvs(Preferences &prefs)
{
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        switch (d.type)
        {
            case TcType::U16:  prefs.putUInt(d.nvsKey, *static_cast<uint16_t *>(d.storage)); break;
            case TcType::U32:  prefs.putUInt(d.nvsKey, *static_cast<uint32_t *>(d.storage)); break;
            case TcType::Bool: prefs.putBool(d.nvsKey, *static_cast<bool *>    (d.storage)); break;
            case TcType::Str:  prefs.putString(d.nvsKey, *static_cast<String *>(d.storage)); break;
        }
    }
}

// ===========================================================================
// Member-backed config half (B1) -- the impure apply/emit/publish hooks + the full
// table. Each hook routes through TimerManager's existing public setters/getters; the
// enum rows stage the enum cast in TcValue::num, the icon rows stage the name in
// TcValue::str. The validate column reuses the SAME pure predicates the validator
// table uses (timerMemValidate*, declared in the header) so the two cannot drift.
// ===========================================================================
namespace
{
    // -- buzzer --
    void memApplyBuzzer(const TcValue &v) { TimerManager.setBuzzerMode((BuzzerMode)v.num); }
    void memEmitBuzzer (JsonDocument &doc) { doc["buzzer"] = TimerManager.buzzerModeString(); }
    // Publish hook: the live value onto the wire seam, payload the
    // per-enum codec's canonical `wire` string -- never the numeric index, and
    // deliberately not the HASelect `ha` label the retired setState path sent.
    void memPublishBuzzer()
    {
        MQTTManager.publishTimerWire(MQTTManager.timerWireTopic(TimerHaEntity::Buzzer).c_str(),
                                     TimerManager.buzzerModeString());
    }

    // -- finished --
    void memApplyFinished(const TcValue &v) { TimerManager.setFinishedMode((FinishedMode)v.num); }
    void memEmitFinished (JsonDocument &doc) { doc["finished"] = TimerManager.finishedModeString(); }
    // Publish hook: see memPublishBuzzer.
    void memPublishFinished()
    {
        MQTTManager.publishTimerWire(MQTTManager.timerWireTopic(TimerHaEntity::Finished).c_str(),
                                     TimerManager.finishedModeString());
    }

    // -- icon_<state> (shared validate + shared aggregate publish; the per-slot
    // apply/emit fold into these two TimerState-indexed templates over iconByState[]).
    // A fifth timer state wires its apply + emit for free from its row's <State> tag
    // instead of needing a hand-written pair that could be forgotten. The
    // snapshot/command key per state, ordered by TimerState (== each row's cmdKey).
    static const char *const kIconCmdKeys[kTimerStateCount] = {
        "icon_idle", "icon_running", "icon_paused", "icon_finished"};
    template <TimerState S>
    void memApplyIcon(const TcValue &v) { TimerManager.setIcon(S, v.str); }
    template <TimerState S>
    void memEmitIcon (JsonDocument &doc) { doc[kIconCmdKeys[(size_t)S]] = TimerManager.getIcon(S); }
    // Publish hook, shared by all four icon rows: the icon keys have
    // ONE wire artifact — the aggregate four-state JSON on the plain
    // {MQTT_PREFIX}/timer/icons topic (not an HA entity data topic), payload
    // byte-identical to the retired MQTTManager::publishTimerIcons composer.
    void memPublishIcons()
    {
        DynamicJsonDocument doc(256);
        doc["idle"]     = TimerManager.getIconIdle();
        doc["running"]  = TimerManager.getIconRunning();
        doc["paused"]   = TimerManager.getIconPaused();
        doc["finished"] = TimerManager.getIconFinished();
        String payload;
        serializeJson(doc, payload);
        MQTTManager.publishTimerWire(MQTTManager.timerIconsTopic().c_str(), payload.c_str());
    }
}

const TimerMemberConfigDesc TIMER_MEMBER_CONFIG_DESCS[] = {
    {"buzzer",        timerMemValidateBuzzer,   memApplyBuzzer,        memEmitBuzzer,        memPublishBuzzer},
    {"finished",      timerMemValidateFinished, memApplyFinished,      memEmitFinished,      memPublishFinished},
    {"icon_idle",     timerMemValidateIcon,     memApplyIcon<TimerState::Idle>,     memEmitIcon<TimerState::Idle>,     memPublishIcons},
    {"icon_running",  timerMemValidateIcon,     memApplyIcon<TimerState::Running>,  memEmitIcon<TimerState::Running>,  memPublishIcons},
    {"icon_paused",   timerMemValidateIcon,     memApplyIcon<TimerState::Paused>,   memEmitIcon<TimerState::Paused>,   memPublishIcons},
    {"icon_finished", timerMemValidateIcon,     memApplyIcon<TimerState::Finished>, memEmitIcon<TimerState::Finished>, memPublishIcons},
};

const size_t TIMER_MEMBER_CONFIG_DESC_COUNT =
    sizeof(TIMER_MEMBER_CONFIG_DESCS) / sizeof(TIMER_MEMBER_CONFIG_DESCS[0]);

void timerMemberConfigBuildSnapshot(JsonDocument &doc)
{
    for (size_t i = 0; i < TIMER_MEMBER_CONFIG_DESC_COUNT; ++i)
        TIMER_MEMBER_CONFIG_DESCS[i].emit(doc);
}

void timerMemberConfigPublish(const char *cmdKey)
{
    for (size_t i = 0; i < TIMER_MEMBER_CONFIG_DESC_COUNT; ++i)
    {
        const TimerMemberConfigDesc &d = TIMER_MEMBER_CONFIG_DESCS[i];
        if (strcmp(d.cmdKey, cmdKey) != 0) continue;
        if (d.publish) d.publish();
        return;
    }
}

// ===========================================================================
// HTTP GET /api/timer config mirror. The complete persisted-config
// projection: both tables, no snapshot filter, raw values. Lives beside the impure
// member half it dumps. See the header for the full contract.
// ===========================================================================
void timerBuildFullConfig(JsonDocument &doc)
{
    // Table half: EVERY settings row, ignoring inSnapshot, so the sync-role keys
    // (sync_follow/sync_targets) are part of the read mirror even though they are
    // never propagated. Raw value per row via the shared single-row emitter, with
    // the two deliberate carrier-native overrides reusing the family's
    // shared bar formatters -- diverging from the raw propagation snapshot by design,
    // yet never able to disagree in value (same storage).
    for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
    {
        const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
        if (strcmp(d.cmdKey, "bar_color") == 0)
        {
            timerFormatBarColor(d, doc);     // "default" / uppercase "#RRGGBB", not the raw int
        }
        else if (strcmp(d.cmdKey, "bar_bg_color") == 0)
        {
            timerFormatBarBgColor(d, doc);   // "none" / uppercase "#RRGGBB", not the raw int
        }
        else if (strcmp(d.cmdKey, "max_duration") == 0)
        {
            timerSettingEmitValue(d, doc);   // raw seconds kept (e.g. 86400)
            // ...plus a trimmed clock-string sibling, this endpoint's raw+_str
            // duration precedent, reusing the exact timerFormatHMS the duration fields use.
            doc["max_duration_str"] = timerFormatHMS(*static_cast<uint32_t *>(d.storage));
        }
        else
        {
            timerSettingEmitValue(d, doc);
        }
    }

    // Member-backed half: buzzer/finished + the four icon_* live values.
    timerMemberConfigBuildSnapshot(doc);
}
#endif // AWTRIX_DISABLE_TIMER
