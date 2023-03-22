Connection parameters
=====================

:doc:`HAMqtt </documents/api/core/ha-mqtt>` class exposes a few variants of the ``begin`` method that allows specifying the MQTT connection parameters.
**This method should be called only once and at the end of the setup logic.**
The example below presents all possible variants. Pick one that meets your requirements.

.. NOTE::

    Connection to the MQTT broker is established asynchronously.
    The :doc:`HAMqtt::begin </documents/api/core/ha-mqtt>` method just sets the parameters of the connection.
    The connection attempt is made during the loop cycle.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void setup() {
        Ethernet.begin(mac);

        // anoymous connection on default port 1883
        mqtt.begin("192.168.1.50");

        // anoymous connection on port 8888
        mqtt.begin("192.168.1.50", 8888);

        // connection with credentials on default port 1883
        mqtt.begin("192.168.1.50", "username", "password");

        // connection with credentials on port 8888
        mqtt.begin("192.168.1.50", 8888, "username", "password");

        // you can also use hostname in all variants
        mqtt.begin("mybroker.local");
    }

    void loop() {
        Ethernet.maintain();
        mqtt.loop();
    }