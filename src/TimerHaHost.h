#ifndef TimerHaHost_h
#define TimerHaHost_h

#include <ArduinoHA.h>
#include "TimerHa.h"

// TimerHaHost owns the Timer's Home Assistant *carrier* lifecycle: the ten HA
// discovery entities (their pointers and resolved unique ids), their construction
// and teardown, the dedicated Timer duration text callback, and the Timer branches
// of the shared ArduinoHA select/switch/button callbacks (reached via tryHandle*).
// It is a device-bound adapter — it constructs real ArduinoHA objects against the
// HADevice/HAMqtt globals still owned by MQTTManager (reached via extern), exactly
// like MQTTManager.cpp. The pure builder half lives in TimerHa (topics, descriptors,
// options, haRegistrationAtCap).

struct TimerHaHost_
{
    // Resolve the carrier ids and (when SHOW_TIMER) construct/register the carriers.
    // Runs at the same point in HA setup as before, so the ArduinoHA registration
    // order — and therefore the entity-cap drop order — is unchanged.
    void setup();
    // Publish the Timer wire artifacts + attribute groups on MQTT (re)connect.
    // Also consumes the pending discovery-cleanup latch reconcile() set (see below).
    void onConnected();
    // Reconcile the SHOW_TIMER setting against its persisted last-seen value at boot,
    // before MQTT connects: if the timer was toggled off while powered down, latch a
    // one-shot discovery cleanup that onConnected() flushes once MQTT is up. Persists
    // the new last-seen value when it changed. Called from the device loop.
    void reconcile();
    // SHOW_TIMER false->true: create carriers if missing, publish discovery + values.
    void enable();
    // SHOW_TIMER true->false: prune discovery config and clear the retained attr bags.
    void remove();

    // Re-publish the dynamic Targets select's discovery when peer-registry membership
    // changes, debounced. Called every device loop; a no-op when the
    // carriers are absent or MQTT is down. See the debounce state in TimerHaHost.cpp.
    void refreshTargets(unsigned long nowMs);

    // Each returns true iff sender is a Timer carrier; when true the host performs the
    // route-through-timerHaApply + snap-back echo the shared callbacks did before.
    bool tryHandleSelect(HASelect *sender, int8_t index);
    bool tryHandleSwitch(bool state, HASwitch *sender);
    bool tryHandleButton(HAButton *sender);

    // Accessors for the Timer wire seam that deliberately stays in MQTTManager
    //: whether the carriers exist, and a carrier's resolved unique id.
    // They are the seam's surface, not a transition step.
    bool carriersReady() const;
    const char *entityId(TimerHaEntity slot) const;
};

extern TimerHaHost_ TimerHaHost;

#endif
