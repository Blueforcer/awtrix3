Prerequisites
=============

ArduinoHA uses MQTT protocol over TCP to communicate with your Home Assistant instance.
The MQTT broker is not installed by default so you will need to install an extra HA addon in order to get the integration working.

The recommended setup method is to use the `Mosquitto Brokker addon <https://github.com/home-assistant/addons/blob/master/mosquitto/DOCS.md>`_.
After installing the addon your HA instance acts as a broker for all devices you're going to integrate with it.
