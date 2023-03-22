ArduinoHA documentation
==================================================

ArduinoHA allows to integrate an Arduino/ESP based device with Home Assistant using MQTT.
The library is designed to use as low resources (RAM/flash) as possible.
Initially, it was optimized to work on Arduino Uno with Ethernet Shield,
but I successfully use it on ESP8266/ESP8255 boards in my projects.

Features
--------

* Two-way communication (state reporting and command execution)
* MQTT discovery (device is added to the Home Assistant panel automatically)
* MQTT Last Will and Testament
* Support for custom MQTT messages (publishing and subscribing)
* Auto reconnect with MQTT broker
* Reporting availability (online/offline states) of a device
* Doxygen documentation
* Covered by unit tests (AUnit + EpoxyDuino + AUniter)

.. toctree::
   :caption: Docs
   :maxdepth: 3

   documents/getting-started/index
   documents/library/index
   documents/api/index