#ifndef PowerManager_h
#define PowerManager_h

#include <Arduino.h>

class PowerManager_
{
private:
    PowerManager_() = default;
public:
    static PowerManager_ &getInstance();
    void setup();
    void sleepParser(const char*);
    void sleep(uint32_t);
};

extern PowerManager_ &PowerManager;
#endif
