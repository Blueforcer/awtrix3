#ifndef DisplayManager_h
#define DisplayManager_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>

class DisplayManager_
{
private:
    // The pins the buttons are connected to

    DisplayManager_() = default;

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
    static DisplayManager_ &getInstance();
    bool appIsSwitching;
    bool showGif;
    void setup();
    void tick();
    void clear();
    void show();
    void applyAllSettings();
    void rightButton();
    void dismissNotify();
    void HSVtext(int16_t, int16_t, const char *, bool, byte textCase);
    void loadNativeApps();
    void nextApp();
    void previousApp();
    void leftButton();
    void resetTextColor();
    void gererateTimer(String);
    void clearMatrix();
    void selectButton();
    void selectButtonLong();
    void setBrightness(int);
    void setTextColor(uint16_t color);
    void setFPS(uint8_t);
    bool generateNotification(uint8_t source,const char *json);
    bool generateCustomPage(const String &name, const char *json);
    void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase);
    bool setAutoTransition(bool active);
    bool switchToApp(const char *json);
    void setNewSettings(const char *json);
    void drawJPG(uint16_t x, uint16_t y, fs::File jpgFile);
    void drawProgressBar(int16_t x, int16_t y, int progress, uint16_t pColor, uint16_t pbColor);
    void drawMenuIndicator(int cur, int total, uint16_t color);
    void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
    void drawBarChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint16_t color);
    void drawLineChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint16_t color);
    void updateAppVector(const char *json);
    void setMatrixLayout(int layout);
    void setAppTime(long duration);
    String getAppsAsJson();
    String getStats();
    String getSettings();
    void setPower(bool state);
    void powerStateParse(const char *json);
    void setIndicator1Color(uint16_t color);
    void setIndicator1State(bool state);
    void setIndicator2Color(uint16_t color);
    void setIndicator2State(bool state);
    void setIndicator3Color(uint16_t color);
    void setIndicator3State(bool state);
    void reorderApps(const String &jsonString);
    void gammaCorrection();
    bool indicatorParser(uint8_t indicator, const char *json);
    void showSleepAnimation();
    void showCurtainEffect();
    void sendAppLoop();
    void processDrawInstructions(int16_t x, int16_t y, String &drawInstructions);
    String ledsAsJson();
    String getAppsWithIcon();
    void startArtnet();
    bool parseCustomPage(const String &name, const char *json);
    bool moodlight(const char *json);
};

extern DisplayManager_ &DisplayManager;

#endif