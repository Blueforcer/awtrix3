#include <PeripheryManager.h>
#ifdef ULANZI
#include <melody_player.h>
#include <melody_factory.h>
#include "Adafruit_SHT31.h"
#else
#include "Adafruit_BME280.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_HTU21DF.h"
#include "SoftwareSerial.h"
#include <DFMiniMp3.h>
#endif
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
#define I2C_SCL_PIN 22
#define I2C_SDA_PIN 21
#else
// Pinouts für das WEMOS_D1_MINI32-Environment
#define LDR_PIN A0
#define BUTTON_UP_PIN D0
#define BUTTON_DOWN_PIN D8
#define BUTTON_SELECT_PIN D4
#define DFPLAYER_RX D7
#define DFPLAYER_TX D5
#define I2C_SCL_PIN D1
#define I2C_SDA_PIN D3
#endif

#ifdef ULANZI
Adafruit_SHT31 sht31;
#else
Adafruit_BME280 bme280;
Adafruit_BMP280 bmp280;
Adafruit_HTU21DF htu21df;
#endif

EasyButton button_left(BUTTON_UP_PIN);
EasyButton button_right(BUTTON_DOWN_PIN);
EasyButton button_select(BUTTON_SELECT_PIN);
#ifdef ULANZI
MelodyPlayer player(BUZZER_PIN, 1, LOW);
#else
class Mp3Notify
{
};
SoftwareSerial mySoftwareSerial(DFPLAYER_RX, DFPLAYER_TX); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> dfmp3(mySoftwareSerial);
#endif

#ifdef ULANZI
#define USED_PHOTOCELL LightDependentResistor::GL5516
#define PHOTOCELL_SERIES_RESISTOR 10000
#else
#define USED_PHOTOCELL LightDependentResistor::GL5528
#define PHOTOCELL_SERIES_RESISTOR 1000
#endif

LightDependentResistor photocell(LDR_PIN,
                                 PHOTOCELL_SERIES_RESISTOR,
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

const int LDRReadings = 1000;
int TotalLDRReadings[LDRReadings];
float sampleSum = 0.0;
float sampleAverage = 0.0;
float brightnessPercent = 0.0;
int lastBrightness = 0;

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
    if (!BLOCK_NAVIGATION)
    {
#ifndef ULANZI
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
#endif
        DisplayManager.leftButton();
        MenuManager.leftButton();
        DEBUG_PRINTLN(F("Left button clicked"));
    }
}

void right_button_pressed()
{
    if (!BLOCK_NAVIGATION)
    {
#ifndef ULANZI
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
#endif
        DisplayManager.rightButton();
        MenuManager.rightButton();
        DEBUG_PRINTLN(F("Right button clicked"));
    }
}

void select_button_pressed()
{
    if (!BLOCK_NAVIGATION)
    {
#ifndef ULANZI
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
#endif
        DisplayManager.selectButton();
        MenuManager.selectButton();
        DEBUG_PRINTLN(F("Select button clicked"));
    }
}

void select_button_pressed_long()
{
    if (AP_MODE)
    {
#ifndef ULANZI
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
        ++MATRIX_LAYOUT;
        if (MATRIX_LAYOUT < 0)
            MATRIX_LAYOUT = 2;
        saveSettings();
        ESP.restart();
#endif
    }
    else if (!BLOCK_NAVIGATION)
    {
        #ifndef ULANZI
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
        #endif
        if (!ALARM_ACTIVE)
            MenuManager.selectButtonLong();

        DisplayManager.selectButtonLong();

        DEBUG_PRINTLN(F("Select button pressed long"));
    }
}

void select_button_double()
{
    DEBUG_PRINTLN(F("Select button double pressed"));
    if (!BLOCK_NAVIGATION)
    {
#ifndef ULANZI
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
#endif
        if (MATRIX_OFF)
        {
            DisplayManager.setPower(true);
        }
        else
        {
            DisplayManager.setPower(false);
        }
    }
}

void PeripheryManager_::playBootSound()
{
    DEBUG_PRINTLN(F("Playing bootsound"));
    if (!SOUND_ACTIVE)
    {
        DEBUG_PRINTLN(F("Sound output disabled"));
        return;
    }

    if (BOOT_SOUND == "")
    {
#ifdef ULANZI
        // no standard sound
        const int nNotes = 6;
        String notes[nNotes] = {"E5", "C5", "G4", "E4", "G4", "C5"};
        const int timeUnit = 150;
        Melody melody = MelodyFactory.load("Bootsound", timeUnit, notes, nNotes);
        player.playAsync(melody);
#else
        playFromFile(DFMINI_MP3_BOOT);
#endif
    }
    else
    {
        playFromFile(BOOT_SOUND);
    }
}

void PeripheryManager_::stopSound()
{
#ifdef ULANZI
    player.stop();
#else
    dfmp3.stopAdvertisement();
    delay(50);
    dfmp3.stop();
#endif
}

#ifndef ULANZI
void PeripheryManager_::setVolume(uint8_t vol)
{
    uint8_t curVolume = dfmp3.getVolume(); // need to read volume in order to work. Donno why! :(
    dfmp3.setVolume(vol);
    delay(50);
}
#endif

bool PeripheryManager_::parseSound(const char *json)
{
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        return playFromFile(String(json));
    }
    if (doc.containsKey("sound"))
    {
        return playFromFile(doc["sound"].as<String>());
    }
    return false;
}

bool PeripheryManager_::playRTTTLString(String rtttl)
{
    #ifdef ULANZI
        Melody melody = MelodyFactory.loadRtttlString(rtttl.c_str());
        player.playAsync(melody);
        return melody.isValid();
    #else
        return false;
    #endif
}

bool PeripheryManager_::playFromFile(String file)
{
    if (!SOUND_ACTIVE)
        return true;

#ifdef ULANZI
    DEBUG_PRINTLN(F("Playing RTTTL sound file"));
    if (LittleFS.exists("/MELODIES/" + String(file) + ".txt"))
    {
        Melody melody = MelodyFactory.loadRtttlFile("/MELODIES/" + String(file) + ".txt");
        player.playAsync(melody);
        return true;
    }
    else
    {
        return false;
    }
#else
    DEBUG_PRINTLN(F("Playing MP3 file"));
    dfmp3.stop();
    delay(50);
    dfmp3.playMp3FolderTrack(file.toInt());
#endif
    return true;
}

bool PeripheryManager_::isPlaying()
{
#ifdef ULANZI
    return player.isPlaying();
#else
    if ((dfmp3.getStatus() & 0xff) == 0x01) // 0x01 = DfMp3_StatusState_Playing
        return true;
    else
        return false;
#endif
}

void PeripheryManager_::setup()
{
    DEBUG_PRINTLN(F("Setup periphery"));
    startTime = millis();
    pinMode(LDR_PIN, INPUT);
#ifndef ULANZI
    dfmp3.begin();
    delay(100);
    setVolume(VOLUME);
#endif
    button_left.begin();
    button_right.begin();
    button_select.begin();

    if (ROTATE_SCREEN)
    {
        Serial.println("Button rotation");
        button_left.onPressed(right_button_pressed);
        button_right.onPressed(left_button_pressed);
    }
    else
    {
        button_left.onPressed(left_button_pressed);
        button_right.onPressed(right_button_pressed);
    }

    button_select.onPressed(select_button_pressed);
    button_select.onPressedFor(1000, select_button_pressed_long);
    button_select.onSequence(2, 300, select_button_double);

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
#ifdef ULANZI
    sht31.begin(0x44);
#else
	if (bme280.begin(BME280_ADDRESS) || bme280.begin(BME280_ADDRESS_ALTERNATE))
	{
        DEBUG_PRINTLN(F("BME280 sensor detected"));
		TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_BME280;
	}
    else if (bmp280.begin(BMP280_ADDRESS) || bmp280.begin(BMP280_ADDRESS_ALT))
	{
        DEBUG_PRINTLN(F("BMP280 sensor detected"));
		TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_BMP280;
	}
    else if (htu21df.begin())
	{
        DEBUG_PRINTLN(F("HTU21DF sensor detected"));
		TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_HTU21DF;
	}
    dfmp3.begin();
#endif
    photocell.setPhotocellPositionOnGround(false);
}

void PeripheryManager_::tick()
{
    if (ROTATE_SCREEN)
    {
        MQTTManager.sendButton(2, button_left.read());
        MQTTManager.sendButton(0, button_right.read());
    }
    else
    {
        MQTTManager.sendButton(0, button_left.read());
        MQTTManager.sendButton(2, button_right.read());
    }

    MQTTManager.sendButton(1, button_select.read());
    unsigned long currentMillis_BatTempHum = millis();
    if (currentMillis_BatTempHum - previousMillis_BatTempHum >= interval_BatTempHum)
    {
        previousMillis_BatTempHum = currentMillis_BatTempHum;
#ifdef ULANZI
        uint16_t ADCVALUE = analogRead(BATTERY_PIN);
        BATTERY_PERCENT = max(min((int)map(ADCVALUE, 475, 665, 0, 100), 100), 0);
        BATTERY_RAW = ADCVALUE;
#endif
        if (SENSOR_READING)
        {
#ifdef ULANZI
            sht31.readBoth(&CURRENT_TEMP, &CURRENT_HUM);
#else
			switch (TEMP_SENSOR_TYPE)
			{
			case TEMP_SENSOR_TYPE_BME280:
                CURRENT_TEMP = bme280.readTemperature();
				CURRENT_HUM = bme280.readHumidity();
				break;
			case TEMP_SENSOR_TYPE_BMP280:
                CURRENT_TEMP = bmp280.readTemperature();
				CURRENT_HUM = 0;
				break;
			case TEMP_SENSOR_TYPE_HTU21DF:
                CURRENT_TEMP = htu21df.readTemperature();
                CURRENT_HUM = htu21df.readHumidity();
				break;
			default:
				CURRENT_TEMP = 0;
				CURRENT_HUM = 0;
				break;
			}
#endif
            CURRENT_TEMP += TEMP_OFFSET;
            CURRENT_HUM += HUM_OFFSET;
        }

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
        if (AUTO_BRIGHTNESS && !MATRIX_OFF)
        {
            brightnessPercent = sampleAverage / 4095.0 * 100.0;
            int brightness = map(brightnessPercent, 0, 100, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
            DisplayManager.setBrightness(brightness);
        }
    }
}

const int MIN_ALARM_INTERVAL = 60; // 1 Minute
time_t lastAlarmTime = 0;

void PeripheryManager_::checkAlarms()
{
    if (LittleFS.exists("/alarms.json"))
    {
        File file = LittleFS.open("/alarms.json", "r");
        DynamicJsonDocument doc(file.size() * 1.33);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            DEBUG_PRINTLN(F("Failed to read Alarm file"));
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
}

const char *PeripheryManager_::readUptime()
{
    static char uptime[25]; // Make the array static to keep it from being destroyed when the function returns
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
    unsigned long uptimeSeconds = elapsedTime / 1000;
    sprintf(uptime, "%lu", uptimeSeconds);
    return uptime;
}