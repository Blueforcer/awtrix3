Compiler macros
===============

The library supports couple of macros that are defined in the `ArduinoHADefines.h` file.
You can uncomment them in this specific file or provide flags directly to compiler (for example: via Makefile).

Debug mode
----------

Debug mode unlocks logging feature in the library.
Logs may be useful for debugging the communication with the Home Assistant.

To enable debug mode you need to defined `ARDUINOHA_DEBUG` macro.

Code optimization
-----------------

Defining one of the macros listed below results in truncating the corresponding device type.
It may be useful if you want to save some flash memory occupied by virtual tables of those classes.

* `EX_ARDUINOHA_BINARY_SENSOR`
* `EX_ARDUINOHA_BUTTON` 
* `EX_ARDUINOHA_CAMERA`
* `EX_ARDUINOHA_COVER`
* `EX_ARDUINOHA_DEVICE_TRACKER`
* `EX_ARDUINOHA_DEVICE_TRIGGER`
* `EX_ARDUINOHA_FAN`
* `EX_ARDUINOHA_HVAC`
* `EX_ARDUINOHA_LIGHT`
* `EX_ARDUINOHA_LOCK`
* `EX_ARDUINOHA_NUMBER`
* `EX_ARDUINOHA_SCENE`
* `EX_ARDUINOHA_SELECT`
* `EX_ARDUINOHA_SENSOR`
* `EX_ARDUINOHA_SWITCH`
* `EX_ARDUINOHA_TAG_SCANNER`
