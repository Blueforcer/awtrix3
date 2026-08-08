#ifndef AWTRIX_DISABLE_TIMER
#include "TimerHaHost.h"
#include <ArduinoHA.h>
#include <WiFi.h>
#include "Globals.h"
#include "MQTTManager.h"   // kMaxHAEntities (the general MQTT module's HA entity cap)
#include "TimerManager.h"
#include "TimerSettings.h"  // timerFormatHMS (the Duration state's clock string)
#include "TimerHa.h"
#include "SyncTargetsDebounce.h"

// The ArduinoHA client + device stay owned by the general MQTT translation unit
// (MQTTManager.cpp); this file reaches them via extern (no injection).
extern HADevice device;
extern HAMqtt mqtt;

// The shared ArduinoHA callbacks stay in MQTTManager (they keep their non-Timer
// branches); createCarriers() wires the Timer carriers onto them, and each delegates
// its Timer branch back to this host via tryHandle* (leading guard in MQTTManager).
extern void onButtonCommand(HAButton *sender);
extern void onSelectCommand(int8_t index, HASelect *sender);
extern void onSwitchCommand(bool state, HASwitch *sender);

// --- Carrier state (private to the host) -------------------------------------
// The ten HA carrier entity pointers, all file-local.
static HAText *timerDuration = nullptr;
static HASensorNumber *timerRemaining = nullptr;
static HASensor *timerStateSensor = nullptr;
static HASelect *timerBuzzer = nullptr, *timerFinishedSel = nullptr;
static HAButton *timerStartBtn = nullptr, *timerPauseBtn = nullptr, *timerResetBtn = nullptr;
// Sync-control entities: the writable Follow switch and dynamic
// Off/All Targets select that make the two sync settings controllable from HA.
static HASwitch *timerSyncFollowSw = nullptr;
static HASelect *timerSyncTargetsSel = nullptr;

// Each Timer entity's resolved HA discovery unique id ("%s" filled with the MAC),
// indexed by TimerHaEntity slot (timerHaIds[(size_t)slot]). Filled once in setup()
// via formatTimerHaEntityId() and read by both createCarriers() and remove()
// through timerHaId(slot) — never by hand-ordered position, so create and teardown
// cannot drift. The strings must outlive the entities: ArduinoHA stores the
// unique-id pointer, not a copy. Exposed to MQTTManager's wire seam via entityId().
static char timerHaIds[TIMER_HA_DESCRIPTOR_COUNT][40];

// The id buffer for a given Timer HA slot. See timerHaIds above.
static char *timerHaId(TimerHaEntity slot) { return timerHaIds[static_cast<size_t>(slot)]; }

// --- Dynamic Targets select debounce state (private to the host) ------
// Option-string sizing for the dynamic Targets select.
constexpr size_t kSyncTargetPeerCap = 16;
constexpr size_t kSyncTargetOptsCap = 8 + kSyncTargetPeerCap * 33 + 1; // "Off;All" + ";<id>"...
// The settle-window republish decision (signature, dirty flag, window start) lives in
// the SyncTargetsDebounce module; this instance
// is the only debounce state. createCarriers() seeds it; refreshTargets() steps it.
static SyncTargetsDebounce syncTargetsDebounce;

// Snapshot the current sorted peer ids and build the select's option string into
// optsOut. Returns the peer count; idsOut holds the ids (whose c_str() backs ptrsOut)
// so the caller can also map a value<->index against the SAME list.
static size_t buildCurrentSyncTargetsOptions(String *idsOut, const char **ptrsOut, size_t cap,
                                             char *optsOut, size_t optsLen)
{
    size_t n = TimerManager.peerIds(idsOut, cap);
    for (size_t i = 0; i < n; ++i) ptrsOut[i] = idsOut[i].c_str();
    timerSyncTargetsBuildOptions(ptrsOut, n, optsOut, optsLen);
    return n;
}

// --- SHOW_TIMER reconcile bookkeeping (private to the host) ------------------
// Set by reconcile() when SHOW_TIMER latched off across a reboot; onConnected()
// consumes it once MQTT is up to prune the Timer carriers' discovery. Private host
// state — nothing outside the host touches it. SHOW_TIMER_HA_PREV, by
// contrast, is a persisted (NVS-backed) shared global the settings-apply path writes
// independently, so it stays a Globals-owned flag reconcile() reads/writes via extern
// — exactly like SHOW_TIMER itself.
static bool pendingTimerHADiscoveryCleanup = false;

// The dedicated Timer duration text callback (registered on the Duration carrier).
static void onTimerDurationMessage(const char *message, uint16_t length, HAText *sender)
{
    String in;
    in.reserve(length);
    for (uint16_t i = 0; i < length; i++) in += message[i];
    in.trim();

    // Route the raw HH:MM:SS text through parseCommand: it owns the
    // parse/validate (timerParseHMS + range, reject-not-clamp) — the HA layer no longer
    // duplicates that logic. A rejected input applies nothing (atomic-reject).
    TimerManager.timerHaApply(TimerHaEntity::Duration, in);

    // Echo the canonical value back so rejected input snaps the field to the
    // previous valid time rather than leaving the bad text displayed.
    sender->setState(timerFormatHMS(TimerManager.getDuration()).c_str(), true);
}

// Creates the Timer HA entity objects (and registers them with HAMqtt via their
// constructors). Idempotent: the objects persist for the device lifetime, so a second
// call is a no-op. This must run with the Timer entity id buffers already resolved
// (setup() fills them before SHOW_TIMER is consulted).
static void createCarriers()
{
    if (timerDuration) return;

    // Each entity's strings come from the descriptor table; the ArduinoHA
    // object type and the type-specific wiring (callbacks, initial state)
    // stay here because they're heterogeneous across HA component types.
    const TimerHaDescriptor &dDur   = timerHaDescriptor(TimerHaEntity::Duration);
    const TimerHaDescriptor &dRem   = timerHaDescriptor(TimerHaEntity::Remaining);
    const TimerHaDescriptor &dState = timerHaDescriptor(TimerHaEntity::State);
    const TimerHaDescriptor &dBuz   = timerHaDescriptor(TimerHaEntity::Buzzer);
    const TimerHaDescriptor &dFin   = timerHaDescriptor(TimerHaEntity::Finished);
    const TimerHaDescriptor &dStart = timerHaDescriptor(TimerHaEntity::Start);
    const TimerHaDescriptor &dPause = timerHaDescriptor(TimerHaEntity::Pause);
    const TimerHaDescriptor &dReset = timerHaDescriptor(TimerHaEntity::Reset);

    timerDuration = new HAText(timerHaId(TimerHaEntity::Duration));
    timerDuration->setIcon(dDur.icon);
    timerDuration->setName(dDur.name);
    timerDuration->setRetain(true);
    timerDuration->onMessage(onTimerDurationMessage);
    timerDuration->setState(timerFormatHMS(TimerManager.getDuration()).c_str(), true);
    // Opt the Duration text entity into JSON attributes (HAText opt-in)
    // so its discovery config advertises json_attr_t; its retained {max_duration}
    // object — the cap in carrier-native clock form ("24:00:00") — rides the wire
    // seam to that topic. No TIMER_HA_DESCRIPTORS change.
    timerDuration->setJsonAttributes(true);

    timerRemaining = new HASensorNumber(timerHaId(TimerHaEntity::Remaining), HASensorNumber::PrecisionP0);
    timerRemaining->setIcon(dRem.icon);
    timerRemaining->setName(dRem.name);
    timerRemaining->setUnitOfMeasurement(dRem.unit);
    timerRemaining->setDeviceClass(dRem.deviceClass);
    timerRemaining->setCurrentValue((uint32_t)TimerManager.getRemaining());
    // Opt the remaining sensor into JSON attributes (HASensorNumber inherits the
    // opt-in from HASensor): its retained {remaining_publish_interval} object rides
    // the wire seam to json_attr_t. No descriptor change.
    timerRemaining->setJsonAttributes(true);

    timerStateSensor = new HASensor(timerHaId(TimerHaEntity::State));
    timerStateSensor->setIcon(dState.icon);
    timerStateSensor->setName(dState.name);
    // Opt the state sensor into JSON attributes so its discovery config advertises
    // json_attr_t; the retained config-view object (max_duration, bar_color as
    // "#RRGGBB", sync roles, …) rides the wire seam to that topic.
    timerStateSensor->setJsonAttributes(true);

    timerBuzzer = new HASelect(timerHaId(TimerHaEntity::Buzzer));
    timerBuzzer->setOptions(dBuz.options);
    timerBuzzer->onCommand(onSelectCommand);
    timerBuzzer->setIcon(dBuz.icon);
    timerBuzzer->setName(dBuz.name);
    timerBuzzer->setState((uint8_t)TimerManager.getBuzzerMode(), true);
    // Opt the buzzer select into JSON attributes so its discovery config advertises
    // json_attr_t; its retained {countdown_seconds, melody_tick, melody_end} object
    // rides the wire seam to that topic. No descriptor change.
    timerBuzzer->setJsonAttributes(true);

    timerFinishedSel = new HASelect(timerHaId(TimerHaEntity::Finished));
    timerFinishedSel->setOptions(dFin.options);
    timerFinishedSel->onCommand(onSelectCommand);
    timerFinishedSel->setIcon(dFin.icon);
    timerFinishedSel->setName(dFin.name);
    timerFinishedSel->setState((uint8_t)TimerManager.getFinishedMode(), true);
    // Opt the finished select into JSON attributes so its discovery config
    // advertises json_attr_t; the retained {"realert_interval":N} then rides the
    // wire seam to that topic. No TIMER_HA_DESCRIPTORS change.
    timerFinishedSel->setJsonAttributes(true);

    timerStartBtn = new HAButton(timerHaId(TimerHaEntity::Start));
    timerStartBtn->setIcon(dStart.icon);
    timerStartBtn->setName(dStart.name);
    timerStartBtn->onCommand(onButtonCommand);

    timerPauseBtn = new HAButton(timerHaId(TimerHaEntity::Pause));
    timerPauseBtn->setIcon(dPause.icon);
    timerPauseBtn->setName(dPause.name);
    timerPauseBtn->onCommand(onButtonCommand);

    timerResetBtn = new HAButton(timerHaId(TimerHaEntity::Reset));
    timerResetBtn->setIcon(dReset.icon);
    timerResetBtn->setName(dReset.name);
    timerResetBtn->onCommand(onButtonCommand);

    // Sync-control entities. The two sync settings — until now
    // read-only attributes on the state sensor — become writable here. Both
    // callbacks route through TimerManager::timerHaApply -> parseCommand, so they
    // inherit the atomic-reject validation and NVS persistence the rest of the
    // control surface has. sync_* are local identity (inSnapshot=false) and never
    // propagate to peers —.
    const TimerHaDescriptor &dSyncF = timerHaDescriptor(TimerHaEntity::SyncFollow);
    const TimerHaDescriptor &dSyncT = timerHaDescriptor(TimerHaEntity::SyncTargets);

    timerSyncFollowSw = new HASwitch(timerHaId(TimerHaEntity::SyncFollow));
    timerSyncFollowSw->setIcon(dSyncF.icon);
    timerSyncFollowSw->setName(dSyncF.name);
    timerSyncFollowSw->onCommand(onSwitchCommand);
    timerSyncFollowSw->setState(TIMER_SYNC_FOLLOW, true);

    timerSyncTargetsSel = new HASelect(timerHaId(TimerHaEntity::SyncTargets));
    // Dynamic options: "Off;All" plus each currently-discovered peer id,
    // not the static dSyncT.options table field — the select tracks the peer registry.
    String        stIds[kSyncTargetPeerCap];
    const char   *stPtrs[kSyncTargetPeerCap];
    char          stOpts[kSyncTargetOptsCap];
    size_t        stN = buildCurrentSyncTargetsOptions(stIds, stPtrs, kSyncTargetPeerCap,
                                                       stOpts, sizeof(stOpts));
    timerSyncTargetsSel->setOptions(stOpts);
    syncTargetsDebounce.seed(stOpts);   // seed the republish baseline (no spurious first republish)
    timerSyncTargetsSel->onCommand(onSelectCommand);
    timerSyncTargetsSel->setIcon(dSyncT.icon);
    timerSyncTargetsSel->setName(dSyncT.name);
    // Reflect the current sync_targets against the CURRENT id list: Off/All, the
    // matching peer's option, or unknown (-1) for a multi-ID CSV / an id no longer
    // present — the read-only attribute stays authoritative for the exact value.
    timerSyncTargetsSel->setState(
        timerSyncTargetsIndexForValue(TIMER_SYNC_TARGETS.c_str(), stPtrs, stN), true);

    // Every `new HAX` above auto-registered with HAMqtt via the
    // HABaseDeviceType ctor. ArduinoHA drops entities once registration reaches the
    // effective cap (kMaxHAEntities - 1; see haRegistrationAtCap) — which silently
    // dropped these two sync-control entities until the cap was raised. Log the count
    // and warn if we're back at the cap (the runtime guard, since the build-flag-
    // conditional base count rules out a clean compile-time check).
    if (DEBUG_MODE)
    {
        uint8_t registered = mqtt.getDevicesTypesNb();
        DEBUG_PRINTF("HA entities registered: %u of %u", registered, kMaxHAEntities);
        if (haRegistrationAtCap(registered, kMaxHAEntities))
            DEBUG_PRINTLN(F("WARN: HA entity registration at effective cap; entities may be dropped (raise kMaxHAEntities)"));
    }
}

void TimerHaHost_::setup()
{
    // Resolve every Timer entity's unique id from the Timer HA Presence table into
    // the slot-keyed timerHaIds buffers (also read by teardown), each through the one
    // formatTimerHaEntityId() helper so create and teardown agree per slot. Row i
    // describes slot i. Same MAC suffix MQTTManager::setup()
    // feeds its own entities, so the ids are unchanged by the move.
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char macStr[7];
    snprintf(macStr, 7, "%02x%02x%02x", mac[3], mac[4], mac[5]);
    for (size_t i = 0; i < TIMER_HA_DESCRIPTOR_COUNT; ++i)
        formatTimerHaEntityId(TIMER_HA_DESCRIPTORS[i], macStr, timerHaIds[i], sizeof(timerHaIds[i]));

    if (SHOW_TIMER)
        createCarriers();
}

void TimerHaHost_::onConnected()
{
    // Flush the one-shot cleanup reconcile() latched: if SHOW_TIMER went off across a
    // reboot, prune the Timer carriers' retained discovery now that MQTT is up. When
    // set, SHOW_TIMER is false, so the publish block below is skipped this connect.
    if (pendingTimerHADiscoveryCleanup)
    {
        remove();
        pendingTimerHADiscoveryCleanup = false;
    }
    if (SHOW_TIMER)
    {
        TimerManager.publishAllWire();   // every wire artifact, derived from the member table
        TimerManager.publishAllAttributeGroups();   // every carrier's read-only attribute object
    }
}

// Reconcile SHOW_TIMER against its persisted last-seen value at boot (before MQTT
// connects). This is the host equivalent of the free reconcileTimerHAState() that
// lived in MQTTManager: if the timer was toggled off while powered down,
// latch the one-shot discovery cleanup onConnected() flushes; and persist the new
// last-seen value whenever it changed. SHOW_TIMER_HA_PREV stays a shared persisted
// global (settings-apply writes it too), reached via extern —.
void TimerHaHost_::reconcile()
{
    if (SHOW_TIMER_HA_PREV && !SHOW_TIMER)
    {
        pendingTimerHADiscoveryCleanup = true;
    }
    if (SHOW_TIMER_HA_PREV != SHOW_TIMER)
    {
        SHOW_TIMER_HA_PREV = SHOW_TIMER;
        saveSettings();
    }
}

// Brings the Timer HA entities online at runtime when SHOW_TIMER flips false->true.
// Mirror of remove(): creates the entities if missing, then publishes their discovery
// config (so HA adds them without a full reconnect) and current values.
void TimerHaHost_::enable()
{
    if (!HA_DISCOVERY) return;
    createCarriers();
    if (!mqtt.isConnected()) return; // discovery will publish at next connect via onConnected()

    HABaseDeviceType *timerTypes[] = {
        timerDuration, timerRemaining, timerStateSensor, timerBuzzer,
        timerFinishedSel, timerStartBtn, timerPauseBtn, timerResetBtn,
        timerSyncFollowSw, timerSyncTargetsSel};
    for (HABaseDeviceType *dt : timerTypes)
        mqtt.publishConfigForDeviceType(dt);

    TimerManager.publishAllWire();   // every wire artifact, derived from the member table
    TimerManager.publishAllAttributeGroups();   // every carrier's read-only attribute object
}

void TimerHaHost_::remove()
{
    const char *deviceUniqueId = device.getUniqueId();
    if (!deviceUniqueId) return;
    char topic[160];
    for (size_t i = 0; i < TIMER_HA_DESCRIPTOR_COUNT; ++i)
    {
        const TimerHaDescriptor &d = TIMER_HA_DESCRIPTORS[i];
        snprintf(topic, sizeof(topic), "%s/%s/%s/%s/config",
                 HA_PREFIX.c_str(), d.component, deviceUniqueId, timerHaId(d.slot));
        mqtt.publish(topic, "", true);
    }
    // Clear each carrier's retained json_attr_t object too, so pruning the
    // discovery config does not leave an orphaned attribute payload behind on the
    // broker. Rides the same wire seam the attribute publish does.
    TimerManager.clearAllAttributeGroups();
}

// Re-publish the Targets select's discovery when peer-registry membership changes,
// debounced so a burst of beacon churn yields one republish. No-op when
// the timer HA entities are absent or MQTT is down (discovery re-publishes at the next
// connect). On a settled change it rebuilds the options, re-publishes the discovery
// config so Home Assistant sees the new list, and re-applies the selected state.
void TimerHaHost_::refreshTargets(unsigned long nowMs)
{
    if (timerSyncTargetsSel == nullptr) return;
    if (!mqtt.isConnected()) return;

    String      ids[kSyncTargetPeerCap];
    const char *ptrs[kSyncTargetPeerCap];
    char        opts[kSyncTargetOptsCap];
    size_t      n = buildCurrentSyncTargetsOptions(ids, ptrs, kSyncTargetPeerCap,
                                                   opts, sizeof(opts));

    // The settle-window decision (unchanged / start window / waiting / republish) is
    // SyncTargetsDebounce's transition table; only Republish reaches the effects below.
    // The guards above stay BEFORE step() so window state ages across MQTT-down gaps.
    if (syncTargetsDebounce.step(opts, nowMs) != SyncTargetsDebounce::Action::Republish)
        return;

    timerSyncTargetsSel->resetOptions();
    timerSyncTargetsSel->setOptions(opts);
    mqtt.publishConfigForDeviceType(timerSyncTargetsSel);
    timerSyncTargetsSel->setState(
        timerSyncTargetsIndexForValue(TIMER_SYNC_TARGETS.c_str(), ptrs, n), true);
}

bool TimerHaHost_::tryHandleButton(HAButton *sender)
{
    if (sender == timerStartBtn)
    {
        // Route through parseCommand: start/pause/reset re-enter the
        // control surface, so HA buttons drive peer clocks (run-state propagation)
        // the same way the MQTT topic does. parseCommand also owns the
        // start-from-idle switch-to-Timer-app behaviour, so it isn't duplicated here.
        TimerManager.timerHaApply(TimerHaEntity::Start, "");
        return true;
    }
    if (sender == timerPauseBtn)
    {
        TimerManager.timerHaApply(TimerHaEntity::Pause, "");
        return true;
    }
    if (sender == timerResetBtn)
    {
        TimerManager.timerHaApply(TimerHaEntity::Reset, "");
        return true;
    }
    return false;
}

bool TimerHaHost_::tryHandleSwitch(bool state, HASwitch *sender)
{
    if (sender == timerSyncFollowSw)
    {
        // Route through parseCommand: the Follow switch re-enters the
        // control surface, so it gets the strict-bool atomic-reject validation and
        // NVS persistence. sync_follow is local identity and never propagates.
        TimerManager.timerHaApply(TimerHaEntity::SyncFollow, String(state ? 1 : 0));
        // Echo the value actually applied back (snap-back if a write were rejected).
        sender->setState(TIMER_SYNC_FOLLOW);
        return true;
    }
    return false;
}

bool TimerHaHost_::tryHandleSelect(HASelect *sender, int8_t index)
{
    if (sender == timerBuzzer)
    {
        // Route through parseCommand: the HA select re-enters the
        // control surface like every other edit, so it gets atomic-reject
        // validation and the per-enum codec wire spelling — no deep setter here.
        TimerManager.timerHaApply(TimerHaEntity::Buzzer, String(index));
        // Echo the canonical live mode back (snap-back to last valid on a reject).
        sender->setState((int8_t)TimerManager.getBuzzerMode());
        return true;
    }
    if (sender == timerFinishedSel)
    {
        TimerManager.timerHaApply(TimerHaEntity::Finished, String(index));
        sender->setState((int8_t)TimerManager.getFinishedMode());
        return true;
    }
    if (sender == timerSyncTargetsSel)
    {
        // Dynamic select: resolve the chosen option index through the
        // CURRENT peer-id list to its sync_targets value (Off -> "", All -> "all", a
        // peer option -> that id), then route through parseCommand for the
        // bespoke validator + NVS persistence. Echo the canonical applied value back
        // against the same list (Off/All/peer, or unknown for a multi-ID CSV / an id
        // no longer present — the read-only attribute stays authoritative). sync_targets
        // never propagates.
        String      ids[kSyncTargetPeerCap];
        const char *ptrs[kSyncTargetPeerCap];
        char        opts[kSyncTargetOptsCap];
        size_t      n = buildCurrentSyncTargetsOptions(ids, ptrs, kSyncTargetPeerCap,
                                                       opts, sizeof(opts));
        char val[40];
        if (timerSyncTargetsValueForIndex(index, ptrs, n, val, sizeof(val)))
            TimerManager.timerHaApply(TimerHaEntity::SyncTargets, val);
        sender->setState(timerSyncTargetsIndexForValue(TIMER_SYNC_TARGETS.c_str(), ptrs, n));
        return true;
    }
    return false;
}

bool TimerHaHost_::carriersReady() const { return timerDuration != nullptr; }

const char *TimerHaHost_::entityId(TimerHaEntity slot) const { return timerHaId(slot); }

TimerHaHost_ TimerHaHost;
#endif // AWTRIX_DISABLE_TIMER
