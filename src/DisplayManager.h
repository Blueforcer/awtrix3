#ifndef DisplayManager_h
#define DisplayManager_h

#include <Arduino.h>
#include <LittleFS.h>

class DisplayManager_
{
private:
    // The pins the buttons are connected to

    DisplayManager_() = default;

    const int BatReadings = 10;
    uint16_t TotalBatReadings[10];
    int readIndex = 0;
    uint16_t total = 0;
    uint16_t average = 0;
    const int LDRReadings = 30;
    uint16_t TotalLDRReadings[30];
    int sampleIndex = 0;
    unsigned long previousMillis = 0;
    const unsigned long interval = 1000;

public:
    static DisplayManager_ &getInstance();
    void setup();
    void tick();
    void clear();
    void show();
    void applyAllSettings();
    void rightButton();
    void dismissNotify();
    void HSVtext(int16_t, int16_t, const char *, bool);
    void loadApps();
    void nextApp();
    void previousApp();
    void leftButton();
    void resetTextColor();
    void gererateTimer(String);
    void clearMatrix();
    void selectButton();
    void selectButtonLong();
    void setBrightness(uint8_t);
    void setTextColor(uint16_t color);
    void setFPS(uint8_t);
    void MatrixState(bool);
    void generateNotification(String);
    void generateCustomPage(String, String);
    void printText(int16_t x, int16_t y, const char *text, bool centered, bool ignoreUppercase);
    bool setAutoTransition(bool active);
    void switchToApp(String Payload);
    void setNewSettings(String Payload);
    void drawGIF(uint16_t x, uint16_t y, fs::File gifFile);
    void drawJPG(uint16_t x, uint16_t y, fs::File jpgFile);
    void drawProgressBar(int cur, int total);
    void drawMenuIndicator(int cur, int total);
};

extern DisplayManager_ &DisplayManager;

#endif