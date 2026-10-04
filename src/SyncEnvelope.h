#ifndef SyncEnvelope_h
#define SyncEnvelope_h

#include <Arduino.h>
#include <ArduinoJson.h>

// Timer-sync wire envelope + inbound receive gate (extracted from
// TimerManager — the third such cut, after PeerRegistry and SyncSeenCache).
//
// Two halves, both stateless (free functions in a namespace — there is no set to
// own, unlike the two class siblings):
//
//   * classify() — the PURE receive policy. Given a parsed packet plus this
//     clock's {ownId, follow}, it returns the verdict: Ignore | HarvestPresence |
//     Apply. It reads no globals, no clock, and no dedup cache. The four gates it
//     owns (echo-drop / presence / follow consent / target match) mirror the old
//     inline gate in applySyncCommand exactly. The DEDUP step deliberately stays
//     OUT of classify: SyncSeenCache::seen() is test-and-record (stateful), so it
//     cannot live in a pure function — TimerManager runs it as the single stateful
//     guard before re-entry.
//
//   * build() — the send-side envelope constructor. The bare overload writes
//     {src,seq} (the presence beacon); the targeted overload also parses the
//     CSV / "all" target list into tgt. seq is INJECTED (TimerManager owns the
//     monotonic _syncSeq counter), so this half is stateless too.
//
// classify's context is {ownId, follow} ONLY — never this clock's own target
// list. A follower obeys on the SENDER's tgt + its OWN follow consent (the
// two-axis sync-roles invariant); the receiver's own send-target
// list has no role here. That list belongs to build() (the send axis).
//
// NOTE the wire-gate targetsMe() here is a DIFFERENT concept from TimerHa's
// timerSyncTargets* cluster, which maps the HA Targets *select* (Off/All/peer-id
// <-> option index). Same words, opposite surfaces; kept in separate files.
namespace SyncEnvelope
{
    struct Context
    {
        String ownId;   // this clock's uniqueID (echo-drop + targetsMe)
        bool   follow;  // this clock's receive-consent axis (TIMER_SYNC_FOLLOW)
    };

    struct Decision
    {
        enum Kind
        {
            Ignore,           // drop the packet, do nothing
            HarvestPresence,  // record `src` into the peer registry (ungated)
            Apply             // re-enter parseCommand under _remoteApply (after dedup)
        } kind;

        String   src;   // set for HarvestPresence + Apply
        uint32_t seq;   // set for Apply (the dedup key, with src)
    };

    // PURE receive policy. Order mirrors the old applySyncCommand gate:
    //   _sync null            -> Ignore (not a sync packet)
    //   src empty / == ownId  -> Ignore (malformed / own echo)
    //   presence == true      -> HarvestPresence{src}  (ungated, bypasses follow/target)
    //   !ctx.follow           -> Ignore (consent gate)
    //   !targetsMe(tgt,ownId) -> Ignore (not addressed to this clock)
    //   else                  -> Apply{src, seq}
    Decision classify(JsonVariantConst packet, const Context &ctx);

    // Does this packet's _sync.tgt cover ownId? Either the literal "all" string,
    // or membership in the tgt id array. Anything else is false.
    bool targetsMe(JsonVariantConst tgt, const String &ownId);

    // Send-side envelope construction into `sync` (a created "_sync" object).
    // Bare: just {src, seq} — the presence beacon. seq is injected.
    void build(JsonObject sync, const String &ownId, uint32_t seq);
    // Targeted: also writes tgt from the CSV/"all" target list. "all" -> "all"
    // (string); a CSV -> a tgt id array (empty/whitespace ids skipped).
    void build(JsonObject sync, const String &ownId, uint32_t seq, const String &targets);
}

#endif
