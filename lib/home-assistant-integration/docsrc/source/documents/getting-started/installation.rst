Installation
============

There are two ways to install the library depending on your project.
If you're using Arduino IDE to compile your project you only need to install the library into the IDE.

The second method is meant for more advanced users that use *makeEspArduino* for building the project for ESP32/ESP8266 devices.

Arduino IDE
-----------

To install the library into your Arduino IDE you can use Library Manager (available from IDE version 1.6.2).
Open the IDE and click to the "Sketch" menu and then *Include Library > Manage Libraries*.

.. image:: images/manage-libraries.png
  :width: 500
  :align: center

Type "home-assistant-integration" in the search field, select the latest version of the library from the dropdown and then click *Install*.
  
.. image:: images/library-search.png
  :width: 500
  :align: center

For other installation methods please refer to `Arduino documentation <https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries>`_.

makeEspArduino
--------------

The library can be installed in an environment managed by `makeEspArduino <https://github.com/plerup/makeEspArduino>`_.
The best approach is to add the library and its dependency as submodules in the project as follows:

::

    git submodule add https://github.com/dawidchyrzynski/arduino-home-assistant.git arduino-home-assistant
    cd arduino-home-assistant && git checkout tags/2.0.0 && cd ..
    git submodule add https://github.com/knolleary/pubsubclient.git pubsubclient
    cd pubsubclient && git checkout tags/v2.8

Then you just need to add one extra line in your `Makefile`:

::

    LIBS := $(ROOT)/arduino-home-assistant $(ROOT)/pubsubclient