#ifndef MQTTManager_h
#define MQTTManager_h

#include <Arduino.h>
#include <map>

#ifndef AWTRIX_DISABLE_TIMER
#include "TimerHa.h"
#endif

// HA entity registration cap. ArduinoHA's HAMqtt::addDeviceType has an off-by-one
// (`_devicesTypesNb + 1 >= _maxDevicesTypesNb`), so the EFFECTIVE capacity is
// kMaxHAEntities - 1 (= 39 here). Inventory: 25 base entities (incl. battery on
// ulanzi) + 10 Timer entities (TIMER_HA_DESCRIPTOR_COUNT) = 35, leaving 4 spare
// slots. Raised from 34: at 34 the effective cap of 33 silently
// dropped the two Timer sync-control entities (the last to register). No clean
// compile-time guard — the base count is build-flag conditional — so the runtime
// guard is the DEBUG_MODE warning in TimerHaHost's carrier build via haRegistrationAtCap().
// Shared with TimerHaHost (the Timer carrier build reads it for that guard).
constexpr uint8_t kMaxHAEntities = 40;

class MQTTManager_
{
private:
    MQTTManager_() = default;

public:
    static MQTTManager_ &getInstance();

    void setup();
    void tick();
    void rawPublish(const char *prefix, const char *topic, const char *payload);
    void publish(const char *topic, const char *payload);
    void setCurrentApp(String);
    void sendStats();
    void sendButton(byte, bool);
    void setIndicatorState(uint8_t indicator, bool state, uint32_t color);
    void beginPublish(const char *topic, unsigned int plength, boolean retained);
    void writePayload(const char *data, const uint16_t length);
    void endPublish();
    bool subscribe(const char* topic);
    bool isConnected();
    String getValueForTopic(const String &topic);

    #ifndef AWTRIX_DISABLE_TIMER
    // The Timer wire seam: the single chokepoint through
    // which Timer MQTT output flows as (topic, payload) strings, published
    // retained (like the HA setValue path it replaces). timerWireTopic() sources
    // an entity's canonical data topic — byte-identical to what ArduinoHA emits.
    void publishTimerWire(const char *topic, const char *payload);
    String timerWireTopic(TimerHaEntity slot);
    String timerWireAttrTopic(TimerHaEntity slot);
    String timerIconsTopic();
    #endif
};

#ifndef AWTRIX_DISABLE_TIMER
void reconcileTimerHAState();
#endif

extern MQTTManager_ &MQTTManager;

#endif