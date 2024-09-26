# Upgrade from AWTRIX 2 to AWTRIX 3

AWTRIX 2 used to run on an ESP8266. AWTRIX 3 requires more resources and therefore only runs on an ESP32. 

If you have the official AWTRIX 2 mainboard, you can simply swap the ESP8266 Wemos D1 mini 1:1 with an ESP32 D1 mini without changing the pinout. However, you will need the AWTRIX2_upgrade.bin firmware as the pinout has remained the same.

If you do not have a mainboard then replace your current ESP8266 with an ESP32 as a WROOM D2 variant, a normal NodeMCU32 should also do the trick.
Use the [pinout](https://blueforcer.github.io/awtrix3/#/hardware) of the Ulanzi and also flash the Ulanzi firmware