#include <Arduino.h>
#include "Dictionary.h"
 
// MQTT
const char StatsTopic[] PROGMEM = {"stats"};
const char ButtonLeftTopic[] PROGMEM = {"stats/buttonLeft"};
const char ButtonSelectTopic[] PROGMEM = {"stats/buttonSelect"};
const char ButtonRightTopic[] PROGMEM = {"stats/buttonRight"};

// Generic
const char State0[] PROGMEM = {"0"};
const char State1[] PROGMEM = {"1"};

// HA
const char HAmanufacturer[] PROGMEM = {"Blueforcer"};
const char HAmodel[] PROGMEM = {"AWTRIX 3"};
const char HAmatID[] PROGMEM = {"%s_mat"};
const char HAmatIcon[] PROGMEM = {"mdi:clock-digital"};
const char HAmatName[] PROGMEM = {"Matrix"};

const char HAi1ID[] PROGMEM = {"%s_ind1"};
const char HAi1Icon[] PROGMEM = {"mdi:arrow-top-right-thick"};
const char HAi1Name[] PROGMEM = {"Indicator 1"};

const char HAi2ID[] PROGMEM = {"%s_ind2"};
const char HAi2Icon[] PROGMEM = {"mdi:arrow-right-thick"};
const char HAi2Name[] PROGMEM = {"Indicator 2"};

const char HAi3ID[] PROGMEM = {"%s_ind3"};
const char HAi3Icon[] PROGMEM = {"mdi:arrow-bottom-right-thick"};
const char HAi3Name[] PROGMEM = {"Indicator 3"};

const char HASPEEDID[] PROGMEM = {"%s_sspeed"};
const char HASPEEDIcon[] PROGMEM = {"mdi:speedometer"};
const char HASPEEDName[] PROGMEM = {"Scrollspeed"};

const char HAbriID[] PROGMEM = {"%s_bri"};
const char HAbriIcon[] PROGMEM = {"mdi:brightness-auto"};
const char HAbriName[] PROGMEM = {"Brightness mode"};
const char HAbriOptions[] PROGMEM = {"Manual;Auto"};

const char HAeffectID[] PROGMEM = {"%s_eff"};
const char HAeffectIcon[] PROGMEM = {"mdi:auto-fix"};
const char HAeffectName[] PROGMEM = {"Transition effect"};
const char HAeffectOptions[] PROGMEM = {"Random;Slide;Dim;Zoom;Rotate;Pixelate;Curtain;Ripple;Blink;Reload;Fade;Instant"};

const char HAbtnaID[] PROGMEM = {"%s_btna"};
const char HAbtnaIcon[] PROGMEM = {"mdi:bell-off"};
const char HAbtnaName[] PROGMEM = {"Dismiss notification"};

const char HAbtnbID[] PROGMEM = {"%s_btnb"};
const char HAbtnbIcon[] PROGMEM = {"mdi:arrow-right-bold"};
const char HAbtnbName[] PROGMEM = {"Next app"};

const char HAbtncID[] PROGMEM = {"%s_btnc"};
const char HAbtncIcon[] PROGMEM = {"mdi:arrow-left-bold"};
const char HAbtncName[] PROGMEM = {"Previous app"};

const char HAappID[] PROGMEM = {"%s_app"};
const char HAappIcon[] PROGMEM = {"mdi:apps"};
const char HAappName[] PROGMEM = {"Current app"};


const char HAIDID[] PROGMEM = {"%s_id"};
const char HAIDIcon[] PROGMEM = {"mdi:id-card"};
const char HAIDName[] PROGMEM = {"Device topic"};


const char HAtempID[] PROGMEM = {"%s_temp"};
const char HAtempIcon[] PROGMEM = {"mdi:thermometer"};
const char HAtempName[] PROGMEM = {"Temperature"};
const char HAtempClass[] PROGMEM = {"temperature"};
const char HAtempUnit[] PROGMEM = {"°C"};

const char HAhumID[] PROGMEM = {"%s_hum"};
const char HAhumIcon[] PROGMEM = {"mdi:water-percent"};
const char HAhumName[] PROGMEM = {"Humidity"};
const char HAhumClass[] PROGMEM = {"humidity"};
const char HAhumUnit[] PROGMEM = {"%"};

const char HAbatID[] PROGMEM = {"%s_bat"};
const char HAbatIcon[] PROGMEM = {"mdi:battery-90"};
const char HAbatName[] PROGMEM = {"Battery"};
const char HAbatClass[] PROGMEM = {"battery"};
const char HAbatUnit[] PROGMEM = {"%"};

const char HAluxID[] PROGMEM = {"%s_lux"};
const char HAluxIcon[] PROGMEM = {"mdi:sun-wireless"};
const char HAluxName[] PROGMEM = {"Illuminance"};
const char HAluxClass[] PROGMEM = {"illuminance"};
const char HAluxUnit[] PROGMEM = {"lx"};

const char HAverID[] PROGMEM = {"%s_ver"};
const char HAverName[] PROGMEM = {"Version"};

const char HAtransID[] PROGMEM = {"%s_tra"};
const char HAtransName[] PROGMEM = {"Transition"};
const char HAtransIcon[] PROGMEM = {"mdi:swap-horizontal"};

const char HAupdateID[] PROGMEM = {"%s_upd"};
const char HAupdateName[] PROGMEM = {"Update"};
const char HAupdateClass[] PROGMEM = {"update"};
const char HAupdateIcon[] PROGMEM = {"mdi:update"};

const char HAdoUpID[] PROGMEM = {"%s_doupd"};
const char HAdoUpName[] PROGMEM = {"Start Update"};
const char HAdoUpIcon[] PROGMEM = {"mdi:update"};

const char HAsigID[] PROGMEM = {"%s_sig"};
const char HAsigIcon[] PROGMEM = {"mdi:sun-wireless"};
const char HAsigName[] PROGMEM = {"WiFi strength"};
const char HAsigClass[] PROGMEM = {"signal_strength"};
const char HAsigUnit[] PROGMEM = {"dB"};

const char HAupID[] PROGMEM = {"%s_up"};
const char HAupName[] PROGMEM = {"Uptime"};
const char HAupClass[] PROGMEM = {"duration"};

const char HAbtnLID[] PROGMEM = {"%s_btnL"};
const char HAbtnLName[] PROGMEM = {"Button left"};
const char HAbtnLIcon[] PROGMEM = {"mdi:chevron-left-box-outline"};

const char HAbtnMID[] PROGMEM = {"%s_btnM"};
const char HAbtnMName[] PROGMEM = {"Button select"};
const char HAbtnMIcon[] PROGMEM = {"mdi:circle-box-outline"};

const char HAbtnRID[] PROGMEM = {"%s_btnR"};
const char HAbtnRName[] PROGMEM = {"Button right"};
const char HAbtnRIcon[] PROGMEM = {"mdi:chevron-right-box-outline"};

const char HAramRID[] PROGMEM = {"%s_ram"};
const char HAramIcon[] PROGMEM = {"mdi:memory"};
const char HAramName[] PROGMEM = {"Free ram"};
const char HAramClass[] PROGMEM = {"data_size"};
const char HAramUnit[] PROGMEM = {"B"};


// JSON properties

const char HAipAddrRID[] PROGMEM = {"%s_ip_address"};
const char HAipAddrName[] PROGMEM = {"IP Address"};
const char HAipAddrIcon[] PROGMEM = {"mdi:wifi"};



#ifndef awtrix2_upgrade
const char BatKey[] PROGMEM = {"bat"};
const char BatRawKey[] PROGMEM = {"bat_raw"};
#endif
const char LuxKey[] PROGMEM = {"lux"};
const char LDRRawKey[] PROGMEM = {"ldr_raw"};
const char BrightnessKey[] PROGMEM = {"bri"};
const char TempKey[] PROGMEM = {"temp"};
const char HumKey[] PROGMEM = {"hum"};
const char UpTimeKey[] PROGMEM = {"uptime"};
const char SignalStrengthKey[] PROGMEM = {"wifi_signal"};
const char UpdateKey[] PROGMEM = {"up_available"};
const char MessagesKey[] PROGMEM = {"messages"};
const char VersionKey[] PROGMEM = {"version"};
const char RamKey[] PROGMEM = {"ram"};
const char IpAddrKey[] PROGMEM = {"ip_address"};
