Device configuration
====================

:doc:`HADevice </documents/api/core/ha-device>` represents the physical device where the library is installed.
Logically it's a group of types like sensors, switches, lights and so on. 
In the Home Assistant, it's listed with properties that may be configured using the library's API.

Each property except the unique ID is optional.
Setting optional properties increases flash and RAM usage so it's not recommended to set them on lower-spec MCUs.

The supported properties are:

* unique ID*
* name
* software version
* manufacturer
* model

Unique ID
---------

The ID of a device needs to be unique in a scope of a Home Assistant instance.
The safest solution is to use the MAC address of an Ethernet or Wi-Fi chip but you can also implement your own solution.

There are three different ways to set the ID of the device.
You can pick one depending on your needs.

1) Providing string (const char*) to the :doc:`HADevice </documents/api/core/ha-device>` constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Try to keep the ID simple (alphanumeric characters) and short.

::

    #include <ArduinoHA.h>

    HADevice device("myUniqueID");

    void setup() {
        // ...
    }

    void loop() {
        // ...
    }

2) Providing byte array to the :doc:`HADevice </documents/api/core/ha-device>` constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    #include <ArduinoHA.h>

    // use your own unique bytes sequence
    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    HADevice device(mac, sizeof(mac));

    void setup() {
        // ...
    }

    void loop() {
        // ...
    }

3) Using :doc:`HADevice::setUniqueId </documents/api/core/ha-device>` method during the setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    #include <ArduinoHA.h>

    HADevice device;

    void setup() {
        // any length is acceptable
        byte myId[] = {0x05, 0xb4, 0xc6, 0x9f, 0xbe, 0xce, 0x8c, 0x1f, 0xc7};
        device.setUniqueId(myId, sizeof(myId)); // the input array is cloned internally

        // ...
    }

    void loop() {
        // ...
    }

Device properties
-----------------

Each property has its corresponding setter method in the :doc:`HADevice </documents/api/core/ha-device>` class.
Please note that all these methods accept const char pointer whose **content is not copied**.

::

    #include <ArduinoHA.h>

    HADevice device("myUniqueId");

    void setup() {
        device.setName("Bedroom Light Controller");
        device.setSoftwareVersion("1.0.0");
        device.setManufacturer("Developer Corp.");
        device.setModel("ABC-123");

        // ...
    }

    void loop() {
        // ...
    }