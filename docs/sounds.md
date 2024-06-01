# Sounds

You can play sound with the integrated passive buzzer in the Ulanzi Clock or you can use a DFPlayer to play mp3 with your custom build.

## Buzzer
With the Buzzer you can play monophone RTTTL melodies in two ways:
  
**via API:**  
You can send the RTTTL string directly with your API request.  
See documentation for commands:  
[Single sounds](https://blueforcer.github.io/awtrix3/#/api?id=sound-playback)  
[With your notification](https://blueforcer.github.io/awtrix3/#/api?id=json-properties)  
  
**via file:**  
This method is to avoid long jsons beacuse the receive buffer is limited.  
Go to the file manager in the web interface and create a new text file in the "MELODIES" folder.  
Name it whatever you like but use the ".txt" extension, e.g. "alarm.txt". Inside the file, place a melody in RTTTL format.  
When using the sound file anywhere, omit the file extension.  
  
You can find many melodies on the internet. For example:
* [Laub-Home Wiki: RTTTL Songs](https://www.laub-home.de/wiki/RTTTL_Songs)
* [Online RTTTL player](https://adamonsoon.github.io/rtttl-play/)
* [RTTTL editor](https://corax89.github.io/esp8266Game/soundEditor.html)

## DFPlayer

The DFPlayer is used for the output of sounds and can be used optionally. It stores mp3 files on a memory card, which is inserted into the micro SD slot of the player. This allows you to have your awtrix speak to you or play sounds when you receive notifications, for example.
You need to create a folder “MP3” on your DFplayer SD card and move your mp3s to this folder. The mp3 must start with a 4-digit number, e.g. 0001.mp3 or 0001 - Testfile.mp3.  
You can now play them by using the /sound API or the "sound" key in your notification.
