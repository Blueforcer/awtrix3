#include <PeripheryManager.h>
#include <melody_player.h>
#include <melody_factory.h>
#include "Globals.h"
#include "DisplayManager.h"
#include "MQTTManager.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <LightDependentResistor.h>
#include <MenuManager.h>

#ifdef ULANZI
// Pinouts für das ULANZI-Environment
#define BATTERY_PIN 34
#define BUZZER_PIN 15
#define LDR_PIN 35
#define BUTTON_UP_PIN 26
#define BUTTON_DOWN_PIN 14
#define BUTTON_SELECT_PIN 27
#else
// Pinouts für das WEMOS_D1_MINI32-Environment
#define BUZZER_PIN -1
#define LDR_PIN A0
#define BUTTON_UP_PIN D0
#define BUTTON_DOWN_PIN D4
#define BUTTON_SELECT_PIN D8
#endif

#ifdef ULANZI
Adafruit_SHT31 sht31;
#else
Adafruit_BME280 bme280;
#endif
EasyButton button_left(BUTTON_UP_PIN);
EasyButton button_right(BUTTON_DOWN_PIN);
EasyButton button_select(BUTTON_SELECT_PIN);
MelodyPlayer player(BUZZER_PIN, LOW);

#define USED_PHOTOCELL LightDependentResistor::GL5516

LightDependentResistor photocell(LDR_PIN,
                                 10000,
                                 USED_PHOTOCELL,
                                 10,
                                 10);

int readIndex = 0;
int sampleIndex = 0;
unsigned long previousMillis_BatTempHum = 0;
unsigned long previousMillis_LDR = 0;
const unsigned long interval_BatTempHum = 10000;
const unsigned long interval_LDR = 100;
int total = 0;
unsigned long startTime;

const int LDRReadings = 10;
int TotalLDRReadings[LDRReadings];
float sampleSum = 0.0;
float sampleAverage = 0.0;
float brightnessPercent = 0.0;

// The getter for the instantiated singleton instance
PeripheryManager_ &PeripheryManager_::getInstance()
{
    static PeripheryManager_ instance;
    return instance;
}

// Initialize the global shared instance
PeripheryManager_ &PeripheryManager = PeripheryManager.getInstance();

void left_button_pressed()
{
    DisplayManager.leftButton();
    MenuManager.leftButton();
}

void right_button_pressed()
{
    DisplayManager.rightButton();
    MenuManager.rightButton();
}

void select_button_pressed()
{
    DisplayManager.selectButton();
    MenuManager.selectButton();
}

void select_button_pressed_long()
{
    DisplayManager.selectButtonLong();
    MenuManager.selectButtonLong();
}

void select_button_tripple()
{
    if (MATRIX_OFF)
    {
        DisplayManager.MatrixState(true);
    }
    else
    {
        DisplayManager.MatrixState(false);
    }
}

void PeripheryManager_::playBootSound()
{
    if (!SOUND_ACTIVE)
        return;
    if (BOOT_SOUND == "")
    {
        const int nNotes = 6;
        String notes[nNotes] = {"E5", "C5", "G4", "E4", "G4", "C5"};
        const int timeUnit = 150;
        Melody melody = MelodyFactory.load("Bootsound", timeUnit, notes, nNotes);
        player.playAsync(melody);
    }
    else

    {
        playFromFile("/MELODIES/" + BOOT_SOUND + ".txt");
    }
}

void PeripheryManager_::stopSound()
{
    player.stop();
}

void PeripheryManager_::playFromFile(String file)
{
    if (!SOUND_ACTIVE)
        return;
    Melody melody = MelodyFactory.loadRtttlFile(file);
    player.playAsync(melody);
}

bool PeripheryManager_::isPlaying()
{
    return player.isPlaying();
}

void fistStart()
{
#ifdef ULANZI
    uint16_t ADCVALUE = analogRead(BATTERY_PIN);
    BATTERY_PERCENT = min((int)map(ADCVALUE, 490, 690, 0, 100), 100);
    BATTERY_RAW = ADCVALUE;
    sht31.readBoth(&CURRENT_TEMP, &CURRENT_HUM);
    CURRENT_TEMP -= 9.0;
#else
    CURRENT_TEMP = bme280.readTemperature();
    CURRENT_HUM = 0;
#endif

    uint16_t LDRVALUE = analogRead(LDR_PIN);
    brightnessPercent = LDRVALUE / 4095.0 * 100.0;
    int brightness = map(brightnessPercent, 0, 100, 10, 120);
    DisplayManager.setBrightness(brightness);
}

void PeripheryManager_::setup()
{
    startTime = millis();
    pinMode(LDR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    button_left.begin();
    button_right.begin();
    button_select.begin();
    button_left.onPressed(left_button_pressed);
    button_right.onPressed(right_button_pressed);
    button_select.onPressed(select_button_pressed);
    button_select.onPressedFor(1000, select_button_pressed_long);
    button_select.onSequence(2, 300, select_button_tripple);
    Wire.begin(21, 22);
#ifdef ULANZI
    sht31.begin(0x44);
#else
    bme280.begin();
#endif
    photocell.setPhotocellPositionOnGround(false);
    fistStart();
}

void PeripheryManager_::tick()
{

    MQTTManager.sendButton(0, button_left.read());
    MQTTManager.sendButton(1, button_select.read());
    MQTTManager.sendButton(2, button_right.read());

    unsigned long currentMillis_BatTempHum = millis();
    if (currentMillis_BatTempHum - previousMillis_BatTempHum >= interval_BatTempHum)
    {
        previousMillis_BatTempHum = currentMillis_BatTempHum;
#ifdef ULANZI
        uint16_t ADCVALUE = analogRead(BATTERY_PIN);
        BATTERY_PERCENT = min((int)map(ADCVALUE, 475, 665, 0, 100), 100);
        BATTERY_RAW = ADCVALUE;
        sht31.readBoth(&CURRENT_TEMP, &CURRENT_HUM);
        CURRENT_TEMP -= 9.0;
#else
        CURRENT_TEMP = bme280.readTemperature();
        CURRENT_HUM = bme280.readHumidity();
#endif
        checkAlarms();
        MQTTManager.sendStats();
    }

    unsigned long currentMillis_LDR = millis();
    if (currentMillis_LDR - previousMillis_LDR >= interval_LDR)
    {
        previousMillis_LDR = currentMillis_LDR;
        TotalLDRReadings[sampleIndex] = analogRead(LDR_PIN);

        sampleIndex = (sampleIndex + 1) % LDRReadings;
        sampleSum = 0.0;
        for (int i = 0; i < LDRReadings; i++)
        {
            sampleSum += TotalLDRReadings[i];
        }
        sampleAverage = sampleSum / (float)LDRReadings;
        LDR_RAW = sampleAverage;
        CURRENT_LUX = (roundf(photocell.getSmoothedLux() * 1000) / 1000);
        if (AUTO_BRIGHTNESS)
        {
            brightnessPercent = sampleAverage / 4095.0 * 100.0;
            BRIGHTNESS = map(brightnessPercent, 0, 100, 0, 255);
            int brightness = map(brightnessPercent, 0, 100, 10, 120);
            DisplayManager.setBrightness(brightness);
        }
    }
}

const int MIN_ALARM_INTERVAL = 60; // 1 Minute
time_t lastAlarmTime = 0;

void PeripheryManager_::checkAlarms()
{
    File file = LittleFS.open("/alarms.json", "r");
    if (!file)
    {
        return;
    }

    DynamicJsonDocument doc(file.size() * 1.33);
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println(F("Failed to read Alarm file"));
        return;
    }
    JsonArray alarms = doc["alarms"];
    file.close();

    time_t now1 = time(nullptr);
    struct tm *timeInfo;
    timeInfo = localtime(&now1);
    int currentHour = timeInfo->tm_hour;
    int currentMinute = timeInfo->tm_min;
    int currentDay = timeInfo->tm_wday - 1;

    for (JsonObject alarm : alarms)
    {
        int alarmHour = alarm["hour"];
        int alarmMinute = alarm["minute"];
        String alarmDays = alarm["days"];

        if (currentHour == alarmHour && currentMinute == alarmMinute && alarmDays.indexOf(String(currentDay)) != -1)
        {
            if (difftime(now1, lastAlarmTime) < MIN_ALARM_INTERVAL)
            {
                return;
            }

            ALARM_ACTIVE = true;
            lastAlarmTime = now1;

            if (alarm.containsKey("sound"))
            {
                ALARM_SOUND = alarm["sound"].as<String>();
            }
            else
            {
                ALARM_SOUND = "";
            }

            if (alarm.containsKey("snooze"))
            {
                SNOOZE_TIME = alarm["snooze"].as<uint8_t>();
            }
            else
            {
                SNOOZE_TIME = 0;
            }
        }
    }
}


const char *PeripheryManager_::readUptime()
{
    static char uptime[25]; // Make the array static to keep it from being destroyed when the function returns
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
    unsigned long uptimeSeconds = elapsedTime / 1000;
    unsigned long uptimeMinutes = uptimeSeconds / 60;
    unsigned long uptimeHours = uptimeMinutes / 60;
    unsigned long uptimeDays = uptimeHours / 24;
    unsigned long hours = uptimeHours % 24;
    unsigned long minutes = uptimeMinutes % 60;
    unsigned long seconds = uptimeSeconds % 60;
    sprintf(uptime, "P%dDT%dH%dM%dS", uptimeDays, hours, minutes, seconds);
    return uptime;
}