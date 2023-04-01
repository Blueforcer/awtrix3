#ifndef UPDATER_h
#define UPDATER_h

#include <Arduino.h>

class Updater_
{
private:
    Updater_() = default;
public:
    static Updater_ &getInstance();
    void setup();
    bool checkUpdate(bool); 
    void updateFirmware(); 
};

extern Updater_ &Updater;
#endif
