# Upgrade from AWTRIX 2 to AWTRIX 3

AWTRIX 2 was designed to run on an ESP8266, while AWTRIX 3 requires more processing power and is therefore only compatible with the ESP32.

If you own the official AWTRIX 2 mainboard, you can easily replace the ESP8266 Wemos D1 mini with an ESP32 D1 mini. The pinout remains unchanged, so no additional modifications are necessary. However, you will need to flash the [AWTRIX2_upgrade.bin](https://blueforcer.github.io/awtrix3/#/flasher?id=awtrix-20-controller-upgrade-with-esp32-d1-mini) firmware to ensure compatibility.

If you don't have an official mainboard, you can replace your current ESP8266 with an ESP32, such as the WROOM D2 variant or a standard NodeMCU32. Follow the [pinout](https://blueforcer.github.io/awtrix3/#/hardware) for the Ulanzi, and flash the [Ulanzi firmware](https://blueforcer.github.io/awtrix3/#/flasher?id=ulanzi-tc001-and-custom-builds-flasher).

Please note that AWTRIX 3 is a complete redevelopment from scratch, which means the API has changed. Be sure to carefully read the documentation, particularly the section on "apps."
