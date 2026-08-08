#ifndef TimerHa_h
#define TimerHa_h

#include <stddef.h>
#include <stdint.h>

// Timer HA Presence: the single source of truth for the Home Assistant entities
// that project the Timer over the MQTT discovery layer. This header is pure data
// (no ArduinoHA, no Globals) so it compiles on the host and the contract below can
// be unit-tested. The ArduinoHA `new HAX + setters` apply and the discovery
// teardown both read TIMER_HA_DESCRIPTORS in MQTTManager — one table, no drift.
// See docs/timer.md.

enum class TimerHaEntity : uint8_t
{
    Duration = 0,
    Remaining,
    State,
    Buzzer,
    Finished,
    Start,
    Pause,
    Reset,
    SyncFollow,   // writable switch: the receive-consent toggle (sync_follow)
    SyncTargets,  // writable select: static Off/All targeting (sync_targets)
    COUNT
};

// The static option indices for the SyncTargets select. The select
// ships ONLY these two base options ("Off"/"All"); a specific-ID CSV set out-of-band
// is reflected as unknown (-1) — see timerSyncTargetsIndexForValue.
enum class TimerSyncTargetsOption : int8_t
{
    Off = 0,   // sync_targets == ""  (receive only / no relay)
    All = 1,   // sync_targets == "all"
    COUNT
};

struct TimerHaDescriptor
{
    TimerHaEntity slot;
    const char *component;    // HA discovery component: "text" | "sensor" | "select" | "button"
    const char *idFormat;     // unique-id format, e.g. "%s_timer_dur" ("%s" is the MAC suffix)
    const char *icon;
    const char *name;
    const char *options;      // select only ("`;`"-joined), else nullptr
    const char *unit;         // sensor only, else nullptr
    const char *deviceClass;  // sensor only, else nullptr
};

// One descriptor per TimerHaEntity slot, in slot order (TIMER_HA_DESCRIPTORS[(size_t)slot]).
constexpr size_t TIMER_HA_DESCRIPTOR_COUNT = static_cast<size_t>(TimerHaEntity::COUNT);
extern const TimerHaDescriptor TIMER_HA_DESCRIPTORS[TIMER_HA_DESCRIPTOR_COUNT];

// Convenience accessor: the descriptor for a given slot.
inline const TimerHaDescriptor &timerHaDescriptor(TimerHaEntity slot)
{
    return TIMER_HA_DESCRIPTORS[static_cast<size_t>(slot)];
}

// The single place a descriptor row + device MAC suffix becomes an entity's HA
// discovery unique id: formats the row's idFormat ("%s" -> macSuffix) into
// out[outLen]. Discovery setup (fill) and teardown both derive ids through this,
// keyed by the row they hold, so the two cannot derive different ids for a slot
// and reordering the table moves no entity's id.
void formatTimerHaEntityId(const TimerHaDescriptor &d, const char *macSuffix, char *out, size_t outLen);

// The single place an entity id becomes the full MQTT data (state) topic the
// broker sees: "{dataPrefix}/{deviceUniqueId}/{entityId}/stat_t". MUST stay
// byte-identical to what ArduinoHA emits for the same entity
// (HASerializer::generateDataTopic with the HAStateTopic suffix "stat_t") —
// the wire seam publishes to this topic where setValue() used to, so a format
// drift here is a wire-protocol change. Pinned by test W1.
void formatTimerHaDataTopic(const char *dataPrefix, const char *deviceUniqueId,
                            const char *entityId, char *out, size_t outLen);

// The finished-mode select's JSON-attributes data topic:
// "{dataPrefix}/{deviceUniqueId}/{entityId}/json_attr_t". Same data-topic shape
// as formatTimerHaDataTopic but with ArduinoHA's HAJsonAttributesTopic suffix
// ("json_attr_t") — MUST stay byte-identical to what HASelect::setJsonAttributes
// advertises in the discovery config, since the retained attribute value rides
// the wire seam to this exact topic. Pinned by test W13.
void formatTimerHaAttrTopic(const char *dataPrefix, const char *deviceUniqueId,
                            const char *entityId, char *out, size_t outLen);

// Dynamic SyncTargets select. The select consumes the peer registry, so its
// option list and id<->index mapping are built at runtime from the CURRENT set of
// discovered peer ids (already sorted) rather than the static "Off;All" table field.
// All three helpers are pure (char* / out-buffer, no String/Globals), like the
// rest of this contract.

// Build the option list "Off;All" followed by ";<id>" for each of the n peer ids,
// in the given order, into out[outLen] (truncated like snprintf). n==0 -> "Off;All".
void timerSyncTargetsBuildOptions(const char *const *ids, size_t n, char *out, size_t outLen);

// Forward map (sync_targets value -> select option index) over the CURRENT id list:
// "" -> 0 (Off), "all" -> 1 (All), a single id present at sorted position k -> 2+k,
// an id no longer present OR a multi-id CSV -> -1 (unknown / no option selected).
// With n==0 this reduces to static Off/All/-1 behaviour (the select's base options).
int8_t timerSyncTargetsIndexForValue(const char *syncTargets, const char *const *ids, size_t n);

// Reverse map (select option index -> sync_targets value) over the CURRENT id list:
// 0 -> "", 1 -> "all", k>=2 -> ids[k-2], written into out[outLen]. Returns false
// (and leaves out untouched) when index is out of range — the command path's BadField.
bool timerSyncTargetsValueForIndex(int8_t index, const char *const *ids, size_t n, char *out, size_t outLen);

// ArduinoHA's HAMqtt::addDeviceType drops an entity when
// `_devicesTypesNb + 1 >= _maxDevicesTypesNb`, so the EFFECTIVE capacity is
// maxEntities - 1, not maxEntities (an off-by-one that silently dropped the two
// Timer sync-control entities). This helper encodes that guard once,
// so the DEBUG_MODE registration check and ArduinoHA agree: it returns
// true when `registered` entities already fill the effective cap, i.e. the next
// addDeviceType would be rejected. Pure (no ArduinoHA/Globals).
bool haRegistrationAtCap(uint8_t registered, uint8_t maxEntities);

#endif
