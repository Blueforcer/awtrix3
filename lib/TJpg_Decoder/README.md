Arduino TJpg_Decoder library
===========

This Arduino library supports the rendering of Jpeg files stored both on SD card and in arrays within program memory (FLASH) onto a TFT display. In addition images stored in the SPIFFS (ESP32, ESP8266) and LittleFS (ESP32, ESP8266, RP2040) Flash filing systems or alternatively "PROGMEM" arrays can be used.

The library has been tested on the Arduino Due, ESP32 and ESP8266 (e.g. NodeMCU 1.0), STM32 and RP2040 processors. Other processors should work too if they have sufficient memory. Use with the ESP32 requires Arduino board package 2.0.0 or later.

Jpeg files must be in 24bit format (8 bit not supported). Jpeg files in the "Progressive" format (where image data is compressed in multiple passes with progressively higher detail) are not supported either since this would require much more memory.

When storing the jpeg in a memory array bear in mind that some Arduino boards have a maximum 32767 byte limit for the maximum size of an array (32 KBytes minus 1 byte).

The decompression of Jpeg images needs more RAM than an UNO provides, thus this library is targetted at processors with more RAM. The decoder has a very small memory footprint, typically 3.5K Bytes of RAM (for workspace, Independent of Image Dimensions) and 3.5-8.5K Bytes of ROM for text and constants.


On a Mega the number of images stored in FLASH must be limited because it they are large enough to push the executable code start over the 64K 16 bit address limit then the Mega will fail to boot even though the sketch compiles and uploads correctly. This is a limitation imposed by the Arduino environment not this library!  The Arduino Mega is not recommended as it does not reliably decode some jpeg images possibly due to a shortage of RAM.  The Due will work fine with much bigger image sets in FLASH.

This library uses the TJpgDec decompressor engine detailed here:
http://elm-chan.org/fsw/tjpgd/00index.html
TJpgDec is a generic JPEG image decompressor module that highly optimized for small embedded systems.
