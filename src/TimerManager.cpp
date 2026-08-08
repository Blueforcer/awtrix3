#ifndef AWTRIX_DISABLE_TIMER
#include "TimerManager.h"
#include "TimerSettings.h"
#include "TimerCommand.h"   // the pure atomic-reject command plan (classify)
#include "TimerHa.h"
#include "SyncEnvelope.h"     // wire envelope + pure inbound receive gate
#include "Globals.h"
#include "PeripheryManager.h"
#include "DisplayManager.h"
#include "MQTTManager.h"
#include "MenuManager.h"
#include "ServerManager.h"
#include <Preferences.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

namespace {
    // Sized to hold the full config snapshot (~18 keys) plus the _sync envelope on
    // the propagation surface, with headroom for ArduinoJson's larger 64-bit
    // slots. The HTTP/MQTT control surfaces never approach it.
    constexpr uint16_t kTimerCmdJsonSize     = 2048;

    const char *FALLBACK_END_RTTTL  = "timer:d=4,o=5,b=120:c,8p,c,8p,c";
    const char *FALLBACK_TICK_RTTTL = "tick:d=16,o=6,b=200:c";

    // The config-block keys that stay member-backed (B1 boundary) now
    // live in TIMER_MEMBER_CONFIG_DESCS (TimerSettings.cpp), the config block's second
    // table. Validation, apply, snapshot-emit and the broadcast trigger all loop that
    // one table, so they cannot drift apart.

    // The one home for the wire-refresh first-appearance dedup. Walk `count` rows,
    // project each to a key via keyOf(i), and invoke fn(key) once per DISTINCT key in
    // first-appearance order. publishAll{AttributeGroups,Wire} and clearAllAttributeGroups
    // differ only in the key projected (carrier enum vs publish-hook pointer) and the
    // per-distinct action, so the O(n^2) dedup rule lives here alone -- a new carrier or
    // shared hook is picked up by all three paths automatically. Host-compatible.
    template <typename KeyOf, typename Fn>
    void forEachDistinct(size_t count, KeyOf keyOf, Fn fn)
    {
        for (size_t i = 0; i < count; ++i)
        {
            auto key = keyOf(i);
            bool seen = false;
            for (size_t j = 0; j < i && !seen; ++j)
                seen = (keyOf(j) == key);
            if (!seen) fn(key);
        }
    }
}

static Preferences timerPrefs;

TimerManager_ &TimerManager_::getInstance()
{
    static TimerManager_ instance;
    return instance;
}

TimerManager_ &TimerManager = TimerManager_::getInstance();

void TimerManager_::loadMelodiesCached()
{
    const char *endName  = TIMER_MELODY_END.length()  > 0 ? TIMER_MELODY_END.c_str()  : "timer_end";
    const char *tickName = TIMER_MELODY_TICK.length() > 0 ? TIMER_MELODY_TICK.c_str() : "timer_tick";
    endRtttl  = PeripheryManager.resolveRtttl(endName,  FALLBACK_END_RTTTL);
    tickRtttl = PeripheryManager.resolveRtttl(tickName, FALLBACK_TICK_RTTTL);
}

// One-shot override. captureSnapshot records the SAVED config
// before a save:false command applies on top of it; restoreSnapshot writes it back
// when the timer returns to Idle. The table half (Family A inSnapshot rows) is captured
// generically over TIMER_SETTINGS_DESCS; the member-backed half (Family B) is captured as
// one TimerMemberConfig value (the two helpers below are the single home of its field
// list); the run-state duration and the resolved melody RAM are TimerManager's own members
// captured directly. Restore writes members directly (no setter), so it triggers no
// persist/publish side effects — carriers reported saved values throughout.
// The single home of the member-half field list: read live -> value.
// The NVS string key for each state's icon slot, indexed by TimerState. Shared by
// the load (setup) and save (persist) loops so the two cannot drift apart.
static const char *const kIconNvsKeys[kTimerStateCount] = {
    "ICON_IDLE", "ICON_RUN", "ICON_PAUSE", "ICON_FIN"};

TimerMemberConfig TimerManager_::snapshotMemberConfig() const
{
    TimerMemberConfig c{buzzerMode, finishedMode};
    for (size_t i = 0; i < kTimerStateCount; ++i) c.iconByState[i] = iconByState[i];
    return c;
}

// ...and value -> live, RAW: no persist, no publish, no broadcast. The override revert
// and the honest-observation swap both need exactly this side-effect-free write.
void TimerManager_::restoreMemberConfig(const TimerMemberConfig &c)
{
    buzzerMode   = c.buzzer;
    finishedMode = c.finished;
    for (size_t i = 0; i < kTimerStateCount; ++i) iconByState[i] = c.iconByState[i];
}

void TimerManager_::captureSnapshot()
{
    timerSettingsCaptureSnapshot(_snapTable);
    _snapMember    = snapshotMemberConfig();
    _snapDuration  = durationSec;   // run-state: reverts with the override, outside the member half
    _snapEndRtttl  = endRtttl;      // resolved melody RAM: ditto (the saved NAME is a table key)
    _snapTickRtttl = tickRtttl;
}

void TimerManager_::restoreSnapshot()
{
    timerSettingsRestoreSnapshot(_snapTable);
    restoreMemberConfig(_snapMember);
    durationSec   = _snapDuration;
    endRtttl      = _snapEndRtttl;
    tickRtttl     = _snapTickRtttl;
}

// The single revert seam shared by reset() and the tick auto-clear transition: if a
// one-shot override is active, restore the saved config and clear the flag. A no-op
// otherwise, so the normal return-to-Idle paths are unaffected.
void TimerManager_::returnToIdle()
{
    if (!_overrideActive) return;
    restoreSnapshot();
    _overrideActive = false;
}

// Honest observation carriers: swap the SAVED config block into live
// storage for the duration of a carrier projection, then restore the effective
// (one-shot) values. Only the config-block state the projections read is swapped —
// the table inSnapshot rows (sync_* excluded by construction) and the member-backed
// half (buzzer/finished/icons). Run-state (duration) and the resolved melody RAM are
// not touched. No-op when no override is active, or when enable is false (the caller
// wants the EFFECTIVE view even under an override, e.g. buildConfigSnapshot(View::Effective)).
TimerManager_::SavedConfigScope::SavedConfigScope(const TimerManager_ &t, bool enable)
    : tm(const_cast<TimerManager_ &>(t)), active(enable && t._overrideActive)
{
    if (!active) return;
    // Stash the effective (one-shot) config block, then present the saved config.
    timerSettingsCaptureSnapshot(effTable);
    effMember = tm.snapshotMemberConfig();

    timerSettingsRestoreSnapshot(tm._snapTable);
    tm.restoreMemberConfig(tm._snapMember);
}

TimerManager_::SavedConfigScope::~SavedConfigScope()
{
    if (!active) return;
    timerSettingsRestoreSnapshot(effTable);
    tm.restoreMemberConfig(effMember);
}

void TimerManager_::setup()
{
    timerPrefs.begin("timer", false);
    durationSec    = timerPrefs.getUInt("DUR", 300);
    buzzerMode     = (BuzzerMode)timerPrefs.getUChar("BUZ", (uint8_t)BuzzerMode::End);
    finishedMode   = (FinishedMode)timerPrefs.getUChar("FIN", (uint8_t)FinishedMode::AutoClear);
    for (size_t i = 0; i < kTimerStateCount; ++i)
        iconByState[i] = timerPrefs.getString(kIconNvsKeys[i], "");
    timerPrefs.end();

    // dev.json per-state overrides (unchanged keys), indexed by TimerState.
    const String *const iconOverrides[kTimerStateCount] = {
        &TIMER_ICON_IDLE, &TIMER_ICON_RUNNING, &TIMER_ICON_PAUSED, &TIMER_ICON_FINISHED};
    for (size_t i = 0; i < kTimerStateCount; ++i)
        if (iconOverrides[i]->length() > 0) iconByState[i] = *iconOverrides[i];

    if (durationSec < 1) durationSec = 1;
    if (TIMER_MAX_DURATION > 0 && durationSec > TIMER_MAX_DURATION) durationSec = TIMER_MAX_DURATION;
    remainingSec = durationSec;
    state = TimerState::Idle;
    _suspendPersist = false;
    _dirty = false;        // just loaded from NVS: RAM matches it, nothing pending

    // A (re)boot knows no peers and has emitted no beacon yet — the peer registry is
    // pure RAM/LAN-derived state, repopulated by inbound beacons.
    _registry.setOwnId(uniqueID);
    _registry.clear();
    _lastPresenceMs   = 0;
    _presenceEverSent = false;

    // A (re)boot has applied no sync command yet; the dedup set is pure RAM
    //, repopulated by inbound commands.
    _seen.clear();

    loadMelodiesCached();
}

void TimerManager_::persist()
{
    timerPrefs.begin("timer", false);
    timerPrefs.putUInt("DUR", durationSec);
    timerPrefs.putUChar("BUZ", (uint8_t)buzzerMode);
    timerPrefs.putUChar("FIN", (uint8_t)finishedMode);
    for (size_t i = 0; i < kTimerStateCount; ++i)
        timerPrefs.putString(kIconNvsKeys[i], iconByState[i]);
    timerPrefs.end();
}

void TimerManager_::persistIfDirty()
{
    if (_suspendPersist)
    {
        _dirty = true;
        return;
    }
    persist();
    _dirty = false;
}

String TimerManager_::validateIconName(const String &name)
{
    // Coercing wrapper around the family's char-rule: accepted name passes
    // through (empty = clear), a rejected one coerces to "" (and logs). Single source
    // of the char-rule is timerIsValidIconName, so this cannot drift from the parser.
    if (timerIsValidIconName(name)) return name;
    if (DEBUG_MODE) DEBUG_PRINTLN("timer: icon name rejected");
    return String("");
}

const String &TimerManager_::getIconForState(TimerState s) const
{
    // Running/Paused/Finished fall back to Idle when their own slot is empty.
    const String &own = iconByState[(size_t)s];
    if (s != TimerState::Idle && own.length() == 0) return iconByState[(size_t)TimerState::Idle];
    return own;
}

void TimerManager_::setIcon(TimerState s, const String &name, bool publish)
{
    String n = validateIconName(name);
    if (name.length() > 0 && n.length() == 0) return;   // reject-guard: bad non-empty name is ignored
    String &slot = iconByState[(size_t)s];
    if (slot == n) return;
    slot = n;
    persistIfDirty();
    if (publish) publishIcons();
}

void TimerManager_::publishIcons()
{
    // The four icon rows declare ONE shared aggregate publish hook,
    // so dispatching any icon key reaches the same declaration.
    timerMemberConfigPublish("icon_idle");
}

uint32_t TimerManager_::computeCurrentRemaining() const
{
    if (state != TimerState::Running) return remainingSec;
    unsigned long elapsedMs = millis() - runStartMs;
    uint32_t elapsedSec = elapsedMs / 1000UL;
    if (elapsedSec >= runStartRemainingSec) return 0;
    return runStartRemainingSec - elapsedSec;
}

const char *TimerManager_::getStateString() const
{
    switch (state)
    {
        case TimerState::Idle:     return "idle";
        case TimerState::Running:  return "running";
        case TimerState::Paused:   return "paused";
        case TimerState::Finished: return "finished";
    }
    return "idle";
}

// Canonical wire spellings are a single row read from the per-enum codec table
// (src/TimerEnums.cpp) -- the enum value is the row index.
const char *TimerManager_::buzzerModeString() const
{
    return buzzerCodec(buzzerMode).wire;
}

const char *TimerManager_::finishedModeString() const
{
    return finishedCodec(finishedMode).wire;
}

String TimerManager_::getStateJson() const
{
    // Bumped from the old 512-byte fixed buffer to the shared command-size
    // constant the snapshot/broadcast paths use, to hold the added config mirror
    // (~20 keys incl. two melodies, a CSV sync_targets, four icon names).
    DynamicJsonDocument doc(kTimerCmdJsonSize);
    uint32_t remaining = computeCurrentRemaining();
    doc["state"]         = getStateString();
    doc["enabled"]       = (bool)SHOW_TIMER;
    doc["remaining"]     = remaining;
    doc["remaining_str"] = timerFormatHMS(remaining);
    doc["duration"]      = durationSec;
    doc["duration_str"]  = timerFormatHMS(durationSec);
    doc["buzzer"]        = buzzerModeString();
    doc["finished"]      = finishedModeString();

    // Persisted-config mirror: the complete two-table dump under a
    // nested `config` object, so an HTTP-only client reads back everything it can
    // POST. Built in a temp doc by the pure table-walking projection, then
    // deep-copied in -- duration stays top-level only (run-state, not config).
    DynamicJsonDocument cfg(kTimerCmdJsonSize);
    // During a one-shot override the `config` mirror reports the SAVED config, while
    // the top-level run-state above stays effective.
    withConfigView(View::Saved, [&] { timerBuildFullConfig(cfg); });
    doc["config"] = cfg.as<JsonObject>();

    String out;
    serializeJson(doc, out);
    return out;
}

// Apply the run-state bookkeeping a returned Transition names. The
// pure engine decides the phase change; this owns the state mutation — including
// enterRunning's runStart* capture and the one-shot override restore behind
// ToIdle — so the override store stays in the singleton. Runs
// BEFORE the effects, so the publishes carry the new phase/remaining.
void TimerManager_::applyTransition(const TimerRuntime::Result &r, unsigned long now,
                                    const TimerRuntime::Inputs &in)
{
    switch (r.transition)
    {
    case TimerRuntime::Transition::ToRunning:
        // enterRunning: a fresh start (from Idle/Finished) loads the full duration;
        // a resume (from Paused) keeps the frozen remaining. `state` is still the
        // prior phase here, so it decides which.
        if (state != TimerState::Paused) remainingSec = in.durationSec;
        state = TimerState::Running;
        runStartMs = now;
        runStartRemainingSec = remainingSec;
        runDurationSec = in.durationSec;   // progress-bar denominator (buffers a mid-run duration edit)
        lastPublishMs = now;
        break;
    case TimerRuntime::Transition::ToPaused:
        remainingSec = in.newRemaining;    // freeze the wall-clock remaining
        state = TimerState::Paused;
        break;
    case TimerRuntime::Transition::ToFinished:
        state = TimerState::Finished;
        remainingSec = 0;
        enteredFinishedMs = now;
        lastRealertMs = now;
        break;
    case TimerRuntime::Transition::ToIdle:
        returnToIdle();                    // one-shot: restore saved config (override store lives here)
        state = TimerState::Idle;
        remainingSec = in.durationSec;
        break;
    case TimerRuntime::Transition::IdleReload:
        remainingSec = in.durationSec;     // duration edited while Idle; phase stays Idle
        break;
    case TimerRuntime::Transition::None:
        break;
    }
}

// Thin adapter for the input-driven lifecycle verbs: resolve the
// inputs, let TimerRuntime::step() decide the transition + ordered effects, apply
// the run-state bookkeeping, then execute the effects. The same shape tick() uses,
// so start/pause/reset/setDuration and the countdown step share one seam.
void TimerManager_::runCommand(TimerRuntime::Command cmd)
{
    unsigned long now = millis();
    TimerRuntime::Inputs in = buildInputs();
    in.command = cmd;
    TimerRuntime::State st{state, remainingSec, lastPublishMs, enteredFinishedMs, lastRealertMs};
    TimerRuntime::Result r = TimerRuntime::step(st, now, in);
    applyTransition(r, now, in);
    for (const TimerRuntime::Effect &e : r.effects) applyEffect(e);
}

// Resolve the environment gates + config the pure engine reads into an Inputs
// value. tick() shares this across the Running and Finished branches,
// so the globals/singletons the engine deliberately doesn't name are touched in
// exactly one place.
TimerRuntime::Inputs TimerManager_::buildInputs() const
{
    TimerRuntime::Inputs in;
    in.durationSec      = durationSec;
    in.newRemaining     = computeCurrentRemaining();
    in.countdownArmed   = SOUND_ACTIVE && buzzerMode == BuzzerMode::Countdown && tickRtttl.length() > 0;
    in.isPlaying        = PeripheryManager.isPlaying();
    in.countdownSeconds = TIMER_COUNTDOWN_SECONDS;
    in.publishInterval  = TIMER_PUBLISH_INTERVAL;
    in.navigationFree   = !GAME_ACTIVE && !BLOCK_NAVIGATION && !MenuManager.inMenu;
    in.matrixOff        = MATRIX_OFF;
    in.brightness       = BRIGHTNESS;
    in.playEndTone      = SOUND_ACTIVE && buzzerMode != BuzzerMode::Off && endRtttl.length() > 0;
    in.finishedMode     = finishedMode;
    in.finishedHold     = TIMER_FINISHED_HOLD;
    in.realertInterval  = TIMER_REALERT_INTERVAL;
    in.realertArmed     = SOUND_ACTIVE && buzzerMode != BuzzerMode::Off;
    in.realertToneSet   = endRtttl.length() > 0;
    return in;
}

// Executes one effect against the hardware managers / wire seam. The full effect
// vocabulary is handled so the later slices can lean on it; the Finished path emits
// only a subset.
void TimerManager_::applyEffect(const TimerRuntime::Effect &e)
{
    switch (e.kind)
    {
    case TimerRuntime::EffectKind::PublishState:
        publishState();
        break;
    case TimerRuntime::EffectKind::PublishRemaining:
        publishRemaining();
        break;
    case TimerRuntime::EffectKind::SwitchToTimerApp:
    {
        String j = "{\"name\":\"Timer\",\"fast\":true}";
        DisplayManager.switchToApp(j.c_str());
        break;
    }
    case TimerRuntime::EffectKind::SetBrightness:
        DisplayManager.setBrightness(e.brightness);
        break;
    case TimerRuntime::EffectKind::PlayTone:
        PeripheryManager.playRTTTLString(e.tone == TimerRuntime::Tone::End ? endRtttl : tickRtttl);
        break;
    case TimerRuntime::EffectKind::StopSound:
        PeripheryManager.stopSound();
        break;
    }
}

void TimerManager_::start() { runCommand(TimerRuntime::Command::Start); }

void TimerManager_::pause() { runCommand(TimerRuntime::Command::Pause); }

void TimerManager_::reset() { runCommand(TimerRuntime::Command::Reset); }

// The run-state seam: the verb + its peer mirror, paired in one
// place. broadcastRunState self-no-ops under _remoteApply and sync-off, so this is
// safe to call unconditionally from any locally-driven path.
void TimerManager_::runStateAction(TimerCommand::Action a)
{
    switch (a)
    {
    case TimerCommand::Action::Start: start(); broadcastRunState("start"); break;
    case TimerCommand::Action::Pause: pause(); broadcastRunState("pause"); break;
    case TimerCommand::Action::Reset: reset(); broadcastRunState("reset"); break;
    case TimerCommand::Action::None:  break;   // no verb, no packet
    }
}

void TimerManager_::setDuration(uint32_t seconds)
{
    // Clamp + no-op guard stay in the adapter (input validation, not a transition);
    // durationSec must be committed before runCommand so buildInputs() feeds the
    // engine the NEW duration (the Idle reload / paused-edit reset load it into
    // remaining). The engine decides the phase transition; the duration
    // persist + republish are unconditional adapter concerns.
    if (seconds < 1) seconds = 1;
    if (TIMER_MAX_DURATION > 0 && seconds > TIMER_MAX_DURATION) seconds = TIMER_MAX_DURATION;
    if (durationSec == seconds) return;
    durationSec = seconds;
    runCommand(TimerRuntime::Command::SetDuration);   // Idle: reload remaining; Paused: reset to Idle (US6)
    persistIfDirty();
    publishDuration();
}

void TimerManager_::setBuzzerMode(BuzzerMode m, bool persist)
{
    if (buzzerMode == m) return;
    buzzerMode = m;
    if (persist) persistIfDirty();
    else _dirty = true;   // deferred edit: pending until the next PersistBatch commit
    publishBuzzerMode();
}

void TimerManager_::setFinishedMode(FinishedMode m, bool persist)
{
    if (finishedMode == m) return;
    finishedMode = m;
    if (persist) persistIfDirty();
    else _dirty = true;   // deferred edit: pending until the next PersistBatch commit
    publishFinishedMode();
}

// Thin adapter over the pure engine: resolve the inputs, let
// TimerRuntime::step() decide the ordered effects + the run-state bookkeeping for
// both the Running and Finished branches, then apply the bookkeeping and execute
// the effects in order. The state mutation (and the one-shot override restore
// behind ToIdle) stays here; effects run after it so the publishes carry the new
// phase/remaining, exactly as the imperative tick did.
void TimerManager_::tick()
{
    if (state != TimerState::Running && state != TimerState::Finished) return;

    unsigned long now = millis();
    TimerRuntime::Inputs in = buildInputs();
    TimerRuntime::State  st{state, remainingSec, lastPublishMs, enteredFinishedMs, lastRealertMs};
    TimerRuntime::Result r = TimerRuntime::step(st, now, in);

    // Tick-only bookkeeping the transition switch does not own: the Running branch
    // always advances remaining to the freshly-computed value and re-anchors the
    // publish throttle; the Finished branch advances the re-alert anchor. The phase
    // change itself (ToFinished / auto-clear ToIdle) is applied by applyTransition,
    // the same seam start/pause/reset/setDuration use.
    if (state == TimerState::Running)
    {
        remainingSec = in.newRemaining;
        if (r.publishFired) lastPublishMs = now;
    }
    else if (r.realertFired)   // Finished
    {
        lastRealertMs = now;
    }

    applyTransition(r, now, in);

    for (const TimerRuntime::Effect &e : r.effects) applyEffect(e);
}

TimerCmdResult TimerManager_::parseCommand(const char *json)
{
    if (!SHOW_TIMER) return TimerCmdResult::Disabled;
    if (json == nullptr || json[0] == '\0') return TimerCmdResult::BadJson;

    DynamicJsonDocument doc(kTimerCmdJsonSize);
    auto err = deserializeJson(doc, json);
    if (err)
    {
        if (DEBUG_MODE && err == DeserializationError::NoMemory)
            DEBUG_PRINTLN("timer: parseCommand NoMemory");
        return TimerCmdResult::BadJson;
    }

    // -- Validation: the whole atomic-reject pass runs once in the pure
    //    TimerCommand::classify, mutating nothing. The shell
    //    fills the Context from the globals it owns (the saved ceiling + _remoteApply)
    //    and drives apply from the returned Plan -- the packet is never re-read below. --
    TimerCommand::Plan plan =
        TimerCommand::classify(doc.as<JsonObjectConst>(),
                               TimerCommand::Context{TIMER_MAX_DURATION, _remoteApply});
    if (!plan.ok) return TimerCmdResult::BadField;   // first invalid field; nothing applied

    // -- Command is known-good: only now disturb device state. --
    // One-shot override: before applying, snapshot the saved config so
    // returnToIdle() can restore it. Only the first one-shot in a run captures (latest-
    // command-wins, single snapshot); a later one-shot applies on top of the same baseline.
    if (plan.oneShot && !_overrideActive)
    {
        captureSnapshot();
        _overrideActive = true;
    }

    // -- Apply, inside ONE PersistBatch window; its scope exit commits the whole config
    //    (both NVS namespaces, each at most once). Table rows FIRST, so a raised
    //    TIMER_MAX_DURATION lands before setDuration() re-clamps against the GLOBAL
    //    ceiling (a duration classify accepted against the staged
    //    ceiling would otherwise be silently clamped to the old one). Then duration
    //    (run-state, B1), then the member-backed applies via their publish-aware setters. --
    bool melodyChanged = false;
    {
        PersistBatch batch(*this);
        if (plan.oneShot) batch.setTransient();   // one-shot: scope exit writes nothing to flash
        for (size_t i = 0; i < TIMER_SETTINGS_DESC_COUNT; ++i)
        {
            if (!plan.tablePresent[i]) continue;
            const TimerSettingDesc &d = TIMER_SETTINGS_DESCS[i];
            if (timerSettingStore(d, plan.tableStaged[i])) batch.markTableDirty();
            if (strcmp(d.cmdKey, "melody_tick") == 0 || strcmp(d.cmdKey, "melody_end") == 0) melodyChanged = true;
        }
        if (plan.haveDuration) setDuration(plan.durationSec);   // pre-validated in range
        for (size_t i = 0; i < TIMER_MEMBER_CONFIG_DESC_COUNT; ++i)
            if (plan.memberPresent[i]) TIMER_MEMBER_CONFIG_DESCS[i].apply(plan.memberStaged[i]);
    }

    if (melodyChanged) loadMelodiesCached();
    // Inline melodies: use the validated tune directly as the resolved RAM,
    // after any bare-name re-resolve above so it wins. The saved name globals are
    // untouched (config mirror keeps the saved name); the snapshot captured the saved-
    // resolved RAM, so returnToIdle() reverts these on return to Idle.
    if (plan.haveInlineEnd)  endRtttl  = plan.inlineEnd;
    if (plan.haveInlineTick) tickRtttl = plan.inlineTick;

    // Rebaseline (story 21): a normal (save:true) config command arriving
    // during an active one-shot run commits the live config — including prior one-shot
    // values — as the new saved baseline and ends the override, so a later revert leaves
    // the promoted truth in place. A pure action/duration command does NOT rebaseline (it
    // carries no config to commit), so the override survives to revert. configInCommand
    // (table inSnapshot half OR member half; sync_* and run-state excluded) is computed
    // by classify.
    if (!plan.oneShot && _overrideActive && plan.configInCommand)
    {
        persist();        // member half: full live state -> "timer" NVS
        saveSettings();   // table half: full live state -> "awtrix" NVS
        _overrideActive = false;
    }

    // Settings projected as read-only HA attributes: republish each affected
    // carrier's bag when any of its mapped keys was in the command (classify computed the
    // dirty set). Fires on the remote-apply path too (not _remoteApply-gated), keeping
    // each synced peer's HA attributes consistent. Suppressed under a
    // one-shot command so the retained bags keep reporting the saved config.
    if (!plan.oneShot)
    {
        for (size_t c = 0; c < (size_t)TimerHaEntity::COUNT; ++c)
            if (plan.attrCarrierDirty[c]) publishAttributeGroup((TimerHaEntity)c);
    }

    // Run-state dispatch rides the runStateAction seam: the verb and its
    // peer mirror are paired there, so this path can't emit one without the other.
    // The propagation contract is the run-scoped config mirror: a `start` emits
    // the ONE combined packet (action + duration + effective config snapshot),
    // pause/reset propagate run-state only, and a config/bare-duration edit
    // propagates NOTHING; broadcastRunState itself no-ops under _remoteApply
    // (one-hop) and sync-off. The broadcast deliberately fires BEFORE the
    // switch-to-app below: the broadcast reads state/duration/effective config,
    // switchToApp touches display only.
    bool fromIdle = (state == TimerState::Idle);
    runStateAction(plan.action);
    if (plan.action == TimerCommand::Action::Start && fromIdle &&
        !GAME_ACTIVE && !BLOCK_NAVIGATION)
    {
        String j = "{\"name\":\"Timer\"}";
        DisplayManager.switchToApp(j.c_str());
    }

    return TimerCmdResult::Ok;
}

// HA control adapter: each HA timer callback re-enters the control
// surface through parseCommand, exactly as the propagation surface does, rather
// than poking a deep setter. The minimal JSON each entity builds is the SAME shape
// the {prefix}/timer MQTT topic accepts, so HA edits inherit atomic-reject
// validation, run-state propagation, and the per-enum codec spellings for free —
// nothing about duration parsing or enum encoding is duplicated in the HA layer.
TimerCmdResult TimerManager_::timerHaApply(TimerHaEntity entity, const String &rawValue)
{
    StaticJsonDocument<128> doc;
    switch (entity)
    {
    case TimerHaEntity::Buzzer:
    {
        // The select callback hands us the chosen option index; map it through the
        // per-enum codec so the emitted wire string is the one
        // parseCommand accepts — the two cannot drift.
        long idx = rawValue.toInt();
        if (idx < 0 || (size_t)idx >= TIMER_BUZZER_CODEC_COUNT) return TimerCmdResult::BadField;
        doc["buzzer"] = TIMER_BUZZER_CODEC[idx].wire;
        break;
    }
    case TimerHaEntity::Finished:
    {
        long idx = rawValue.toInt();
        if (idx < 0 || (size_t)idx >= TIMER_FINISHED_CODEC_COUNT) return TimerCmdResult::BadField;
        doc["finished"] = TIMER_FINISHED_CODEC[idx].wire;
        break;
    }
    case TimerHaEntity::Duration:
        // The raw HH:MM:SS text rides straight into parseCommand, which owns the
        // parse/validate (timerParseHMS + range, reject-not-clamp). On a non-Ok result
        // the caller echoes the canonical live value back (snap-back).
        doc["duration"] = rawValue;
        break;
    case TimerHaEntity::Start:  doc["action"] = "start"; break;
    case TimerHaEntity::Pause:  doc["action"] = "pause"; break;
    case TimerHaEntity::Reset:  doc["action"] = "reset"; break;
    case TimerHaEntity::SyncFollow:
        // The switch callback hands us the new bool ("1"/"0"); emit the strict
        // bool parseCommand's sync_follow validator (TcCheck::Bool) accepts. Local
        // identity (inSnapshot=false), so it persists but never propagates.
        doc["sync_follow"] = (rawValue.toInt() != 0);
        break;
    case TimerHaEntity::SyncTargets:
        // The dynamic select hands us the RESOLVED sync_targets value,
        // not an index: "" (Off), "all" (All), or a discovered peer id. MQTTManager
        // maps the chosen option index through the CURRENT id list before calling, so
        // the option set tracks the registry. The bespoke sync_targets validator
        // (parseSyncTargets) rejects a malformed id (atomic-reject parity). Local
        // identity (inSnapshot=false): persists but never propagates.
        doc["sync_targets"] = rawValue;
        break;
    default:
        return TimerCmdResult::BadField;   // not a control entity
    }

    String json;
    serializeJson(doc, json);
    return parseCommand(json.c_str());
}

void TimerManager_::onShowTimerChange(bool prev, bool now)
{
    if (prev && !now) reset();
}

// State is run-state, not a member-config row, so it goes through the wire
// seam directly: the exact (topic, payload) the broker receives, byte-identical
// to the retired HASensor::setValue path.
void TimerManager_::publishState()
{
    MQTTManager.publishTimerWire(MQTTManager.timerWireTopic(TimerHaEntity::State).c_str(),
                                 getStateString());
}
// Remaining is run-state too: straight through the seam, payload a
// plain decimal string — byte-identical to the retired HASensorNumber
// (PrecisionP0) setValue path.
void TimerManager_::publishRemaining()
{
    MQTTManager.publishTimerWire(MQTTManager.timerWireTopic(TimerHaEntity::Remaining).c_str(),
                                 String(remainingSec).c_str());
}
// Duration is run-state too: straight through the seam, payload the
// trimmed-HMS clock string — byte-identical to the retired HAText::setState path.
void TimerManager_::publishDuration()
{
    MQTTManager.publishTimerWire(MQTTManager.timerWireTopic(TimerHaEntity::Duration).c_str(),
                                 timerFormatHMS(durationSec).c_str());
}
// The enum keys are member-config rows: dispatch through the row's
// declared publish hook so validate/apply/emit/publish stay co-located and the
// table is the single definition of how each key goes out on the wire.
void TimerManager_::publishBuzzerMode()   { timerMemberConfigPublish("buzzer"); }
void TimerManager_::publishFinishedMode() { timerMemberConfigPublish("finished"); }

// A carrier's read-only JSON attribute object: the bag is
// built table-driven by timerBuildAttributeGroup, serialized, and ridden onto the
// carrier's json_attr_t topic by the wire seam — the same path every other Timer
// value takes. HA reads it because the TimerHaHost carrier build opted the carrier
// select into json attributes (setJsonAttributes), so the discovery config advertises
// this topic. Retained means HA repopulates after a restart for free. A carrier
// with no mapped rows yields an empty bag and publishes nothing.
void TimerManager_::publishAttributeGroup(TimerHaEntity carrier)
{
    // 512: the state sensor's bag is the largest (eight config-view keys incl.
    // two strings), which overflows 256 on a 64-bit host.
    DynamicJsonDocument doc(512);
    // A republish (e.g. on reconnect) during a one-shot override serializes the SAVED
    // config, so HA attribute bags never show transient one-off values.
    withConfigView(View::Saved, [&] { timerBuildAttributeGroup(carrier, doc); });
    if (doc.as<JsonObjectConst>().size() == 0) return;
    String payload;
    serializeJson(doc, payload);
    MQTTManager.publishTimerWire(MQTTManager.timerWireAttrTopic(carrier).c_str(), payload.c_str());
}

// Every distinct carrier's attribute object, each published once. Carriers are
// deduped by first appearance in the attribute-group table (a carrier owns
// several rows), mirroring publishAllWire's hook dedupe.
void TimerManager_::publishAllAttributeGroups()
{
    forEachDistinct(TIMER_ATTR_GROUP_DESC_COUNT,
        [](size_t i) { return TIMER_ATTR_GROUP_DESCS[i].carrier; },
        [this](TimerHaEntity carrier) { publishAttributeGroup(carrier); });
}

// Teardown mirror of publishAllAttributeGroups: empty the retained json_attr_t
// topic of every distinct carrier so disabling the Timer (discovery teardown)
// leaves no orphaned attribute object on the broker. Same carrier
// dedupe and same wire seam — an empty retained payload is the MQTT clear.
// publishTimerWire's creation-sentinel gate makes this no-op when no entity ever
// existed (nothing was advertised, so nothing to clear).
void TimerManager_::clearAllAttributeGroups()
{
    forEachDistinct(TIMER_ATTR_GROUP_DESC_COUNT,
        [](size_t i) { return TIMER_ATTR_GROUP_DESCS[i].carrier; },
        [](TimerHaEntity carrier) {
            MQTTManager.publishTimerWire(MQTTManager.timerWireAttrTopic(carrier).c_str(), "");
        });
}

// Full wire refresh. The run-state trio is a fixed
// set (state/remaining/duration are run-state, not table rows); the config half
// is DERIVED from TIMER_MEMBER_CONFIG_DESCS, so a row added with a publish hook
// is republished on connect / discovery-enable without touching this function.
// Hooks are deduped by pointer (forEachDistinct) — the four icon rows share one
// aggregate hook, whose JSON must hit the wire exactly once; the null-hook rows
// (key not individually published) collapse to one distinct null that the guard
// skips. Order preserved from the retired hand-listed blocks: duration, remaining,
// state, then table order.
void TimerManager_::publishAllWire()
{
    publishDuration();
    publishRemaining();
    publishState();
    forEachDistinct(TIMER_MEMBER_CONFIG_DESC_COUNT,
        [](size_t i) { return TIMER_MEMBER_CONFIG_DESCS[i].publish; },
        [](void (*hook)()) { if (hook) hook(); });
}

// ---------------------------------------------------------------------------
// Propagation surface (device-to-device timer sync). See docs/timer.md
// "Multi-device sync".
// ---------------------------------------------------------------------------

void TimerManager_::addSyncEnvelope(JsonObject &sync)
{
    // Thin forwarder: this class owns the monotonic _syncSeq counter (injected as
    // seq); the envelope shape + target-CSV parsing live in SyncEnvelope.
    SyncEnvelope::build(sync, uniqueID, ++_syncSeq, TIMER_SYNC_TARGETS);
}

void TimerManager_::buildConfigSnapshot(JsonDocument &doc, View view) const
{
    // Config block only — never action/duration (run-state) or sync_* (local identity,
    // inSnapshot=false). Two tables, one config block: TIMER_SETTINGS_DESCS' inSnapshot
    // rows and TIMER_MEMBER_CONFIG_DESCS (the member-backed half, B1).
    // Each table also feeds the parseCommand broadcast trigger, so the snapshot can't
    // drift from what fires a broadcast.
    // View::Saved masks an active one-shot override so the snapshot reports the SAVED
    // config — a follower never receives transient one-off values it has no notion of
    // reverting (broadcastConfig is itself suppressed during an
    // override, so this is also defensive). View::Effective takes live
    // storage as-is so a leader's own one-shot run mirrors to followers — the snapshot
    // reports what is actually running.
    withConfigView(view, [&] {
        timerSettingsBuildSnapshot(doc);
        timerMemberConfigBuildSnapshot(doc);
    });
}

void TimerManager_::broadcastRunState(const char *action)
{
    if (_remoteApply) return;                       // one-hop: never re-emit an applied remote command
    if (TIMER_SYNC_TARGETS.length() == 0) return;   // sync off

    // Only an action ever reaches here — start / pause / reset. A `start` is the
    // SOLE duration-bearing packet: it carries the leader's effective `duration` plus the
    // leader's EFFECTIVE config snapshot, bundled into one combined packet — the
    // run-scoped config mirror. Config no longer travels on a config edit, and
    // a bare duration edit propagates nothing; both ride one combined packet with the
    // start so a follower mirrors the leader for that run. The combined packet needs the
    // full kTimerCmdJsonSize buffer (config snapshot + envelope); pause/reset stay
    // run-state-only and fit a small static buffer.
    bool isStart = (strcasecmp(action, "start") == 0);

    if (isStart)
    {
        DynamicJsonDocument doc(kTimerCmdJsonSize);
        JsonObject sync = doc.createNestedObject("_sync");
        addSyncEnvelope(sync);
        doc["action"] = action;
        doc["duration"] = durationSec;   // duration rides only with a start (defines the countdown)
        // EFFECTIVE config: a leader's own one-shot run mirrors to followers, so the
        // snapshot reports what is actually running. sync_* are inSnapshot=false and
        // excluded by construction; inline melodies never travel (the saved bare name
        // globals back the snapshot).
        buildConfigSnapshot(doc, View::Effective);

        String out; serializeJson(doc, out);
        ServerManager.sendTimerSync(out);
        return;
    }

    // pause / reset: run-state only, no duration, no config.
    StaticJsonDocument<256> doc;
    JsonObject sync = doc.createNestedObject("_sync");
    addSyncEnvelope(sync);
    doc["action"] = action;

    String out; serializeJson(doc, out);
    ServerManager.sendTimerSync(out);
}

void TimerManager_::broadcastConfig()
{
    if (_remoteApply) return;
    if (TIMER_SYNC_TARGETS.length() == 0) return;

    DynamicJsonDocument doc(kTimerCmdJsonSize);
    JsonObject sync = doc.createNestedObject("_sync");
    addSyncEnvelope(sync);
    buildConfigSnapshot(doc, View::Saved);   // propagated config reports SAVED

    String out; serializeJson(doc, out);
    ServerManager.sendTimerSync(out);
}

void TimerManager_::applySyncCommand(const char *json)
{
    if (json == nullptr || json[0] == '\0') return;

    DynamicJsonDocument doc(kTimerCmdJsonSize);
    if (deserializeJson(doc, json)) return;

    // The echo/presence/follow/target decision is a PURE function (SyncEnvelope):
    // this shell only deserializes, then acts on the Decision. Dedup is
    // deliberately NOT in classify — SyncSeenCache::seen() is stateful (test-and-
    // record), so it stays here as the single guard before re-entry.
    SyncEnvelope::Decision d =
        SyncEnvelope::classify(doc.as<JsonVariantConst>(), {uniqueID, TIMER_SYNC_FOLLOW});

    switch (d.kind)
    {
    case SyncEnvelope::Decision::HarvestPresence:
        // Presence harvest: record the sender ungated, apply no
        // timer state. classify already bypassed the follow/target gate.
        _registry.record(d.src, millis());
        break;

    case SyncEnvelope::Decision::Apply:
        if (_seen.seen(d.src, d.seq, millis())) return;   // redundant copy of a burst
        // Re-enter the local control surface. The send-path _remoteApply guard
        // prevents re-broadcasting (one-hop); parseCommand ignores the _sync envelope.
        _remoteApply = true;
        parseCommand(json);
        _remoteApply = false;
        break;

    case SyncEnvelope::Decision::Ignore:
        break;
    }
}

// ---------------------------------------------------------------------------
// Peer presence beacon. The peer SET lives in PeerRegistry
// (extracted); this class keeps only the beacon cadence + UDP send.
// ---------------------------------------------------------------------------

void TimerManager_::broadcastPresence()
{
    // A small unconditional beacon: {_sync:{src,seq}, presence:true}. No tgt — it is
    // informational, harvested ungated by every receiver. Independent of sync targets
    // so even a clock that commands nobody is still discoverable. 256 matches the
    // pause/reset run-state beacon buffer (the src/seq envelope routes through
    // SyncEnvelope::build, which wants a touch more pool headroom on the 64-bit host).
    StaticJsonDocument<256> doc;
    JsonObject sync = doc.createNestedObject("_sync");
    SyncEnvelope::build(sync, uniqueID, ++_syncSeq);   // bare {src,seq}: no tgt (informational)
    doc["presence"] = true;

    String out; serializeJson(doc, out);
    ServerManager.sendTimerSync(out);   // also AP-gated at the transport (defense in depth)
}

void TimerManager_::tickPresence(unsigned long nowMs)
{
    _registry.prune(nowMs);

    if (AP_MODE) return;   // no beacon in AP mode (a standalone clock with no real LAN)

    if (!_presenceEverSent || (nowMs - _lastPresenceMs) >= kPresenceIntervalMs)
    {
        broadcastPresence();
        _lastPresenceMs   = nowMs;
        _presenceEverSent = true;
    }
}
#endif // AWTRIX_DISABLE_TIMER
