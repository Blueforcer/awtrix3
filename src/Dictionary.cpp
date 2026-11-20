#include <Arduino.h>
#include "Dictionary.h"

// These strings are used as plain C strings (JSON keys, MQTT topics, Home Assistant
// names). The ESP8266 can't read flash byte by byte, so there they stay in RAM.
#ifdef ESP8266
#define DICT_PROGMEM
#else
#define DICT_PROGMEM PROGMEM
#endif
 
// MQTT
const char StatsTopic[] DICT_PROGMEM = {"stats"};
const char ButtonLeftTopic[] DICT_PROGMEM = {"stats/buttonLeft"};
const char ButtonSelectTopic[] DICT_PROGMEM = {"stats/buttonSelect"};
const char ButtonRightTopic[] DICT_PROGMEM = {"stats/buttonRight"};

// Generic
const char State0[] DICT_PROGMEM = {"0"};
const char State1[] DICT_PROGMEM = {"1"};

// HA
const char HAmanufacturer[] DICT_PROGMEM = {"Blueforcer"};
const char HAmodel[] DICT_PROGMEM = {"AWTRIX 3"};
const char HAmatID[] DICT_PROGMEM = {"%s_mat"};
const char HAmatIcon[] DICT_PROGMEM = {"mdi:clock-digital"};
const char HAmatName[] DICT_PROGMEM = {"Matrix"};

const char HAi1ID[] DICT_PROGMEM = {"%s_ind1"};
const char HAi1Icon[] DICT_PROGMEM = {"mdi:arrow-top-right-thick"};
const char HAi1Name[] DICT_PROGMEM = {"Indicator 1"};

const char HAi2ID[] DICT_PROGMEM = {"%s_ind2"};
const char HAi2Icon[] DICT_PROGMEM = {"mdi:arrow-right-thick"};
const char HAi2Name[] DICT_PROGMEM = {"Indicator 2"};

const char HAi3ID[] DICT_PROGMEM = {"%s_ind3"};
const char HAi3Icon[] DICT_PROGMEM = {"mdi:arrow-bottom-right-thick"};
const char HAi3Name[] DICT_PROGMEM = {"Indicator 3"};

const char HASPEEDID[] DICT_PROGMEM = {"%s_sspeed"};
const char HASPEEDIcon[] DICT_PROGMEM = {"mdi:speedometer"};
const char HASPEEDName[] DICT_PROGMEM = {"Scrollspeed"};

const char HAbriID[] DICT_PROGMEM = {"%s_bri"};
const char HAbriIcon[] DICT_PROGMEM = {"mdi:brightness-auto"};
const char HAbriName[] DICT_PROGMEM = {"Brightness mode"};
const char HAbriOptions[] DICT_PROGMEM = {"Manual;Auto"};

const char HAeffectID[] DICT_PROGMEM = {"%s_eff"};
const char HAeffectIcon[] DICT_PROGMEM = {"mdi:auto-fix"};
const char HAeffectName[] DICT_PROGMEM = {"Transition effect"};
const char HAeffectOptions[] DICT_PROGMEM = {"Random;Slide;Dim;Zoom;Rotate;Pixelate;Curtain;Ripple;Blink;Reload;Fade"};

const char HAbtnaID[] DICT_PROGMEM = {"%s_btna"};
const char HAbtnaIcon[] DICT_PROGMEM = {"mdi:bell-off"};
const char HAbtnaName[] DICT_PROGMEM = {"Dismiss notification"};

const char HAbtnbID[] DICT_PROGMEM = {"%s_btnb"};
const char HAbtnbIcon[] DICT_PROGMEM = {"mdi:arrow-right-bold"};
const char HAbtnbName[] DICT_PROGMEM = {"Next app"};

const char HAbtncID[] DICT_PROGMEM = {"%s_btnc"};
const char HAbtncIcon[] DICT_PROGMEM = {"mdi:arrow-left-bold"};
const char HAbtncName[] DICT_PROGMEM = {"Previous app"};

const char HAappID[] DICT_PROGMEM = {"%s_app"};
const char HAappIcon[] DICT_PROGMEM = {"mdi:apps"};
const char HAappName[] DICT_PROGMEM = {"Current app"};


const char HAIDID[] DICT_PROGMEM = {"%s_id"};
const char HAIDIcon[] DICT_PROGMEM = {"mdi:id-card"};
const char HAIDName[] DICT_PROGMEM = {"Device topic"};


const char HAtempID[] DICT_PROGMEM = {"%s_temp"};
const char HAtempIcon[] DICT_PROGMEM = {"mdi:thermometer"};
const char HAtempName[] DICT_PROGMEM = {"Temperature"};
const char HAtempClass[] DICT_PROGMEM = {"temperature"};
const char HAtempUnit[] DICT_PROGMEM = {"°C"};

const char HAhumID[] DICT_PROGMEM = {"%s_hum"};
const char HAhumIcon[] DICT_PROGMEM = {"mdi:water-percent"};
const char HAhumName[] DICT_PROGMEM = {"Humidity"};
const char HAhumClass[] DICT_PROGMEM = {"humidity"};
const char HAhumUnit[] DICT_PROGMEM = {"%"};

const char HAbatID[] DICT_PROGMEM = {"%s_bat"};
const char HAbatIcon[] DICT_PROGMEM = {"mdi:battery-90"};
const char HAbatName[] DICT_PROGMEM = {"Battery"};
const char HAbatClass[] DICT_PROGMEM = {"battery"};
const char HAbatUnit[] DICT_PROGMEM = {"%"};

const char HAluxID[] DICT_PROGMEM = {"%s_lux"};
const char HAluxIcon[] DICT_PROGMEM = {"mdi:sun-wireless"};
const char HAluxName[] DICT_PROGMEM = {"Illuminance"};
const char HAluxClass[] DICT_PROGMEM = {"illuminance"};
const char HAluxUnit[] DICT_PROGMEM = {"lx"};

const char HAverID[] DICT_PROGMEM = {"%s_ver"};
const char HAverName[] DICT_PROGMEM = {"Version"};

const char HAtransID[] DICT_PROGMEM = {"%s_tra"};
const char HAtransName[] DICT_PROGMEM = {"Transition"};
const char HAtransIcon[] DICT_PROGMEM = {"mdi:swap-horizontal"};

const char HAupdateID[] DICT_PROGMEM = {"%s_upd"};
const char HAupdateName[] DICT_PROGMEM = {"Update"};
const char HAupdateClass[] DICT_PROGMEM = {"update"};
const char HAupdateIcon[] DICT_PROGMEM = {"mdi:update"};

const char HAdoUpID[] DICT_PROGMEM = {"%s_doupd"};
const char HAdoUpName[] DICT_PROGMEM = {"Start Update"};
const char HAdoUpIcon[] DICT_PROGMEM = {"mdi:update"};

const char HAsigID[] DICT_PROGMEM = {"%s_sig"};
const char HAsigIcon[] DICT_PROGMEM = {"mdi:sun-wireless"};
const char HAsigName[] DICT_PROGMEM = {"WiFi strength"};
const char HAsigClass[] DICT_PROGMEM = {"signal_strength"};
const char HAsigUnit[] DICT_PROGMEM = {"dB"};

const char HAupID[] DICT_PROGMEM = {"%s_up"};
const char HAupName[] DICT_PROGMEM = {"Uptime"};
const char HAupClass[] DICT_PROGMEM = {"duration"};

const char HAbtnLID[] DICT_PROGMEM = {"%s_btnL"};
const char HAbtnLName[] DICT_PROGMEM = {"Button left"};
const char HAbtnLIcon[] DICT_PROGMEM = {"mdi:chevron-left-box-outline"};

const char HAbtnMID[] DICT_PROGMEM = {"%s_btnM"};
const char HAbtnMName[] DICT_PROGMEM = {"Button select"};
const char HAbtnMIcon[] DICT_PROGMEM = {"mdi:circle-box-outline"};

const char HAbtnRID[] DICT_PROGMEM = {"%s_btnR"};
const char HAbtnRName[] DICT_PROGMEM = {"Button right"};
const char HAbtnRIcon[] DICT_PROGMEM = {"mdi:chevron-right-box-outline"};

const char HAramRID[] DICT_PROGMEM = {"%s_ram"};
const char HAramIcon[] DICT_PROGMEM = {"mdi:memory"};
const char HAramName[] DICT_PROGMEM = {"Free ram"};
const char HAramClass[] DICT_PROGMEM = {"data_size"};
const char HAramUnit[] DICT_PROGMEM = {"B"};


// JSON properties

const char HAipAddrRID[] DICT_PROGMEM = {"%s_ip_address"};
const char HAipAddrName[] DICT_PROGMEM = {"IP Address"};
const char HAipAddrIcon[] DICT_PROGMEM = {"mdi:wifi"};



#ifndef awtrix2_upgrade
const char BatKey[] DICT_PROGMEM = {"bat"};
const char BatRawKey[] DICT_PROGMEM = {"bat_raw"};
#endif
const char LuxKey[] DICT_PROGMEM = {"lux"};
const char LDRRawKey[] DICT_PROGMEM = {"ldr_raw"};
const char BrightnessKey[] DICT_PROGMEM = {"bri"};
const char TempKey[] DICT_PROGMEM = {"temp"};
const char HumKey[] DICT_PROGMEM = {"hum"};
const char UpTimeKey[] DICT_PROGMEM = {"uptime"};
const char SignalStrengthKey[] DICT_PROGMEM = {"wifi_signal"};
const char UpdateKey[] DICT_PROGMEM = {"up_available"};
const char MessagesKey[] DICT_PROGMEM = {"messages"};
const char VersionKey[] DICT_PROGMEM = {"version"};
const char RamKey[] DICT_PROGMEM = {"ram"};
const char IpAddrKey[] DICT_PROGMEM = {"ip_address"};
