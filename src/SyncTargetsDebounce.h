#ifndef SyncTargetsDebounce_h
#define SyncTargetsDebounce_h

#include <Arduino.h>

// The Targets-select republish debounce decision, carved out
// of TimerHaHost::refreshTargets as a stateful, isolated module — the
// SyncSeenCache sibling shape: injected `nowMs`, no globals, no clock, no
// ArduinoHA. A peer-membership change must hold for the settle window before the
// Targets select's options are republished, so a burst of beacon churn yields one
// republish; this class is that decision ONLY. The effects (rebuild options, publish
// discovery config, re-apply state) and the shell guards (carrier nullptr, MQTT
// connected) stay in TimerHaHost, BEFORE step() — so window state ages across
// MQTT-down gaps and an expired window republishes on the first connected tick.
//
// Transition table (byte-for-byte the TimerHaHost debounce it was carved
// from). State: sig (last
// adopted options signature), dirty (settle window open), sinceMs (window start).
// Input per step: cur (current options), now.
//
// | # | Condition                               | Next state                    | Action      |
// |---|-----------------------------------------|-------------------------------|-------------|
// | 1 | cur == sig                              | dirty := false                | Unchanged   |
// | 2 | cur != sig, !dirty                      | dirty := true; sinceMs := now | StartWindow |
// | 3 | cur != sig, dirty, now - sinceMs < 3000 | (none)                        | Waiting     |
// | 4 | cur != sig, dirty, now - sinceMs >= 3000| sig := cur; dirty := false    | Republish   |
//
// Consequences the table encodes (each pinned by a test):
//  - Row 1 mid-window is revert-cancel: membership flipping back to the published
//    set cancels the pending republish.
//  - Row 3 compares cur to the PUBLISHED sig, never the first-sighted value, so
//    mid-window drift does NOT restart the window: A→B@t0, →C@t0+2s ⇒ one
//    Republish at t0+3s adopting C.
//  - seed() adopts and closes the window (no spurious first republish).
//  - (now - sinceMs) is unsigned wrap arithmetic, safe across millis() rollover.
//
// Consumed by TimerHaHost: refreshTargets() steps it, createCarriers() seeds it.
class SyncTargetsDebounce
{
public:
    enum class Action { Unchanged, StartWindow, Waiting, Republish };

    // Atomic step: compare currentOpts to the last-adopted signature and
    // advance the settle window. Adopts the signature internally on Republish.
    Action step(const char *currentOpts, unsigned long nowMs);

    // Baseline at carrier creation: adopt opts, close the window
    // (no spurious first republish).
    void seed(const char *opts);

private:
    String        _sig;
    bool          _dirty = false;
    unsigned long _sinceMs = 0;
    static constexpr unsigned long kDebounceMs = 3000;
};

#endif
