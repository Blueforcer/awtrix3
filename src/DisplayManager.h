#ifndef DisplayManager_h
#define DisplayManager_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>
#include <FastLED_NeoMatrix.h>


class DisplayManager_
{
private:
    // The pins the buttons are connected to

    DisplayManager_() = default;

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
    void loadCustomApps();
    void loadNativeApps();
    void nextApp();
    void previousApp();
    void checkNewYear();
    void leftButton();
    void resetTextColor();
    void clearMatrix();
    void selectButton();
    void selectButtonLong();
    void setBrightness(int);
    bool generateNotification(uint8_t source, const char *json);
    bool generateCustomPage(const String &name, JsonObject doc, bool preventSave);
    void printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase);
    void GradientText(int16_t x, int16_t y, const char *text, int color1, int color2, bool clear, byte textCase);
    bool setAutoTransition(bool active);
    bool switchToApp(const char *json);
    void setNewSettings(const char *json);
    void drawJPG(uint16_t x, uint16_t y, fs::File jpgFile);
    void drawJPG(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t  data_size);
    void drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor);
    void drawMenuIndicator(int cur, int total, uint32_t color);
    void drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
    void drawBarChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color, uint32_t barBG);
    void drawLineChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color);
    void updateAppVector(const char *json);
    void setMatrixLayout(int layout);
    void setAppTime(long duration);
    String getAppsAsJson();
    String getStats();
    String getSettings();
    void setPower(bool state);
    void powerStateParse(const char *json);
    void setIndicator1Color(uint32_t color);
    void setIndicator1State(bool state);
    void setIndicator2Color(uint32_t color);
    void setIndicator2State(bool state);
    void setIndicator3Color(uint32_t color);
    void setIndicator3State(bool state);
    void reorderApps(const String &jsonString);
    void gammaCorrection();
    bool indicatorParser(uint8_t indicator, const char *json);
    void showSleepAnimation();
    void sendAppLoop();
    void processDrawInstructions(int16_t x, int16_t y, String &drawInstructions);
    String ledsAsJson();
    String getAppsWithIcon();
    void startArtnet();
    bool parseCustomPage(const String &name, const char *json, bool preventSave);
    bool moodlight(const char *json);
    int *getLedColors();
    CRGB getPixelColor(int16_t x, int16_t y);
    CRGB *getLeds();
    void forceNextApp();
    String getEffectNames();
    String getTransitionNames();
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
    void drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
    void drawPixel(int16_t x0, int16_t y0, uint32_t color);
    void drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint32_t color);
    void matrixPrint(const char *str);
    void matrixPrint(char c);
    void matrixPrint(String str);
    void matrixPrint(char *str);
    void matrixPrint(char str[], size_t length);
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint32_t color);
    void matrixPrint(double number, uint8_t digits);
    void setCustomAppColors(uint32_t color);
};

extern DisplayManager_ &DisplayManager;

#endif