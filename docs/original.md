If you want to remove AWTRIX from your Ulanzi TC001 you need to flash the original Firmware.


## Method 1
1. Download the dump of the [original firmware](https://raw.githubusercontent.com/Blueforcer/awtrix3/main/docs/assets/ulanzi_original_firmware.bin)
2. Connect your Ulanzi with your PC and open https://esp.huhn.me/ in your browser.
3. Hit Connect -> Erase
4. Upload the .bin file and enter 0x00000. Then hit "program"  
![image](https://github.com/Blueforcer/awtrix3/assets/31169771/b79bdf7e-477e-47f6-a41e-9106519f636b)
    
## Method 2 (Windows only) 
1. Download the [ESP32 download tool](https://www.espressif.com/en/support/download/other-tools)
2. Download the dump of the [original firmware](https://raw.githubusercontent.com/Blueforcer/awtrix3/main/docs/assets/ulanzi_original_firmware.bin)
3. Start the ESP32 download tool and select ESP32 as chip type

Set it up like that and hit "Start"  
  
![image](https://github.com/Blueforcer/awtrix3/assets/31169771/48a29f33-4896-4ee5-a001-17b44710c8ae)
