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
    void setCurrentApp(String value);
    void sendStats();
};

extern MQTTManager_ &MQTTManager;

#endif