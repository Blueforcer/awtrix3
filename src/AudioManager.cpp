#include <AudioManager.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <LittleFS.h>
#include "Audio.h"

Preferences sender;

#define LRCLK 18
#define BCLK 17
#define DOUT 16

const char *StreamTitle;

Audio audio;

typedef struct
{
    char url[150];
    char name[32];
    uint8_t enabled;
} Station;

#define STATIONS 30 

Station stationlist[STATIONS];
#define DEFAULTSTATIONS 24

AudioManager_ &AudioManager_::getInstance()
{
    static AudioManager_ instance;
    return instance;
}

AudioManager_ &AudioManager = AudioManager.getInstance();

void loadStations()
{
    // Open the JSON file
    File file = LittleFS.open("/stations.json", "r");

    if (!file)
    {
        Serial.println("Failed to open stations.json file");
        return;
    }

    // Read the JSON data from the file
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, file);

    if (error)
    {
        Serial.println("Failed to parse stations.json file");
        file.close();
        return;
    }

    // Get the array of stations from the JSON data
    JsonArray stations = doc["stations"];

    // Save each station in the array to the stationlist array
    int i = 0;
    for (JsonObject station : stations)
    {
        const char *url = station["url"];
        const char *name = station["name"];
        uint8_t enabled = station["enabled"];
        strncpy(stationlist[i].url, url, sizeof(stationlist[i].url) - 1);
        strncpy(stationlist[i].name, name, sizeof(stationlist[i].name) - 1);
        stationlist[i].enabled = enabled;
        i++;
        if (i >= STATIONS)
        {
            break;
        }
    }
    file.close();
}

void AudioManager_::setup()
{
    audio.setPinout(BCLK, LRCLK, DOUT); // 0...21
    // audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
    audio.setVolume(14);
    loadStations();
}

void AudioManager_::tick()
{
    audio.loop();
    isRunning = audio.isRunning();
}

void AudioManager_::increaseVol()
{
    if (curGain < 21)
    {
        curGain++;
    }
    audio.setVolume(curGain);
}

void AudioManager_::decreaseVol()
{
    if (curGain > 0)
    {
        curGain--;
    }
    audio.setVolume(curGain);
}

void AudioManager_::playText(String text)
{
    audio.connecttospeech(text.c_str(), "de");
}

void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}

void audio_showstreamtitle(const char *info)
{
    Serial.print("Streamtitle     ");
    Serial.println(info);
    AudioManager_::getInstance().StreamInfo = String(info);
}

String AudioManager_::getNextRadioStation(bool playPrevious)
{
    static int currentStation = 0;         // Speichere die Indexnummer der letzten gespielten Station
    int startStation = currentStation + 1; // Starte mit der nächsten Station

    if (playPrevious)
    {
        // Wenn die vorherige Station aktiviert ist, aktualisiere die Indexnummer
        int prevStation = currentStation - 1;
        if (prevStation >= 0 && stationlist[prevStation].enabled)
        {
            currentStation = prevStation;
            return String(stationlist[currentStation].name);
        }
    }

    // Durchlaufe die Liste von Stationen, beginnend mit der nächsten Station
    for (int i = startStation; i < STATIONS; i++)
    {
        // Wenn die Station aktiviert ist, aktualisiere die Indexnummer und gib den Namen zurück
        if (stationlist[i].enabled)
        {
            currentStation = i;
            return String(stationlist[currentStation].name);
        }
    }

    // Wenn keine Station aktiviert ist, beginne wieder von vorne
    for (int i = 0; i <= currentStation; i++)
    {
        // Wenn die Station aktiviert ist, aktualisiere die Indexnummer und gib den Namen zurück
        if (stationlist[i].enabled)
        {
            currentStation = i;
            return String(stationlist[currentStation].name);
        }
    }

    // Wenn keine Station aktiviert ist, gebe einen leeren String zurück
    return "";
}

void AudioManager_::startRadioStation(String stationName)
{
    // Durchlaufe die Liste von Stationen, um die URL der Station mit dem gegebenen Namen zu finden
    for (int i = 0; i < STATIONS; i++)
    {
        if (stationlist[i].enabled && String(stationlist[i].name) == stationName)
        {
            audio.connecttohost(stationlist[i].url);
            break;
        }
    }
}

String AudioManager_::getCurrentRadioStation()
{
    int currentStation = stationIndex;
    if (currentStation >= 0 && currentStation < STATIONS && stationlist[currentStation].enabled)
    {
        return String(stationlist[currentStation].name);
    }
    return "";
}

void AudioManager_::prevStation()
{
    do
    {
        stationIndex--;
        if (stationIndex < 0)
        {
            stationIndex = STATIONS - 1; // Wrap around to the last station
        }
    } while (!stationlist[stationIndex].enabled);
}

void AudioManager_::nextStation()
{
    do
    {
        stationIndex++;
        if (stationIndex >= STATIONS)
        {
            stationIndex = 0; // Wrap around to the first station
        }
    } while (!stationlist[stationIndex].enabled);
}

void AudioManager_::stopPlay(){
    audio.stopSong();
}