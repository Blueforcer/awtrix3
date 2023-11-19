/*The MIT License (MIT)

Copyright (c) 2014 Nathanaël Lécaudé
https://github.com/natcl/Artnet, http://forum.pjrc.com/threads/24688-Artnet-to-OctoWS2811

Copyright (c) 2016,2019 Stephan Ruloff
https://github.com/rstephan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef ARTNET_WIFI_H
#define ARTNET_WIFI_H

#include <functional>
#include <Arduino.h>
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_SAMD)
#if defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#else
#include <WiFiNINA.h>
#endif
#else
#error "Architecture not supported!"
#endif
#include <WiFiUdp.h>

// UDP specific
#define ART_NET_PORT 6454
// Opcodes
#define ART_POLL 0x2000
#define ART_DMX 0x5000
#define ART_SYNC 0x5200
// Buffers
#define MAX_BUFFER_ARTNET 530
// Packet
#define ART_NET_ID "Art-Net"
#define ART_DMX_START 18

#define DMX_FUNC_PARAM uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data
typedef std::function <void (DMX_FUNC_PARAM)> StdFuncDmx_t;

class ArtnetWifi
{
public:
  ArtnetWifi();

  void begin(String hostname = "");
  uint16_t read(void);
  /* returns 1 for Ok, or 0 on problem */
  int write(void);
  int write(IPAddress ip);
  void setByte(uint16_t pos, uint8_t value);
  void printPacketHeader(void);
  void printPacketContent(void);

  // Return a pointer to the start of the DMX data
  inline uint8_t* getDmxFrame(void)
  {
    return artnetPacket + ART_DMX_START;
  }

  inline uint16_t getOpcode(void)
  {
    return opcode;
  }

  inline uint8_t getSequence(void)
  {
    return sequence;
  }

  inline uint16_t getUniverse(void)
  {
    return incomingUniverse;
  }

  inline void setUniverse(uint16_t universe)
  {
    outgoingUniverse = universe;
  }

  inline void setPhysical(uint8_t port)
  {
    physical = port;
  }

  [[deprecated]]
  inline void setPhisical(uint8_t port)
  {
    setPhysical(port);
  }

  inline uint16_t getLength(void)
  {
    return dmxDataLength;
  }

  inline void setLength(uint16_t len)
  {
    dmxDataLength = len;
  }

  inline void setArtDmxCallback(void (*fptr)(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data))
  {
    artDmxCallback = fptr;
  }

  inline void setArtDmxFunc(StdFuncDmx_t func)
  {
    artDmxFunc = func;
  }

  inline IPAddress& getSenderIp()
  {
    return senderIp;
  }

private:
  uint16_t makePacket(void);

  WiFiUDP Udp;
  String host;
  uint8_t artnetPacket[MAX_BUFFER_ARTNET];
  uint16_t packetSize;
  uint16_t opcode;
  uint8_t sequence;
  uint8_t physical;
  uint16_t incomingUniverse;
  uint16_t outgoingUniverse;
  uint16_t dmxDataLength;
  void (*artDmxCallback)(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);
  StdFuncDmx_t artDmxFunc;
  static const char artnetId[];
  IPAddress senderIp;
};

#endif
