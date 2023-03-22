Availability reporting
======================

Home Assistant allows to track online/offline states of devices and device types.
In this way controls available in the panel will be displayed as disabled if a device is offline.

The library allows to expose state of the entire device (i.e. shared availability) or specific type (sensor, switch, light, etc.).
By default this feature is not enabled to save resources (RAM and flash) but you can easily turn it on as shown below.

Shared availability
-------------------

I highly recommend to use shared availability feature as it allows to utilize MQTT LWT.
Basically, shared availability allows to control availability of all types related to a specific device.
For example: if your device has 5 switches and 2 buttons you can control their availability in the HA panel using a single method call.

See example below showing how to enable shared availability of the device.
By default, the device is considered online but you can control its state manually using ``HADevice::setAvailability(bool online)`` method.
In most cases you won't need to control availability manually as the library takes care of availability as long as the device is powered on.

::

    #include <ArduinoHA.h>

    HADevice device("myUniqueID");

    void setup() {
        device.enableSharedAvailability();
        // device.setAvailability(false); // changes default state to offline

        // ...
    }

    void loop() {
        // ...

        // device.setAvailability(true); // you can control availability manually if you want
    }

MQTT LWT
--------

The shared availability feature is considered a complete solution only if it's used with MQTT LWT feature.
Without LWT if the device is powered off then Home Assistant displays it as online.
That's because availability tracking relies on MQTT messages and if you cut off power of your device then its not capable of publishing the offline message.

When LWT feature is enabled the device becomes offline in the HA panel even if you cut off power supply.
This solution is implemented by MQTT broker that automatically publishes the message when the TCP connection to the device is lost.

::

    #include <ArduinoHA.h>

    HADevice device("myUniqueID");

    void setup() {
        device.enableSharedAvailability();
        device.enableLastWill();

        // ...
    }

    void loop() {
        // ...
    }

Device type's availability
--------------------------

There also a way to control availability of specific device types.
Each type can be controlled separately as shown below.
Please note that this solution requires shared availability to be disabled and it's not supported by LWT.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);
    HASwitch mySwitch("mySwitchId");

    void setup() {
        Ethernet.begin(mac);

        // this line enables availability for your switch
        mySwitch.setAvailability(true); // you can also set it to false

        // ...
    }

    void loop() {
        // ...

        // you can control availability at runtime as follows:
        mySwitch.setAvailability(true); // online
        mySwitch.setAvailability(false); // offline
    }