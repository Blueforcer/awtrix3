#ifndef TimerManager_h
#define TimerManager_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "TimerEnums.h"          // TimerState + BuzzerMode / FinishedMode + their codec tables
#include "TimerRuntime.h"        // pure run-state engine: step() returns effects this class applies
#include "TimerHa.h"             // TimerHaEntity (the HA carrier publishAttributeGroup targets)
#include "TimerSettings.h"       // TcValue + TIMER_SETTINGS_DESC_CAP (one-shot override snapshot)
#include "TimerCommand.h"        // TimerCommand::Action (the runStateAction verb)
#include "PeerRegistry.h"        // the LAN peer set (extracted from this class)
#include "SyncSeenCache.h"       // the sync dedup set (extracted from this class)

// Result of parseCommand. All control surfaces share one validation policy
// (reject invalid input atomically); only the HTTP API surfaces this as a
// status code — MQTT ignores it.
enum class TimerCmdResult : uint8_t { Ok = 0, BadJson = 1, BadField = 2, Disabled = 3 };

// Globals.cpp's full "awtrix"-namespace flush (declared in Globals.h, repeated
// here so the PersistBatch guard below can perform the table-half commit).
void saveSettings();

// The member-backed config half (B1) as one value, mirroring TIMER_MEMBER_CONFIG_DESCS
// ({buzzer, finished, the four icon_<state> names}) -- the singleton fields that are NOT
// driven by the generic TIMER_SETTINGS_DESCS table. It is exactly the set the one-shot
// override snapshots and SavedConfigScope swaps, so "swap == copy this struct" holds.
// Deliberately excludes durationSec (run-state) and the resolved melody RAM
// (endRtttl/tickRtttl -- the saved melody NAME is a table key); those revert with the
// override but are not config the observation carriers project as "saved".
// Number of TimerState values (Idle/Running/Paused/Finished) — the width of the
// state-indexed icon array. TimerState has no COUNT member; this is its stand-in.
static constexpr size_t kTimerStateCount = 4;

struct TimerMemberConfig
{
    // The explicit constructor keeps the two-value brace-init valid in C++11
    // (a class with default member initializers is not a C++11 aggregate).
    TimerMemberConfig() = default;
    TimerMemberConfig(BuzzerMode buzzer_, FinishedMode finished_)
        : buzzer(buzzer_), finished(finished_) {}

    BuzzerMode   buzzer   = BuzzerMode::End;
    FinishedMode finished = FinishedMode::AutoClear;
    String       iconByState[kTimerStateCount];   // indexed by TimerState
};

class TimerManager_
{
private:
    TimerManager_() = default;

    TimerState state = TimerState::Idle;
    BuzzerMode buzzerMode = BuzzerMode::End;
    FinishedMode finishedMode = FinishedMode::AutoClear;
    uint32_t durationSec = 300;
    uint32_t remainingSec = 300;

    String iconByState[kTimerStateCount];   // indexed by TimerState (Idle/Running/Paused/Finished)

    String endRtttl;
    String tickRtttl;

    unsigned long runStartMs = 0;
    uint32_t runStartRemainingSec = 0;
    uint32_t runDurationSec = 0;   // duration in force when the current run began; the progress-bar denominator (buffers a mid-run duration edit)
    unsigned long enteredFinishedMs = 0;
    unsigned long lastRealertMs = 0;
    unsigned long lastPublishMs = 0;

    bool _suspendPersist = false;
    // Member-backed RAM state differs from the "timer" NVS namespace: set by a
    // suspended persistIfDirty() inside a PersistBatch window, or by a deferred
    // (persist=false) enum edit during a TIMER-menu scroll session. Cleared by
    // the flush that writes it (persistIfDirty / PersistBatch scope exit).
    bool _dirty          = false;

    // -- Propagation surface (device-to-device timer sync) --
    // While true, an inbound sync packet is being applied via parseCommand; the
    // broadcast* methods early-return so a received command is never re-emitted
    // (one-hop topology).
    bool     _remoteApply = false;
    uint32_t _syncSeq     = 0;     // per-command sequence; only needs uniqueness within the dedup window

    // Emit one UDP broadcast mirroring a locally-accepted action to peers. No-ops
    // when sync is off (empty target list) or while applying an inbound packet
    // (_remoteApply). Private: every local run-state actor goes through
    // runStateAction(), so the verb+mirror pairing is not a caller obligation.
    void broadcastRunState(const char *action);

    // Bounded recently-seen (src,seq) dedup set so the 3x redundant send is applied
    // once. Extracted to its own self-contained module (SyncSeenCache); the
    // UDP transport + parseCommand re-entry stay here. TTL-based, so a sender reboot
    // (seq restart) self-clears by ageing out.
    SyncSeenCache _seen;

    // -- Peer presence registry (extracted to PeerRegistry) --
    // The LAN peer set lives in its own self-contained module (PeerRegistry); this
    // class keeps only the beacon cadence + UDP send. Harvested UNGATED from
    // inbound presence beacons (presence is informational, not a command — it
    // bypasses the follow/target gate and applies no timer state).
    static constexpr unsigned long kPresenceIntervalMs = 30000;   // beacon cadence
    PeerRegistry  _registry;
    unsigned long _lastPresenceMs    = 0;
    bool          _presenceEverSent  = false;
    void broadcastPresence();                                  // emit one {_sync,presence:true} beacon

    // -- One-shot override (save:false) --
    // A save:false command applies its config for the CURRENT RUN only: the saved
    // config is snapshotted, the command applies live, and returnToIdle() restores
    // the snapshot when the timer next returns to Idle (reset or auto-clear). While
    // an override is active no NVS write, no config broadcast and no HA config-
    // attribute republish occur. A normal (save:true) config command mid-override
    // promotes the live config to the new saved baseline and ends the override.
    bool             _overrideActive = false;
    TcValue          _snapTable[TIMER_SETTINGS_DESC_CAP];   // Family A (inSnapshot rows), generic capture
    TimerMemberConfig _snapMember;                          // Family B (member-backed half)
    uint32_t         _snapDuration   = 300;
    String           _snapEndRtttl, _snapTickRtttl;
    void captureSnapshot();   // record the saved config (both tables + duration + melody RAM)
    void restoreSnapshot();   // write the snapshot back (no publish/persist side effects)
    void returnToIdle();      // revert seam shared by reset() and the tick auto-clear transition

    // The only two places that name the member-backed config fields: read the live
    // singleton state into a value (snapshot), and write a value back RAW -- no persist,
    // no publish, no broadcast -- as the override revert + the honest-observation swap
    // both require. Used by captureSnapshot/restoreSnapshot and SavedConfigScope.
    TimerMemberConfig snapshotMemberConfig() const;
    void              restoreMemberConfig(const TimerMemberConfig &c);

    // Honest observation carriers. RAII: while an override is active,
    // present the SAVED config block (table inSnapshot rows + member-backed half) in
    // live storage so a carrier projection reads saved values, then restore the
    // effective (one-shot) values on scope exit. No-op when no override is active.
    // The const_cast is sound — the singleton is non-const and the swap is fully
    // reverted, so wrapping a const projection method stays observably const. Run-state
    // (duration) and sync_* (inSnapshot=false) are intentionally untouched.
    class SavedConfigScope
    {
    public:
        // enable=false forces a no-op even under an active override (View::Effective).
        explicit SavedConfigScope(const TimerManager_ &t, bool enable = true);
        ~SavedConfigScope();
        SavedConfigScope(const SavedConfigScope &) = delete;
        SavedConfigScope &operator=(const SavedConfigScope &) = delete;
    private:
        TimerManager_    &tm;
        bool              active;
        TcValue           effTable[TIMER_SETTINGS_DESC_CAP];
        TimerMemberConfig effMember;   // the effective (one-shot) member half, restored on scope exit
    };

    // Which config a serializer reports: Saved opens a SavedConfigScope so an active
    // one-shot override is masked (broadcastConfig); Effective takes live
    // storage as-is so a leader's own one-shot run mirrors to followers.
    enum class View { Saved, Effective };

    // The one seam every config-honesty serializer names its view through: runs
    // `serialize` with live storage swapped to `view` (View::Saved opens a
    // SavedConfigScope masking the one-shot override; View::Effective is a no-op).
    // Callers (GET mirror, HA bags, propagated snapshot) must name a View to compile,
    // so no path silently omits the scope.
    template <typename Serialize>
    void withConfigView(View view, Serialize &&serialize) const
    {
        SavedConfigScope saved(*this, view == View::Saved);
        serialize();
    }
    void buildConfigSnapshot(JsonDocument &doc, View view) const;   // config keys only; no action/duration/sync_*
    void addSyncEnvelope(JsonObject &sync);              // forwards to SyncEnvelope::build (injects _syncSeq)

    uint32_t computeCurrentRemaining() const;
    TimerRuntime::Inputs buildInputs() const;           // resolve engine inputs from the environment
    void runCommand(TimerRuntime::Command cmd);         // start/pause/reset/setDuration adapter: step() + apply
    void applyTransition(const TimerRuntime::Result &r, unsigned long now,
                         const TimerRuntime::Inputs &in);   // run-state bookkeeping for a returned Transition
    void applyEffect(const TimerRuntime::Effect &e);    // the effects-adapter seam
    void persist();
    void persistIfDirty();
    void loadMelodiesCached();

    static String validateIconName(const String &name);

public:
    // RAII guard that makes the persist-batching window a visible lexical scope
    // — the ONE commit seam shared by both batch call sites:
    // parseCommand's apply block and the TIMER menu's long-press commit
    // (MenuManager). While the guard lives, member-backed persistence is
    // suspended; scope exit commits the whole Timer config, flushing both NVS
    // namespaces at most once each: the member-backed "timer" namespace iff RAM
    // holds uncommitted member state (a setter dirtied it inside the window, or
    // deferred persist=false menu edits dirtied it beforehand), then the
    // table-backed "awtrix" namespace (saveSettings) iff markTableDirty() was
    // called. Exceptions are off on this target, so "scope exit" means the
    // normal return paths.
    class PersistBatch
    {
    public:
        explicit PersistBatch(TimerManager_ &tm) : tm(tm)
        {
            tm._suspendPersist = true;
        }
        ~PersistBatch()
        {
            tm._suspendPersist = false;
            // Transient (one-shot, save:false) window: scope exit writes NOTHING to
            // flash. RAM intentionally diverges from NVS for the duration of the run
            // (returnToIdle() restores it), so the pending member-half dirty flag is
            // discarded rather than flushed, and the table half is left untouched.
            if (transient)
            {
                tm._dirty = false;
                return;
            }
            if (tm._dirty)
            {
                tm._dirty = false;
                tm.persist();
            }
            if (tableDirty)
                saveSettings();
        }
        // A table-backed ("awtrix"-namespace) value changed inside the window.
        void markTableDirty() { tableDirty = true; }
        // Mark this window one-shot: scope exit skips both NVS flushes.
        void setTransient() { transient = true; }

        PersistBatch(const PersistBatch &) = delete;
        PersistBatch &operator=(const PersistBatch &) = delete;

    private:
        TimerManager_ &tm;
        bool tableDirty = false;
        bool transient  = false;
    };

    static TimerManager_ &getInstance();
    void setup();
    void tick();

    void start();
    void pause();
    void reset();

    // The run-state seam: dispatch the verb (start/pause/reset)
    // AND mirror it to peers via broadcastRunState with the matching action string —
    // the pairing every accepted action must make, folded into one entry so a call
    // site can't emit the verb without the mirror. Action::None is a no-op. Safe on
    // every path: broadcastRunState self-no-ops under _remoteApply and sync-off.
    void runStateAction(TimerCommand::Action a);

    void setDuration(uint32_t seconds);
    // persist=false applies + publishes live but defers the NVS write (the TIMER
    // menu's deferred-to-commit path; mirrors setIcon*'s publish flag). Every other
    // caller uses the default and persists immediately.
    void setBuzzerMode(BuzzerMode m, bool persist = true);
    void setFinishedMode(FinishedMode m, bool persist = true);

    // The parse/validate/format statics that once sat here are gone: deleted or
    // relocated into the descriptor-table free functions (    // TimerSettings.h; see docs/timer.md) — those are the only spellings.

    // The one shared icon setter: validate/reject/equality-skip/assign/persist/publish
    // for one state's slot. The four named setters below are thin delegators over it.
    void setIcon(TimerState s, const String &name, bool publish = true);

    void setIconIdle    (const String &name, bool publish = true) { setIcon(TimerState::Idle,     name, publish); }
    void setIconRunning (const String &name, bool publish = true) { setIcon(TimerState::Running,  name, publish); }
    void setIconPaused  (const String &name, bool publish = true) { setIcon(TimerState::Paused,   name, publish); }
    void setIconFinished(const String &name, bool publish = true) { setIcon(TimerState::Finished, name, publish); }

    void publishIcons();

    // Republish the current run-state string through the wire seam.
    // Public because MQTTManager re-emits it on connect / discovery enable; it
    // is the only path that puts the state key on the wire.
    void publishState();

    // Same for the remaining-seconds key: the only path that puts
    // remaining on the wire; the periodic republish throttle stays in tick().
    void publishRemaining();

    // Same for the duration key: the only path that puts the
    // trimmed-HMS duration on the wire. Public for the same connect /
    // discovery-enable republish sites.
    void publishDuration();

    // Same for the two enum keys, dispatching through their member-
    // config rows' publish hooks: the only paths that put buzzer/finished on
    // the wire. Public for the same connect / discovery-enable republish sites.
    void publishBuzzerMode();
    void publishFinishedMode();

    // Full wire refresh: republish every Timer wire artifact once —
    // the run-state trio plus every member-config row's publish hook, derived
    // from TIMER_MEMBER_CONFIG_DESCS so a new published row cannot be skipped.
    // The single call the connect / discovery-enable republish sites make.
    void publishAllWire();

    // Publish one HA carrier's read-only JSON attribute object — the carrier's
    // mapped settings keys built via timerBuildAttributeGroup — onto its
    // json_attr_t topic via the wire seam. Retained, so HA repopulates
    // after a restart for free. No-op for a carrier with no mapped keys.
    void publishAttributeGroup(TimerHaEntity carrier);

    // Publish every distinct carrier's attribute object once. The single call the
    // discovery-enable / reconnect paths make right after publishAllWire(), so HA
    // never sees an entity with missing attributes.
    void publishAllAttributeGroups();

    // Clear (empty retained payload) every distinct carrier's json_attr_t topic —
    // the teardown mirror of publishAllAttributeGroups(). The discovery teardown
    // (TimerHaHost::remove, SHOW_TIMER true->false) calls this so disabling the
    // Timer leaves no orphaned attribute object retained on the broker.
    void clearAllAttributeGroups();

    TimerCmdResult parseCommand(const char *json);

    // -- Home Assistant control adapter --
    // Route a single HA timer callback through parseCommand instead of a deep
    // setter, so HA edits get the SAME atomic-reject validation, the same
    // propagation, and the same codec strings as the {prefix}/timer MQTT surface.
    // Each entity builds the minimal JSON command it represents and hands it to
    // parseCommand, mirroring the sync receive path. Display-free (no ArduinoHA,
    // no MQTT client), so the HA->parseCommand path has no device dependency.
    //
    // rawValue per entity:
    //   * Buzzer / Finished : the selected select-option INDEX as a decimal
    //     string; mapped through the per-enum codec to the canonical
    //     wire spelling, so emitted and accepted JSON cannot drift.
    //   * Duration          : the raw HH:MM:SS text; parseCommand owns the
    //     parse/validate (timerParseHMS + range), so that logic is NOT duplicated here.
    //   * Start/Pause/Reset : ignored; the entity selects the action.
    // Returns parseCommand's result so the caller can echo the canonical live
    // value back on a non-Ok result (snap-back to the last valid value).
    TimerCmdResult timerHaApply(TimerHaEntity entity, const String &rawValue);

    // -- Propagation surface --
    // Run-state and config travel on separate packets; broadcastRunState is private
    // (the runStateAction seam owns the verb+mirror pairing).
    void broadcastConfig();
    // Validate, gate (echo/follow/target/dedup), then apply an inbound sync packet
    // through parseCommand under the _remoteApply guard. A presence beacon
    // (presence:true) is harvested into the peer registry UNGATED and short-circuits
    // before any command path (it carries no action/config and changes no state).
    void applySyncCommand(const char *json);

    // Peer presence. Called from the device loop with the current
    // millis(): emits a presence beacon at most once per kPresenceIntervalMs when on
    // a real network (NEVER in AP mode — broadcasts unconditionally otherwise so a
    // standalone clock is still discoverable), and ages out stale peers each call.
    void tickPresence(unsigned long nowMs);
    // Peer registry observers (consumed by the dynamic HA Targets select).
    // Thin forwarders onto the extracted PeerRegistry, so consumers (MQTTManager)
    // are unchanged by the extraction.
    int  peerCount() const { return _registry.count(); }
    bool hasPeer(const String &id) const { return _registry.has(id); }
    size_t peerIds(String *out, size_t cap) const { return _registry.ids(out, cap); }

    void onShowTimerChange(bool prev, bool now);

    TimerState   getState()        const { return state; }
    uint32_t     getRemaining()    const { return remainingSec; }
    uint32_t     getDuration()     const { return durationSec; }
    uint32_t     getRunDuration()  const { return runDurationSec; }
    BuzzerMode   getBuzzerMode()   const { return buzzerMode; }
    FinishedMode getFinishedMode() const { return finishedMode; }

    // The raw per-state slot (no Idle fallback — that lives in getIconForState).
    // The one indexed read the snapshot/emit hook uses; the four named getters
    // delegate here, mirroring how setIcon unified the setters.
    const String &getIcon(TimerState s) const { return iconByState[(size_t)s]; }
    const String &getIconIdle()     const { return getIcon(TimerState::Idle); }
    const String &getIconRunning()  const { return getIcon(TimerState::Running); }
    const String &getIconPaused()   const { return getIcon(TimerState::Paused); }
    const String &getIconFinished() const { return getIcon(TimerState::Finished); }
    const String &getIconForState(TimerState s) const;

    const char *getStateString() const;

    // Canonical output spellings for the timer enums, co-located with
    // getStateString() so the one true spelling of each enum lives in one place.
    // (The command parser additionally tolerates non-hyphen aliases on input.)
    // Public so the member-config table's emit hooks (TimerSettings.cpp) can read
    // them when building the propagated config snapshot.
    const char *buzzerModeString() const;
    const char *finishedModeString() const;

    // Live read-only snapshot for the GET /api/timer observation surface.
    // Reports computeCurrentRemaining() (wall-clock fresh), not the throttled
    // cached value. See docs/api.md.
    String getStateJson() const;
};

extern TimerManager_ &TimerManager;

#endif
