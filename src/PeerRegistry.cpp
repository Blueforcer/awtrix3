#ifndef AWTRIX_DISABLE_TIMER
#include "PeerRegistry.h"

// A bounded set of {uniqueID, lastSeen} learned from inbound presence beacons;
// the backend the dynamic HA Targets select consumes. Own id is never
// stored; entries age out past kPeerTtlMs.

void PeerRegistry::record(const String &src, unsigned long nowMs)
{
    if (src.length() == 0 || src == _ownId) return;   // never store our own id
    unsigned long seen = (nowMs == 0) ? 1 : nowMs;     // 0 doubles as "never"

    // Refresh an existing entry.
    for (uint8_t i = 0; i < _count; ++i)
    {
        if (_peers[i].uniqueID == src)
        {
            _peers[i].lastSeen = seen;
            return;
        }
    }

    // New peer: append while bounded; otherwise overwrite the stalest slot so a busy
    // LAN keeps the freshest peers rather than rejecting all new ones once full.
    if (_count < kPeerMax)
    {
        _peers[_count].uniqueID = src;
        _peers[_count].lastSeen = seen;
        ++_count;
        return;
    }
    uint8_t oldest = 0;
    for (uint8_t i = 1; i < _count; ++i)
        if (_peers[i].lastSeen < _peers[oldest].lastSeen) oldest = i;
    _peers[oldest].uniqueID = src;
    _peers[oldest].lastSeen = seen;
}

void PeerRegistry::prune(unsigned long nowMs)
{
    uint8_t w = 0;
    for (uint8_t i = 0; i < _count; ++i)
    {
        if ((nowMs - _peers[i].lastSeen) <= kPeerTtlMs)
        {
            if (w != i) _peers[w] = _peers[i];
            ++w;
        }
    }
    for (uint8_t i = w; i < _count; ++i) _peers[i].uniqueID = String();
    _count = w;
}

bool PeerRegistry::has(const String &id) const
{
    for (uint8_t i = 0; i < _count; ++i)
        if (_peers[i].uniqueID == id) return true;
    return false;
}

size_t PeerRegistry::ids(String *out, size_t cap) const
{
    // Sort the FULL set first, then copy the lowest `cap`, so a small buffer still
    // gets a stable prefix of the sorted list (not just the first-discovered ids).
    // The registry is tiny (kPeerMax ~16) so an O(n^2) selection sort on a local
    // copy is cheaper than dragging in <algorithm> and stays allocation-light.
    String sorted[kPeerMax];
    for (uint8_t i = 0; i < _count; ++i) sorted[i] = _peers[i].uniqueID;
    for (uint8_t i = 0; i + 1 < _count; ++i)
    {
        uint8_t lo = i;
        for (uint8_t j = i + 1; j < _count; ++j)
            if (sorted[j] < sorted[lo]) lo = j;
        if (lo != i) { String t = sorted[i]; sorted[i] = sorted[lo]; sorted[lo] = t; }
    }

    size_t n = 0;
    for (uint8_t i = 0; i < _count && n < cap; ++i) out[n++] = sorted[i];
    return n;
}

void PeerRegistry::clear()
{
    for (uint8_t i = 0; i < _count; ++i) _peers[i].uniqueID = String();
    _count = 0;
}
#endif // AWTRIX_DISABLE_TIMER
