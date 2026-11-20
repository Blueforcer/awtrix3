#pragma once

#ifdef ESP8266
#include <Arduino.h>

// The "draw" instructions of a notification or custom app, converted once when they
// arrive (DisplayManager_::compileDrawInstructions) into a compact list of commands
// with the colours already worked out. The JSON used to be kept as text and parsed
// again on every frame: ~2 ms per frame for 25 commands, plus a JSON document
// allocated and freed 40 times a second.
// The bytes are kept in a String: unlike std::vector, it doesn't restart the ESP8266 when
// there's no memory for it (a copy just comes out empty and nothing is drawn).
struct DrawCommands
{
    String bytes;
    size_t length() const { return bytes.length(); }
};
#endif
