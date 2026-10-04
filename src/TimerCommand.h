#ifndef TimerCommand_h
#define TimerCommand_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "TimerSettings.h"   // the descriptor-table family classify validates against
#include "TimerHa.h"         // TimerHaEntity (the attr-carrier dirty set)

// The Timer command plan -- the PURE atomic-reject validation decision lifted out of
// the 244-line TimerManager::parseCommand, mirroring the
// SyncEnvelope sibling. classify() runs the entire validation pass once,
// mutating NOTHING, and returns a Plan that carries everything the impure shell needs
// to apply the command without re-reading the packet.
//
// classify reads NO globals: its only non-packet input is a Context filled by the
// shell from the globals it owns (the saved max_duration ceiling + the _remoteApply
// flag). That discipline -- mirroring SyncEnvelope's {ownId, follow} -- is what lets
// it link against the dependency-light descriptor-table family alone (no
// singleton), so any ceiling / remote scenario can be exercised as a plain
// Context value.
//
// The one-shot override STORE stays in TimerManager; classify computes only
// the oneShot DECISION. apply ordering matters: the shell must write the table rows
// (landing a raised max_duration) BEFORE calling setDuration(), which re-clamps against
// the GLOBAL ceiling -- classify validates a duration against the
// staged ceiling, so a too-early setDuration would silently re-clamp it.
namespace TimerCommand
{
    // The only non-packet input classify reads -- filled by the shell from the globals
    // it owns. savedMaxDuration is the current ceiling (TIMER_MAX_DURATION); a command
    // that also raises max_duration re-bases the ceiling for its own duration check.
    // remoteApply forces the command one-shot (a follower mirrors but never
    // persists).
    struct Context
    {
        uint32_t savedMaxDuration;
        bool     remoteApply;
    };

    // Parsed run-state verb. None = the command carried no `action` key.
    enum class Action : uint8_t { None, Start, Pause, Reset };

    // Everything apply needs, so the shell never re-reads the packet. On !ok the only
    // rejection classify produces is BadField (BadJson / Disabled are shell-level
    // guards before classify); the shell maps !ok -> TimerCmdResult::BadField. All
    // staging arrays are sized to the family's compile-time row caps.
    struct Plan
    {
        bool ok = false;

        // Table half (TIMER_SETTINGS_DESCS): staged values + presence. A present row
        // whose value was an inline melody is marked NOT present here (excluded from
        // the bare-name store) and surfaced via the inline* fields instead.
        TcValue tableStaged[TIMER_SETTINGS_DESC_CAP];
        bool    tablePresent[TIMER_SETTINGS_DESC_CAP] = {false};

        // Member half (TIMER_MEMBER_VALIDATORS, co-indexed with TIMER_MEMBER_CONFIG_DESCS).
        TcValue memberStaged[TIMER_MEMBER_CONFIG_DESC_CAP];
        bool    memberPresent[TIMER_MEMBER_CONFIG_DESC_CAP] = {false};

        // duration (run-state, member-backed): pre-validated in range against the
        // staged ceiling. The shell applies it via setDuration AFTER the table rows.
        bool     haveDuration = false;
        uint32_t durationSec  = 0;

        // Inline RTTTL melodies: validated tunes staged as RAM strings; the
        // shell assigns them to endRtttl/tickRtttl after the bare-name resolve. Always
        // one-shot (no persistable file form), so either forces oneShot.
        bool   haveInlineEnd  = false;
        bool   haveInlineTick = false;
        String inlineEnd;
        String inlineTick;

        Action action  = Action::None;   // the parsed run-state verb (no re-parse in apply)
        bool   oneShot = false;          // !save || inline melody || remoteApply
        bool   configInCommand = false;  // config-block key present (table inSnapshot OR member half)

        // Carriers whose mapped HA-attribute keys appear in the command -- the set the
        // shell republishes (suppressed under a one-shot command). Indexed by carrier.
        bool attrCarrierDirty[(size_t)TimerHaEntity::COUNT] = {false};
    };

    // Run the whole atomic-reject pass once, mutating nothing. On the first invalid
    // field returns {ok=false} having staged nothing the caller should apply. On
    // success returns {ok=true} with every field above populated.
    Plan classify(JsonObjectConst packet, const Context &ctx);
}

#endif
