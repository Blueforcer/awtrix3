#ifndef SyncSeenCache_h
#define SyncSeenCache_h

#include <Arduino.h>

// Timer-sync dedup set (extracted from TimerManager as a deliberate
// sibling of PeerRegistry).
//
// The set of *recently-applied sync commands*, keyed by (src, seq): the backing
// store for "apply each of the 3x redundant sends exactly once". Pure RAM,
// bounded, entries age out after the TTL — the same shape as PeerRegistry, but
// deliberately NOT merged under a shared generic (two 40-line classes are
// cheaper than one generic). It is simpler than PeerRegistry: there is no own-id (own echoes are
// dropped earlier in applySyncCommand), no sorted export, and no separate
// consumer query — the one call site test-and-records in a single atomic op.
//
// This module is the *set* only — check + record + age. It deliberately knows
// nothing about the UDP transport or the parseCommand re-entry; that I/O stays on
// TimerManager (the cut-line mirrors PeerRegistry's: the algorithm leaves, the
// transport stays). The method takes an injected `nowMs`, so there is no global
// read — the whole contract is testable in isolation, without standing up
// the TimerManager singleton.
class SyncSeenCache
{
public:
    // Test-and-record in one atomic op (there is exactly one call site, so a
    // separate check is pointless and racy). Returns true when (src,seq) was seen
    // within the TTL — a redundant copy the caller must drop — WITHOUT refreshing
    // the existing entry (first-seen ages out; the dedup semantic, opposite
    // PeerRegistry's keep-alive refresh). Otherwise records it and returns false.
    bool seen(const String &src, uint32_t seq, unsigned long nowMs);

    // Forget every entry (a reboot has applied nothing yet; pure RAM).
    void clear();

private:
    struct Entry { String src; uint32_t seq = 0; unsigned long atMs = 0; };
    static constexpr uint8_t       kMax   = 8;
    static constexpr unsigned long kTtlMs = 2000;

    Entry   _entries[kMax];
    uint8_t _writeIdx = 0;   // FIFO ring write cursor (round-robin eviction)
};

#endif
