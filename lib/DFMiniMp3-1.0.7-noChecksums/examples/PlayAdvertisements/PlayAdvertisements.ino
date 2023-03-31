// this example will play a track and then every 60 seconds
// it will play an advertisement
//
// it expects the sd card to contain the following mp3 files
// but doesn't care whats in them
//
// sd:/01/001.mp3 - the song to play, the longer the better
// sd:/advert/0001.mp3 - the advertisement to interrupt the song, keep it short

#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

// implement a notification class,
// its member methods will get called 
//
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

// instance a DFMiniMp3 object, 
// defined with the above notification class and the hardware serial class
//
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial1);

// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definition and uncomment these lines
//SoftwareSerial secondarySerial(10, 11); // RX, TX
//DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

uint32_t lastAdvert; // track time for last advertisement

void setup() 
{
  Serial.begin(115200);

  Serial.println("initializing...");
  
  mp3.begin();
  uint16_t volume = mp3.getVolume();
  Serial.print("volume was ");
  Serial.println(volume);
  mp3.setVolume(24);
  volume = mp3.getVolume();
  Serial.print(" and changed to  ");
  Serial.println(volume);
  
  Serial.println("track 1 from folder 1"); 
  mp3.playFolderTrack(1, 1); // sd:/01/001.mp3

  lastAdvert = millis();
}

void loop() 
{
  uint32_t now = millis();
  if ((now - lastAdvert) > 60000)
  {
    // interrupt the song and play the advertisement, it will
    // return to the song when its done playing automatically
    mp3.playAdvertisement(1); // sd:/advert/0001.mp3
    lastAdvert = now;
  }
  mp3.loop();
}
