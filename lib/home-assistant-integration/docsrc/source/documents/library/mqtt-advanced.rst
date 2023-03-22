MQTT advanced features
======================

Callbacks
---------

:doc:`HAMqtt </documents/api/core/ha-mqtt>` class exposes some useful callbacks that you can bind to.
Please take a look at the example below.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void onMessage(const char* topic, const uint8_t* payload, uint16_t length) {
        // this method will be called each time the device receives an MQTT message
    }

    void onConnected() {
        // this method will be called when connection to MQTT broker is established
    }

    void setup() {
        Ethernet.begin(mac);

        mqtt.onMessage(onMessage);
        mqtt.onConnected(onConnected);
        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        Ethernet.maintain();
        mqtt.loop();
    }

Subscriptions
-------------

You can also subscribe to a custom topic using ``HAMqtt::subscribe(const char* topic)`` method.
The subscription needs to be made each time a connection to the MQTT broker is established.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void onMessage(const char* topic, const uint8_t* payload, uint16_t length) {
        if (strcmp(topic, "myTopic") == 0) {
            // message on "myTopic" received
        }
    }

    void onConnected() {
        mqtt.subscribe("myTopic");
    }

    void setup() {
        Ethernet.begin(mac);

        mqtt.onMessage(onMessage);
        mqtt.onConnected(onConnected);
        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        Ethernet.maintain();
        mqtt.loop();
    }

Publishing a message
--------------------

HAMqtt class also exposes the method that allows to publish custom messages.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void setup() {
        Ethernet.begin(mac);

        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        Ethernet.maintain();
        mqtt.loop();

        // Publishing the non-retained message:
        // mqtt.publish("customTopic", "customPayload");

        // Publishing the retained message:
        // mqtt.publish("customTopic", "customPayload", true);
    }