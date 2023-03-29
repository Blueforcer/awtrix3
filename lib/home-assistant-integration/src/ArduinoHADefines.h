// Turns on debug information of the ArduinoHA core.
// Please note that you need to initialize serial interface manually
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
 //#define ARDUINOHA_DEBUG

// These macros allow to exclude some parts of the library to save more resources.
 //#define EX_ARDUINOHA_BINARY_SENSOR
 //#define EX_ARDUINOHA_BUTTON
 #define EX_ARDUINOHA_CAMERA
 #define EX_ARDUINOHA_COVER
 #define EX_ARDUINOHA_DEVICE_TRACKER
 #define EX_ARDUINOHA_DEVICE_TRIGGER
 #define EX_ARDUINOHA_FAN
 #define EX_ARDUINOHA_HVAC
 //#define EX_ARDUINOHA_LIGHT
 #define EX_ARDUINOHA_LOCK
 #define EX_ARDUINOHA_NUMBER
 #define EX_ARDUINOHA_SCENE
 //#define EX_ARDUINOHA_SELECT
// #define EX_ARDUINOHA_SENSOR
// #define EX_ARDUINOHA_SWITCH
 #define EX_ARDUINOHA_TAG_SCANNER

#if defined(ARDUINOHA_DEBUG)
    #include <Arduino.h>
    #define ARDUINOHA_DEBUG_PRINTLN(x) Serial.println(x);
    #define ARDUINOHA_DEBUG_PRINT(x) Serial.print(x);
#else
    #define ARDUINOHA_DEBUG_INIT()
    #define ARDUINOHA_DEBUG_PRINTLN(x)
    #define ARDUINOHA_DEBUG_PRINT(x)
#endif

#define AHATOFSTR(x) reinterpret_cast<const __FlashStringHelper*>(x)
#define AHAFROMFSTR(x) reinterpret_cast<const char*>(x)
