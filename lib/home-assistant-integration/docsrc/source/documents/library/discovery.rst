Discovery
=========

The library automatically maintains connection to the MQTT broker and takes care of the discovery process.
Each device type that you create (sensor, switch, light, fan, etc.) is automatically registered in MQTT manager.
Whenever connection with the MQTT broker is acquired the configuration of all device types is pushed to the Home Assistant.

There is one basic rule that you need to follow: device types need to be constructed after :doc:`HAMqtt </documents/api/core/ha-mqtt>` class.
That's because device types are relying on :doc:`HAMqtt </documents/api/core/ha-mqtt>` instance internally.

Topics prefix
-------------

In some cases you may need to change prefix of MQTT topics.
There are two types of topics utilized by the library:

* **discovery topic** - used for publishing device types' configuration (default: ``homeassistant``)
* **data topic** - used for publishing states, data, etc. (default: ``aha``)

The discovery topic's prefix can be changed using ``HAMqtt::setDiscoveryPrefix(const char* prefix)`` method.
The data topic's prefix can be changed using ``HAMqtt::setDataPrefix(const char* prefix)`` method

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    // register your device types here

    void setup() {
        mqtt.setDiscoveryPrefix("myCustomPrefix");
        mqtt.setDataPrefix("myDataPrefix");

        // ...
    }

    void loop() {
        // ...
    }
