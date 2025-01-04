# Frequently Asked Questions (FAQ)

Welcome to the FAQ section. Here, you'll find answers to some of the most common questions i receive. If you don't find what you're looking for, please don't hesitate to join the [discord server](https://discord.gg/cyBCpdx).

### Table of Contents
- [General Questions](#general-questions)
  - [Can I run AWTRIX 3 standalone?](#q-can-i-run-awtrix3-standalone)
  - [Can I use different matrix sizes other than 8x32?](#q-can-i-use-different-matrix-sizes-other-than-8x32)
  - [Error while flashing my Ulanzi clock](#q-why-am-i-getting-an-error-while-flashing-my-ulanzi-clock-with-the-awtrix-web-flasher)
  - [I'm experiencing drawing glitches with my icons](#q-i39m-experiencing-drawing-glitches-with-my-icons)
  - [I only have 192KB of free flash memory](#q-i-only-have-192kb-of-free-flash-memory-what39s-wrong)
  - [Accidentally touched DoNotTouch.json](#q-what-should-i-do-if-i-accidentally-touched-the-donottouchjson-file-in-awtrix-web-file-manager)
  - [Self-built AWTRIX displaying meaningless characters](#q-my-self-built-awtrix-device-is-displaying-meaningless-characters-on-the-matrix-what-should-i-do)
  - [Which firmware for building my own AWTRIX?](#q-i-want-to-build-my-own-awtrix-which-firmware-should-i-use)
  - [Ulanzi TC001 temperature sensor reporting high temperature](#q-why-does-the-ulanzi-tc001-temperature-sensor-report-a-high-temperature)
  - [Blinking LED on top or bottom left corner](#q-i-have-a-blinking-led-on-the-top-left--bottom-left-corner-what-does-it-mean)


### General Questions
#### Q: Can I Run AWTRIX 3 Standalone?
A: No, AWTRIX is designed as a companion for your smart home system. It displays information based on what you send through its API.

#### Q: Can I Use Different Matrix Sizes Other Than 8x32?
A: Currently, AWTRIX is optimized for the Ulanzi Pixelclock, which supports a matrix size of 32x8 pixels. Other sizes are not supported at this time.

#### Q: Why Am I Getting an Error While Flashing My Ulanzi Clock with the AWTRIX Web Flasher?
A: If you encounter errors during the flashing process, i recommend trying a different USB cable and USB port. Some users have reported issues with the original cable provided with the device.

#### Q: I'm experiencing drawing glitches with my icons.
A: The AWTRIX GIF renderer currently supports only 8-bit GIFs without transparency. If you notice graphic glitches on the matrix, try replacing transparent pixels with solid black. You can easily do this using an online tool like [this one](https://onlinegiftools.com/add-gif-background). 

#### Q: I only have 192KB of free flash memory. What's wrong?
A: The issue arises because Ulanzi uses a different partition table, resulting in a smaller SPIFFS partition.  
To utilize the full 1350KB of flash memory, make sure to use the AWTRIX online flasher, which also writes to all other partitions.  
The Ulanzi web updater only writes to the program partition.

#### Q: What should I do if I accidentally touched the `DoNotTouch.json` file in AWTRIX Web File Manager?
A: The `DoNotTouch.json` file contains settings such as:

- Static IP configuration settings
- MQTT settings
- Homeassistant Discovery
- NTP Server / TimeZone
- HTTP Auth password

If you have accidentally modified this file, you should delete it from the file manager and then reboot your AWTRIX device. The file will be automatically recreated, but you will need to reconfigure the settings mentioned above.

#### Q: My self-built AWTRIX device is displaying meaningless characters on the matrix. What should I do?
A: If your self-built AWTRIX matrix is showing meaningless characters, you need to change the matrix type.

Create a `dev.json` file in the root directory using the file manager with the content as shown below. There are three possible values for the matrix type:
- 0
- 1
- 2
> The default when you flash AWTRIX firmware is 0, so try 1 or 2

Example `dev.json` file content
```json
{
  "matrix": "2"
}
```

Reboot the device

#### Q: I want to build my own AWTRIX, which firmware should I use?
A: Please read the options you have specified below

#### Option 1: [Ulanzi TC001 and Custom Builds Flasher](https://blueforcer.github.io/awtrix3/#/flasher?id=ulanzi-tc001-and-custom-builds-flasher)
- **Firmware Compatibility**: This firmware is compatible with both the Ulanzi TC001 and custom build AWTRIX devices using any ESP32-WROOM board, including the ESP32 D1 Mini.
- **Pinout Guide**: Ensure you follow the specific pinout for your custom build to function correctly with this firmware. The pinout guide is available at: [AWTRIX 3 Hardware Guide](https://blueforcer.github.io/awtrix3/#/hardware)


#### Option 2: [Awtrix 2.0 Controller Upgrade (with ESP32 D1 Mini)](https://blueforcer.github.io/awtrix3/#/flasher?id=awtrix-20-controller-upgrade-with-esp32-d1-mini)
- **Firmware Purpose**: This firmware is specifically designed for upgrading an old AWTRIX 2.0. The original AWTRIX 2.0 was built with a Wemos D1 mini (ESP8266), which is not supported by AWTRIX 3.
- **Upgrade Requirements**: To upgrade, the ESP8266 needs to be replaced with an ESP32 D1 Mini. The ESP32 D1 Mini maintains the same form factor, allowing it to replace the older module on the motherboard directly.
- **Pinout Compatibility**: The ESP32 D1 Mini uses the same pinout as the old AWTRIX 2.0.
- **Build Information**: For more details about this build, visit: [AWTRIX 2.0 Build Information](https://awtrixdocs.blueforcer.de/#/en-en/awtrix_family?id=parts-list)

#### Q: Why does the Ulanzi TC001 temperature sensor report a high temperature?
A: The temperature sensor measures the temperature inside the case, which is not very useful for external temperature readings.
The heat generated by internal components, such as the matrix (depending on the brightness and the number of LEDs turned on) and the battery, affects this measurement.

You can adjust the temperature and humidity offset settings in the `dev.json` file, which can be found at [AWTRIX 3 Developer Settings](https://blueforcer.github.io/awtrix3/#/dev). The configuration of these settings depends on your specific requirements.

Create a `dev.json` file in the root directory using the file manager with the content as shown below. 

*Example dev.json file content*
```json
{
  "temp_offset": -5,
  "hum_offset": -1
}
```

Reboot the device

#### Q: I have a blinking LED on the top left / bottom left corner. What does it mean?
A: Within the AWTRIX firmware, there are 2 status LEDs indicating different issues:

- Blinking LED in the top left corner indicates a problem connecting to WiFi.
- Blinking LED in the bottom left corner signifies a problem connecting to your MQTT broker.

