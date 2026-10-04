#ifndef AWTRIX_DISABLE_TIMER
#include "TimerHa.h"

#include <stdio.h>
#include <string.h>

#include "TimerEnums.h"

// PROGMEM is a no-op for const data on the ESP32 (flash is memory-mapped and
// directly addressable), and isn't defined on the native host build. Define it
// away when absent so this table compiles in both environments.
#ifndef PROGMEM
#define PROGMEM
#endif

// Build a select's "`;`"-joined HA option list from a per-enum codec table's HA
// column, in enum-value order (the table's row index IS the enum value, pinned by
// its static_assert —). One join site, no hand-written list to
// drift from the enum labels. The returned String has static lifetime below, so
// the descriptor's `options` pointer stays valid for the program's life.
static String joinHaOptions(const TimerEnumCodec *table, size_t count)
{
    String s;
    for (size_t i = 0; i < count; ++i)
    {
        if (i) s += ';';
        s += table[i].ha;
    }
    return s;
}

// HA entity strings for the Timer. Co-located with the descriptor table so the
// Timer's HA contract lives in one place (moved here from Dictionary.cpp).
static const char HAtimerDurID[] PROGMEM    = {"%s_timer_dur"};
static const char HAtimerDurIcon[] PROGMEM  = {"mdi:timer-cog-outline"};
static const char HAtimerDurName[] PROGMEM  = {"Timer duration"};

static const char HAtimerRemID[] PROGMEM    = {"%s_timer_rem"};
static const char HAtimerRemIcon[] PROGMEM  = {"mdi:timer-sand"};
static const char HAtimerRemName[] PROGMEM  = {"Timer remaining"};
static const char HAtimerRemUnit[] PROGMEM  = {"s"};
static const char HAtimerRemClass[] PROGMEM = {"duration"};

static const char HAtimerStateID[] PROGMEM   = {"%s_timer_state"};
static const char HAtimerStateIcon[] PROGMEM = {"mdi:state-machine"};
static const char HAtimerStateName[] PROGMEM = {"Timer state"};

static const char HAtimerBuzID[] PROGMEM      = {"%s_timer_buz"};
static const char HAtimerBuzIcon[] PROGMEM    = {"mdi:volume-high"};
static const char HAtimerBuzName[] PROGMEM    = {"Timer buzzer"};
static const String HAtimerBuzOptions = joinHaOptions(TIMER_BUZZER_CODEC, TIMER_BUZZER_CODEC_COUNT);

static const char HAtimerFinID[] PROGMEM      = {"%s_timer_fin"};
static const char HAtimerFinIcon[] PROGMEM    = {"mdi:bell-ring-outline"};
static const char HAtimerFinName[] PROGMEM    = {"Timer finished mode"};
static const String HAtimerFinOptions = joinHaOptions(TIMER_FINISHED_CODEC, TIMER_FINISHED_CODEC_COUNT);

static const char HAtimerStartID[] PROGMEM   = {"%s_timer_start"};
static const char HAtimerStartIcon[] PROGMEM = {"mdi:play"};
static const char HAtimerStartName[] PROGMEM = {"Timer start"};

static const char HAtimerPauseID[] PROGMEM   = {"%s_timer_pause"};
static const char HAtimerPauseIcon[] PROGMEM = {"mdi:pause"};
static const char HAtimerPauseName[] PROGMEM = {"Timer pause"};

static const char HAtimerResetID[] PROGMEM   = {"%s_timer_reset"};
static const char HAtimerResetIcon[] PROGMEM = {"mdi:restore"};
static const char HAtimerResetName[] PROGMEM = {"Timer reset"};

// Sync-control entities: the two sync settings — until now read-only
// HA attributes on the state sensor — become writable. The Follow switch carries
// the receive-consent toggle; the Targets select ships STATIC Off/All only (it
// becomes dynamic in a later peer-discovery slice).
static const char HAtimerSyncFollowID[] PROGMEM   = {"%s_timer_sync_follow"};
static const char HAtimerSyncFollowIcon[] PROGMEM = {"mdi:account-sync"};
static const char HAtimerSyncFollowName[] PROGMEM = {"Timer sync follow"};

static const char HAtimerSyncTargetsID[] PROGMEM   = {"%s_timer_sync_targets"};
static const char HAtimerSyncTargetsIcon[] PROGMEM = {"mdi:target-account"};
static const char HAtimerSyncTargetsName[] PROGMEM = {"Timer sync targets"};
// Static option list, in TimerSyncTargetsOption order (Off=0, All=1). The select's
// state index reflects sync_targets via timerSyncTargetsIndexForValue.
static const char HAtimerSyncTargetsOptions[] PROGMEM = {"Off;All"};

const TimerHaDescriptor TIMER_HA_DESCRIPTORS[TIMER_HA_DESCRIPTOR_COUNT] = {
    {TimerHaEntity::Duration, "text",   HAtimerDurID,   HAtimerDurIcon,   HAtimerDurName,   nullptr,            nullptr,         nullptr},
    {TimerHaEntity::Remaining,"sensor", HAtimerRemID,   HAtimerRemIcon,   HAtimerRemName,   nullptr,            HAtimerRemUnit,  HAtimerRemClass},
    {TimerHaEntity::State,    "sensor", HAtimerStateID, HAtimerStateIcon, HAtimerStateName, nullptr,            nullptr,         nullptr},
    {TimerHaEntity::Buzzer,   "select", HAtimerBuzID,   HAtimerBuzIcon,   HAtimerBuzName,   HAtimerBuzOptions.c_str(),  nullptr,         nullptr},
    {TimerHaEntity::Finished, "select", HAtimerFinID,   HAtimerFinIcon,   HAtimerFinName,   HAtimerFinOptions.c_str(),  nullptr,         nullptr},
    {TimerHaEntity::Start,    "button", HAtimerStartID, HAtimerStartIcon, HAtimerStartName, nullptr,            nullptr,         nullptr},
    {TimerHaEntity::Pause,    "button", HAtimerPauseID, HAtimerPauseIcon, HAtimerPauseName, nullptr,            nullptr,         nullptr},
    {TimerHaEntity::Reset,    "button", HAtimerResetID, HAtimerResetIcon, HAtimerResetName, nullptr,            nullptr,         nullptr},
    {TimerHaEntity::SyncFollow,  "switch", HAtimerSyncFollowID,  HAtimerSyncFollowIcon,  HAtimerSyncFollowName,  nullptr,                   nullptr, nullptr},
    {TimerHaEntity::SyncTargets, "select", HAtimerSyncTargetsID, HAtimerSyncTargetsIcon, HAtimerSyncTargetsName, HAtimerSyncTargetsOptions, nullptr, nullptr},
};

// --- Dynamic SyncTargets select ---------------------------------------
// The select's options and id<->index mapping are derived at runtime from the
// CURRENT peer-id list. See the contract in TimerHa.h.

void timerSyncTargetsBuildOptions(const char *const *ids, size_t n, char *out, size_t outLen)
{
    // Base options are always present; peers append in the caller-provided (sorted)
    // order. String here is local scratch — the device build has it via Arduino.h.
    String s = "Off;All";
    for (size_t i = 0; i < n; ++i)
    {
        s += ';';
        s += ids[i];
    }
    snprintf(out, outLen, "%s", s.c_str());
}

int8_t timerSyncTargetsIndexForValue(const char *syncTargets, const char *const *ids, size_t n)
{
    if (syncTargets == nullptr || syncTargets[0] == '\0') return (int8_t)TimerSyncTargetsOption::Off;
    if (strcmp(syncTargets, "all") == 0) return (int8_t)TimerSyncTargetsOption::All;
    // A multi-id CSV cannot be expressed as a single select option -> unknown.
    if (strchr(syncTargets, ',') != nullptr) return -1;
    // A single specific id reflects only while it is still a discovered peer; the
    // index follows its position in the sorted list (after the two base options).
    for (size_t i = 0; i < n; ++i)
        if (strcmp(syncTargets, ids[i]) == 0) return (int8_t)(2 + i);
    return -1;   // an id no longer present -> unknown (HASelect: no option selected)
}

bool timerSyncTargetsValueForIndex(int8_t index, const char *const *ids, size_t n, char *out, size_t outLen)
{
    if (index == (int8_t)TimerSyncTargetsOption::Off) { snprintf(out, outLen, "%s", ""); return true; }
    if (index == (int8_t)TimerSyncTargetsOption::All) { snprintf(out, outLen, "%s", "all"); return true; }
    if (index >= 2 && (size_t)(index - 2) < n) { snprintf(out, outLen, "%s", ids[index - 2]); return true; }
    return false;
}

void formatTimerHaEntityId(const TimerHaDescriptor &d, const char *macSuffix, char *out, size_t outLen)
{
    snprintf(out, outLen, d.idFormat, macSuffix);
}

void formatTimerHaDataTopic(const char *dataPrefix, const char *deviceUniqueId,
                            const char *entityId, char *out, size_t outLen)
{
    // "stat_t" is ArduinoHA's HAStateTopic suffix; see the contract in TimerHa.h.
    snprintf(out, outLen, "%s/%s/%s/stat_t", dataPrefix, deviceUniqueId, entityId);
}

void formatTimerHaAttrTopic(const char *dataPrefix, const char *deviceUniqueId,
                            const char *entityId, char *out, size_t outLen)
{
    // "json_attr_t" is ArduinoHA's HAJsonAttributesTopic suffix; see the contract
    // in TimerHa.h.
    snprintf(out, outLen, "%s/%s/%s/json_attr_t", dataPrefix, deviceUniqueId, entityId);
}

bool haRegistrationAtCap(uint8_t registered, uint8_t maxEntities)
{
    // Mirror byte-for-byte ArduinoHA's HAMqtt::addDeviceType guard
    // (`_devicesTypesNb + 1 >= _maxDevicesTypesNb`); the uint16_t widening keeps a
    // maxEntities of 255 from wrapping. See the contract in TimerHa.h.
    return (uint16_t)registered + 1 >= maxEntities;
}
#endif // AWTRIX_DISABLE_TIMER
