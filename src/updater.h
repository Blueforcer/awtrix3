#ifndef UPDATER_h
#define UPDATER_h

#include <Arduino.h>
#include <EasyButton.h>
#ifdef ULANZI
    #include "Adafruit_SHT31.h"
#else
    #include "Adafruit_BME280.h"
    #include "SoftwareSerial.h"
    #include <DFMiniMp3.h>
#endif

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
