MQTT security
=============

The library allows you to use credentials for acquiring a TCP connection with the MQTT broker.
By default you can use the same credentials you use for login in the Home Assistant panel but you can also configure custom credentials in the Mosquitto broker.

.. DANGER::

    This solution is not 100% secure because communication between Arduino and Home Assistant is not encrypted.
    Username and password can be easily discovered by analyzing your local network traffic.
    However, that's fine as long as your local network is secured against unattended access.
    On more powerful devices (like ESP), you should consider using TLS/SSL connection.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void setup() {
        // ...

        // replace username and password with your credentials
        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        // ...
    }


SSL connection
--------------

On ESP32/ESP8266 you can use ``WiFiClientSecure`` client to establish encrypted connection between your device and Home Assistant.
Please take a look at :example:`this example <mqtt-with-ssl/mqtt-with-ssl.ino>`.
