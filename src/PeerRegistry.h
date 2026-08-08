#ifndef PeerRegistry_h
#define PeerRegistry_h

#include <Arduino.h>

// Peer presence / peer registry (extracted).
//
// The set of *other clocks currently on the LAN*, keyed by the stable uniqueID
// (the targeting key; the mutable hostname is unsuitable). Pure RAM/LAN-derived
// state: bounded, the clock's own id excluded, entries age out after the TTL.
// Harvested UNGATED from inbound presence beacons by TimerManager (presence is
// informational, not a command) and consumed by the dynamic HA Targets select.
//
// This module is the *set* only — record / age / query. It deliberately knows
// nothing about beacon cadence, the UDP transport, or AP mode; that scheduling +
// I/O stays on TimerManager (one place owns "when do I announce myself"). The
// methods take an injected `nowMs` and the own-id is injected via setOwnId(), so
// there is no global read — the whole contract is testable in isolation,
// without standing up the TimerManager singleton.
class PeerRegistry
{
public:
    // The clock's own uniqueID, so record() can drop our own echoed beacons.
    // Set once from TimerManager::setup(); cheap to call again on reboot.
    void setOwnId(const String &id) { _ownId = id; }

    // Add a peer or refresh its lastSeen. Our own id and the full-registry case
    // are guarded: when full, the stalest slot is overwritten so a busy LAN keeps
    // the freshest peers rather than rejecting all new ones.
    void record(const String &src, unsigned long nowMs);

    // Drop entries not seen within kPeerTtlMs (~3 missed beacons).
    void prune(unsigned long nowMs);

    // Is `id` currently a known peer?
    bool has(const String &id) const;

    // Copy up to `cap` current peer uniqueIDs into out[], SORTED ascending, and
    // return the count written (<= min(count(), cap)). The sort makes a consumer's
    // option list stable regardless of discovery order. Never writes past cap.
    size_t ids(String *out, size_t cap) const;

    int count() const { return _count; }

    // Forget every peer (a reboot knows no peers; the set is repopulated by inbound
    // beacons). Own id is retained.
    void clear();

private:
    struct Peer { String uniqueID; unsigned long lastSeen = 0; };
    static constexpr uint8_t       kPeerMax   = 16;
    static constexpr unsigned long kPeerTtlMs = 100000;   // ~3 missed beacons

    Peer    _peers[kPeerMax];
    uint8_t _count = 0;
    String  _ownId;
};

#endif
