#ifndef BeaconScanner_h
#define BeaconScanner_h

#include <Arduino.h>

class BeaconScanner_
{
private:
    BeaconScanner_() = default;

public:
    static BeaconScanner_ &getInstance();
    void setup();
    void startScan();
};

extern BeaconScanner_ &BeaconScanner;
#endif
