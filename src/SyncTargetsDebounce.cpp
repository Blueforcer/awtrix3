#ifndef AWTRIX_DISABLE_TIMER
#include "SyncTargetsDebounce.h"

// The settle-window state machine for the dynamic Targets select's republish
// decision. The transition table lives in the header; the effects
// live in TimerHaHost.

SyncTargetsDebounce::Action SyncTargetsDebounce::step(const char *currentOpts, unsigned long nowMs)
{
    // Row 1: no change against the published signature. Clearing the dirty flag
    // even mid-window is the revert-cancel semantic.
    if (_sig == currentOpts)
    {
        _dirty = false;
        return Action::Unchanged;
    }
    // Row 2: first sighting of the change — open the settle window. sinceMs stays
    // anchored here for the whole window (row 3 never rewrites it), so mid-window
    // drift does not push the republish out.
    if (!_dirty)
    {
        _dirty = true;
        _sinceMs = nowMs;
        return Action::StartWindow;
    }

    // Row 3: inside the window — the change has not settled yet. cur is compared
    // to the PUBLISHED signature (row 1 above), never the first-sighted value.
    if ((nowMs - _sinceMs) < kDebounceMs)
        return Action::Waiting;

    // Row 4: the change held for the whole window — adopt it in the same atomic
    // step, so the caller's publish and this signature cannot drift apart.
    _sig = currentOpts;
    _dirty = false;
    return Action::Republish;
}

void SyncTargetsDebounce::seed(const char *opts)
{
    _sig = opts;
    _dirty = false;
}
#endif // AWTRIX_DISABLE_TIMER
