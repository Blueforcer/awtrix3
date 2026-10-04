#ifndef AWTRIX_DISABLE_TIMER
#include "SyncSeenCache.h"

// A bounded (src,seq,atMs) ring learned from inbound sync commands; the backend
// for "apply each of the 3x redundant sends exactly once". Entries age out past
// kTtlMs (so a sender reboot — seq restart — self-clears), and the existing entry
// is NOT refreshed on a hit (first-seen ages out; the dedup semantic).

bool SyncSeenCache::seen(const String &src, uint32_t seq, unsigned long nowMs)
{
    // A live, matching entry => redundant copy. Do NOT refresh it: first-seen ages
    // out (opposite PeerRegistry's keep-alive). atMs==0 marks an empty/never slot.
    for (uint8_t i = 0; i < kMax; ++i)
    {
        if (_entries[i].atMs != 0 && (nowMs - _entries[i].atMs) <= kTtlMs
            && _entries[i].seq == seq && _entries[i].src == src)
            return true;
    }

    // Record at the FIFO ring cursor (round-robin eviction at the bound). nowMs==0
    // would collide with the empty-slot sentinel, so it folds to 1.
    _entries[_writeIdx].src  = src;
    _entries[_writeIdx].seq  = seq;
    _entries[_writeIdx].atMs = (nowMs == 0) ? 1 : nowMs;
    _writeIdx = (uint8_t)((_writeIdx + 1) % kMax);
    return false;
}

void SyncSeenCache::clear()
{
    for (uint8_t i = 0; i < kMax; ++i)
    {
        _entries[i].src  = String();
        _entries[i].seq  = 0;
        _entries[i].atMs = 0;
    }
    _writeIdx = 0;
}
#endif // AWTRIX_DISABLE_TIMER
