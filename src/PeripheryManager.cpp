#include <PeripheryManager.h>
#include "Adafruit_SHT31.h"
#include "Adafruit_BME280.h"
#include "Adafruit_BMP280.h"
#include "Adafruit_HTU21DF.h"
#include "SoftwareSerial.h"
#include <DFMiniMp3.h>
#include <MelodyPlayer/melody_player.h>
#include <MelodyPlayer/melody_factory.h>
#include "Globals.h"
#include "DisplayManager.h"
#include "MQTTManager.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <LightDependentResistor.h>
#include <MenuManager.h>
#include <ServerManager.h>
#include <MedianFilterLib.h>
#include <MeanFilterLib.h>
const int buzzerPin = 2;       // Buzzer an GPIO2
const int baudRate = 50;       // Nachrichtenübertragungsrate
const char *message = "HELLO"; // Die Nachricht, die gesendet werden soll
#define LEDC_CHANNEL 0
#define LEDC_RESOLUTION 8 // 8 bit resolution
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define MEDIAN_WND 7 // A median filter window size of seven should be enough to filter out most spikes
#define MEAN_WND 7   // After filtering the spikes we don't need many samples anymore for the average

#define DFPLAYER_RX 23
#define DFPLAYER_TX 18
#define BUZZER_PIN 15
#define RESET_PIN 13

#ifdef awtrix2_upgrade
// Pinouts für das WEMOS_D1_MINI32-Environment
#define LDR_PIN A0
#define BUTTON_UP_PIN D0
#define BUTTON_DOWN_PIN D8
#define BUTTON_SELECT_PIN D4

#define I2C_SCL_PIN D1
#define I2C_SDA_PIN D3
#elif ESP32_S3
#define BATTERY_PIN 4
#define BUZZER_PIN 5
#define LDR_PIN 6
#define BUTTON_UP_PIN 7
#define BUTTON_DOWN_PIN 8
#define BUTTON_SELECT_PIN 10
#define I2C_SCL_PIN 10
#define I2C_SDA_PIN 11
#else
// Pinouts für das ULANZI-Environment
#define BATTERY_PIN 34

#define LDR_PIN 35
#define BUTTON_UP_PIN 26
#define BUTTON_DOWN_PIN 14
#define BUTTON_SELECT_PIN 27
#define I2C_SCL_PIN 22
#define I2C_SDA_PIN 21
#endif

Adafruit_BME280 bme280;
Adafruit_BMP280 bmp280;
Adafruit_HTU21DF htu21df;
Adafruit_SHT31 sht31;

#ifdef awtrix2_upgrade
#define USED_PHOTOCELL LightDependentResistor::GL5528
#define PHOTOCELL_SERIES_RESISTOR 1000
#else
#define USED_PHOTOCELL LightDependentResistor::GL5516
#define PHOTOCELL_SERIES_RESISTOR 10000
#endif

class Mp3Notify
{
};
SoftwareSerial mySoftwareSerial(DFPLAYER_RX, DFPLAYER_TX); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> dfmp3(mySoftwareSerial);

MelodyPlayer player(BUZZER_PIN, 1, LOW);

EasyButton button_left(BUTTON_UP_PIN);
EasyButton button_right(BUTTON_DOWN_PIN);
EasyButton button_select(BUTTON_SELECT_PIN);
EasyButton button_reset(RESET_PIN);

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

MedianFilter<uint16_t> medianFilterBatt(MEDIAN_WND);
MedianFilter<uint16_t> medianFilterLDR(MEDIAN_WND);
MeanFilter<uint16_t> meanFilterBatt(MEAN_WND);
MeanFilter<uint16_t> meanFilterLDR(MEAN_WND);

float brightnessPercent = 0.0;

PeripheryManager_::PeripheryManager_()
{
    this->buttonL = &button_left;
    this->buttonR = &button_right;
    this->buttonS = &button_select;
    this->buttonRST = &button_reset;
}

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
        if (DFPLAYER_ACTIVE)
            PeripheryManager.playFromFile(DFMINI_MP3_CLICK);

        DisplayManager.leftButton();
        MenuManager.leftButton();
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Left button clicked"));
    }
    else
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Left button clicked but blocked"));
    }
}

void right_button_pressed()
{
    if (!BLOCK_NAVIGATION)
    {
        if (DFPLAYER_ACTIVE)
            PeripheryManager.playFromFile(DFMINI_MP3_CLICK);

        DisplayManager.rightButton();
        MenuManager.rightButton();
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Right button clicked"));
    }
    else
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Right button clicked but blocked"));
    }
}

void select_button_pressed()
{
    if (!BLOCK_NAVIGATION)
    {
        if (DFPLAYER_ACTIVE)
            PeripheryManager.playFromFile(DFMINI_MP3_CLICK);

        DisplayManager.selectButton();
        MenuManager.selectButton();
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Select button clicked"));
    }
    else
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Select button clicked but blocked"));
    }
}

void reset_button_pressed_long()
{
    ServerManager.erase();
    ESP.restart();
}

void select_button_pressed_long()
{
    if (DFPLAYER_ACTIVE)
        PeripheryManager.playFromFile(DFMINI_MP3_CLICK);
    if (AP_MODE)
    {
        ++MATRIX_LAYOUT;
        if (MATRIX_LAYOUT < 0)
            MATRIX_LAYOUT = 2;
        saveSettings();
        ESP.restart();
    }
    else if (!BLOCK_NAVIGATION)
    {
        MenuManager.selectButtonLong();
        DisplayManager.selectButtonLong();
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Select button pressed long"));
    }
}

void select_button_double()
{
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Select button double pressed"));
    if (!BLOCK_NAVIGATION)
    {
        if (DFPLAYER_ACTIVE)
            PeripheryManager.playFromFile(DFMINI_MP3_CLICK);

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
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Playing bootsound"));
    if (!SOUND_ACTIVE)
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Sound output disabled"));
        return;
    }

    if (BOOT_SOUND == "")
    {
        if (DFPLAYER_ACTIVE)
        {
            playFromFile(DFMINI_MP3_BOOT);
        }
        else
        {
            const int nNotes = 6;
            String notes[nNotes] = {"E5", "C5", "G4", "E4", "G4", "C5"};
            const int timeUnit = 150;
            Melody melody = MelodyFactory.load("Bootsound", timeUnit, notes, nNotes);
            player.playAsync(melody);
        }
    }
    else
    {
        playFromFile(BOOT_SOUND);
    }
}

void PeripheryManager_::stopSound()
{
    if (DFPLAYER_ACTIVE)
    {
        dfmp3.stopAdvertisement();
        delay(50);
        dfmp3.stop();
    }
    else
    {
        player.stop();
    }
}

void PeripheryManager_::setVolume(uint8_t vol)
{
    if (DFPLAYER_ACTIVE)
    {
        uint8_t curVolume = dfmp3.getVolume(); // need to read volume in order to work. Donno why! :(
        dfmp3.setVolume(vol);
        delay(50);
    }
    else
    {
        int scaledVol = (vol * 255) / 30;
        player.setVolume(scaledVol);
    }
}

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

const char *PeripheryManager_::playRTTTLString(String rtttl)
{
    if (!DFPLAYER_ACTIVE)
    {
        static char melodyName[64];
        Melody melody = MelodyFactory.loadRtttlString(rtttl.c_str());
        player.playAsync(melody);
        strncpy(melodyName, melody.getTitle().c_str(), sizeof(melodyName));
        melodyName[sizeof(melodyName) - 1] = '\0';
        return melodyName;
    }
    return nullptr; // RTTTL not supported with DFPlayer
}

const char *PeripheryManager_::playFromFile(String file)
{
    if (!SOUND_ACTIVE)
        return "";

    if (DFPLAYER_ACTIVE)
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Playing MP3 file"));
        if (!DFPLAYER_ACTIVE)
            return NULL;
        dfmp3.stop();
        delay(50);
        dfmp3.playMp3FolderTrack(file.toInt());

        return file.c_str();
    }
    else
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("Playing RTTTL sound file"));
        if (LittleFS.exists("/MELODIES/" + String(file) + ".txt"))
        {
            static char melodyName[64];
            Melody melody = MelodyFactory.loadRtttlFile("/MELODIES/" + String(file) + ".txt");
            player.playAsync(melody);
            strncpy(melodyName, melody.getTitle().c_str(), sizeof(melodyName));
            melodyName[sizeof(melodyName) - 1] = '\0';
            return melodyName;
        }
        else
        {
            return NULL;
        }
    }
}

bool PeripheryManager_::isPlaying()
{
    if (DFPLAYER_ACTIVE)
    {
        if ((dfmp3.getStatus() & 0xff) == 0x01) // 0x01 = DfMp3_StatusState_Playing
            return true;
        else
            return false;
    }
    else
    {
        return player.isPlaying();
    }
}

void PeripheryManager_::setup()
{
    if (DEBUG_MODE)
        DEBUG_PRINTLN(F("Setup periphery"));
    startTime = millis();
    pinMode(LDR_PIN, INPUT);
    pinMode(RESET_PIN, INPUT);
    if (DFPLAYER_ACTIVE)
    {
        dfmp3.begin();
        delay(100);
        setVolume(SOUND_VOLUME);
    }
    button_left.begin();
    button_right.begin();
    button_select.begin();
    button_reset.begin();

    if ((ROTATE_SCREEN && !SWAP_BUTTONS) || (!ROTATE_SCREEN && SWAP_BUTTONS))
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

#ifdef ULANZI
    button_reset.onPressedFor(5000, reset_button_pressed_long);
#endif

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    if (bme280.begin(BME280_ADDRESS) || bme280.begin(BME280_ADDRESS_ALTERNATE))
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("BME280 sensor detected"));
        TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_BME280;
    }
    else if (bmp280.begin(BMP280_ADDRESS) || bmp280.begin(BMP280_ADDRESS_ALT))
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("BMP280 sensor detected"));
        TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_BMP280;
    }
    else if (htu21df.begin())
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("HTU21DF sensor detected"));
        TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_HTU21DF;
    }
    else if (sht31.begin(0x44))
    {
        if (DEBUG_MODE)
            DEBUG_PRINTLN(F("SHT31 sensor detected"));
        TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_SHT31;
    }

#ifdef awtrix2_upgrade
    dfmp3.begin();
#else

#endif
    if (!LDR_ON_GROUND)
        photocell.setPhotocellPositionOnGround(false);
}

void PeripheryManager_::tick()
{
    if (!MenuManager.inMenu)
    {
        if (ROTATE_SCREEN)
        {
            MQTTManager.sendButton(2, button_left.read());
            ServerManager.sendButton(2, button_left.read());
            MQTTManager.sendButton(0, button_right.read());
            ServerManager.sendButton(0, button_right.read());
        }
        else
        {
            MQTTManager.sendButton(0, button_left.read());
            MQTTManager.sendButton(2, button_right.read());
            ServerManager.sendButton(0, button_left.read());
            ServerManager.sendButton(2, button_right.read());
        }

        MQTTManager.sendButton(1, button_select.read());
        ServerManager.sendButton(1, button_select.read());
    }
    else
    {
        button_left.read();
        button_select.read();
        button_right.read();
    }

    button_reset.read();

    unsigned long currentMillis_BatTempHum = millis();
    if (currentMillis_BatTempHum - previousMillis_BatTempHum >= interval_BatTempHum)
    {
        previousMillis_BatTempHum = currentMillis_BatTempHum;
#ifndef awtrix2_upgrade
        uint16_t ADCVALUE = analogRead(BATTERY_PIN);
        // Discard values that are totally out of range, especially the first value read after a reboot.
        // Meaningful values for an Ulanzi clock are in the range 400..700
        if ((ADCVALUE > 100) && (ADCVALUE < 1000))
        {
            // Send ADC values through median filter to get rid of the remaining spikes and then calculate the average
            BATTERY_RAW = meanFilterBatt.AddValue(medianFilterBatt.AddValue(ADCVALUE));
            BATTERY_PERCENT = max(min((int)map(BATTERY_RAW, MIN_BATTERY, MAX_BATTERY, 0, 100), 100), 0);
            SENSORS_STABLE = true;
        }
#else
        SENSORS_STABLE = true;
#endif
        if (SENSOR_READING)
        {
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
            case TEMP_SENSOR_TYPE_SHT31:
                sht31.readBoth(&CURRENT_TEMP, &CURRENT_HUM);
                break;
            default:
                CURRENT_TEMP = 0;
                CURRENT_HUM = 0;
                break;
            }

            CURRENT_TEMP += TEMP_OFFSET;
            CURRENT_HUM += HUM_OFFSET;
        }
        else
        {
            SENSORS_STABLE = true;
        }
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
        if (LDR_ON_GROUND)
            sampleAverage = 1023.0 - sampleAverage;
        LDR_RAW = sampleAverage;
        CURRENT_LUX = (roundf(photocell.getSmoothedLux() * 1000) / 1000);
        if (AUTO_BRIGHTNESS && !MATRIX_OFF)
        {
            brightnessPercent = (sampleAverage * LDR_FACTOR) / 1023.0 * 100.0;
            brightnessPercent = pow(brightnessPercent, LDR_GAMMA) / pow(100.0, LDR_GAMMA - 1);
            BRIGHTNESS = map(brightnessPercent, 0, 100, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
            DisplayManager.setBrightness(BRIGHTNESS);
        }
    }
}

unsigned long long PeripheryManager_::readUptime()
{
    static unsigned long lastTime = 0;
    static unsigned long long totalElapsed = 0;

    unsigned long currentTime = millis();
    if (currentTime < lastTime)
    {
        // millis() overflow
        totalElapsed += 4294967295UL - lastTime + currentTime + 1;
    }
    else
    {
        totalElapsed += currentTime - lastTime;
    }
    lastTime = currentTime;

    unsigned long long uptimeSeconds = totalElapsed / 1000;
    return uptimeSeconds;
}

void PeripheryManager_::r2d2(const char *msg)
{
#ifdef ULANZI
    for (int i = 0; msg[i] != '\0'; i++)
    {
        char c = msg[i];
        tone(BUZZER_PIN, (c - 'A' + 1) * 50);
        delay(baudRate + 10);
    }
    noTone(BUZZER_PIN);
#endif
}
