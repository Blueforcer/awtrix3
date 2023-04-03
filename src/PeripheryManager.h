#ifndef PeripheryManager_h
#define PeripheryManager_h

#include <Arduino.h>
#include <EasyButton.h>
#ifndef ULANZI
#define DFMINI_MP3_BOOT "1"
#define DFMINI_MP3_ALARM "2"
#define DFMINI_MP3_TIMER "2"
#define DFMINI_MP3_CLICK "5"
#define DFMINI_MP3_CLICK_ON "3"
#define DFMINI_MP3_ENTER "4"
#endif

class PeripheryManager_
{
private:
    PeripheryManager_() = default;
    void checkAlarms();
#ifdef ULANZI
    const int BatReadings = 10;
    uint16_t TotalBatReadings[10];
#endif
    int readIndex = 0;
    uint16_t total = 0;
    uint16_t average = 0;
    const int LDRReadings = 30;
    uint16_t TotalLDRReadings[30];
    int sampleIndex = 0;
    unsigned long previousMillis = 0;
    const unsigned long interval = 1000;

public:
    static PeripheryManager_ &getInstance();
    void setup();
    void tick();
    void playBootSound();
    void playFromFile(String file);
    bool isPlaying();
    void stopSound();
#ifndef ULANZI
    void setVolume(uint8_t);
#endif
    const char *readUptime();
};

extern PeripheryManager_ &PeripheryManager;
#endif
