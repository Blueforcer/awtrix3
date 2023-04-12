#ifndef MQTTManager_h
#define MQTTManager_h

#include <Arduino.h>

class MQTTManager_
{
private:
    MQTTManager_() = default;

public:
    static MQTTManager_ &getInstance();
    void setup();
    void tick();
    void publish(const char *topic, const char *payload);
    void setCurrentApp(String);
    void sendStats();
    void sendButton(byte, bool);
    void setIndicatorState(uint8_t, bool, uint16_t);
};

extern MQTTManager_ &MQTTManager;

#endif