#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>

extern const char *VERSION;
extern IPAddress local_IP;
extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress primaryDNS;
extern IPAddress secondaryDNS;
extern String MQTT_HOST;
extern uint16_t MQTT_PORT;
extern String MQTT_USER;
extern String MQTT_PASS;
extern String MQTT_PREFIX;
extern String CITY;
extern bool IO_BROKER;
extern bool NET_STATIC;
extern bool SHOW_DATE;
extern bool SHOW_WEATHER;
extern bool SHOW_BATTERY;
extern bool SHOW_TEMP;
extern bool SHOW_HUM;
extern bool SHOW_SECONDS;
extern bool SHOW_WEEKDAY;
extern String NET_IP;
extern String NET_GW;
extern String NET_SN;
extern String NET_PDNS;
extern String NET_SDNS;
extern int TIME_PER_FRAME;
extern uint8_t MATRIX_FPS;
extern int TIME_PER_TRANSITION;
extern String NTP_SERVER;
extern String NTP_TZ;
extern bool HA_DISCOVERY;
extern bool UPPERCASE_LETTERS;
extern float CURRENT_TEMP;
extern float CURRENT_HUM;
extern float CURRENT_LUX;
extern String CURRENT_APP;
extern uint8_t BATTERY_PERCENT;
extern uint8_t BRIGHTNESS;
extern uint8_t BRIGHTNESS_PERCENT;
extern String TEXTCOLOR;
extern bool AUTO_BRIGHTNESS;
extern bool AP_MODE;
extern bool ALARM_ACTIVE;
extern bool TIMER_ACTIVE;
extern bool MATRIX_OFF;
extern String ALARM_SOUND;
extern String TIMER_SOUND;
extern uint16_t TEXTCOLOR_565;
extern uint8_t SNOOZE_TIME;
extern bool AUTO_TRANSITION;

void loadSettings();
void saveSettings();
#endif // Globals_H