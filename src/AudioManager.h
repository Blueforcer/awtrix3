#ifndef AudioManager_h
#define AudioManager_h

#include <Arduino.h>

class AudioManager_
{
private:
    AudioManager_() = default;

public:
    static AudioManager_ &getInstance();
    void setup();
    void tick();
    uint8_t curGain = 15;
    String StreamInfo = "";
    bool isRunning;
    void increaseVol();
    void decreaseVol();
    void setGain(uint8_t);
    void playText(String text);
    String getNextRadioStation(bool playPrevious);
    void startRadioStation(String stationName);
    String getCurrentRadioStation();
    void prevStation();
    void nextStation();
    void stopPlay();
    int stationIndex;
};

extern AudioManager_ &AudioManager;

#endif
