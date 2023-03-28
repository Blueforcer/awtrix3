#include <MQTTManager.h>
#include "Globals.h"
#include "DisplayManager.h"
#include "ServerManager.h"
#include <ArduinoHA.h>
#include <WiFi.h>
#include <ArduinoJson.h>

unsigned long startTime;

WiFiClient espClient;
uint8_t lastBrightness;
HADevice device;
HAMqtt mqtt(espClient, device, 18);

unsigned long reconnectTimer = 0;
const unsigned long reconnectInterval = 30000; // 30 Sekunden

HALight *Matrix = nullptr;
HASelect *BriMode = nullptr;
HAButton *dismiss = nullptr;
HAButton *nextApp = nullptr;
HAButton *prevApp = nullptr;

HASensor *curApp = nullptr;
HASensor *battery = nullptr;
HASensor *temperature = nullptr;
HASensor *humidity = nullptr;
HASensor *illuminance = nullptr;
HASensor *uptime = nullptr;
HASensor *strength = nullptr;

HASensor *version = nullptr;
HASensor *ram = nullptr;

// The getter for the instantiated singleton instance
MQTTManager_ &MQTTManager_::getInstance()
{
    static MQTTManager_ instance;
    return instance;
}

// Initialize the global shared instance
MQTTManager_ &MQTTManager = MQTTManager.getInstance();

void onButtonCommand(HAButton *sender)
{
    if (sender == dismiss)
    {
        DisplayManager.dismissNotify();
    }
    else if (sender == nextApp)
    {
        DisplayManager.nextApp();
    }
    else if (sender == prevApp)
    {
        DisplayManager.previousApp();
    }
}

void onSelectCommand(int8_t index, HASelect *sender)
{
    switch (index)
    {
    case 0:
        AUTO_BRIGHTNESS = true;
        break;
    case 1:
        AUTO_BRIGHTNESS = false;

        break;
    default:
        AUTO_BRIGHTNESS = true;
        return;
    }
    Matrix->setBrightness(BRIGHTNESS);
    saveSettings();
    sender->setState(index); // report the selected option back to the HA panel
}

void onRGBColorCommand(HALight::RGBColor color, HALight *sender)
{
    TEXTCOLOR_565 = ((color.red & 0x1F) << 11) | ((color.green & 0x3F) << 5) | (color.blue & 0x1F);
    saveSettings();
    sender->setRGBColor(color); // report color back to the Home Assistant
}

void onStateCommand(bool state, HALight *sender)
{
    if (state)
    {
        MATRIX_OFF = false;
        DisplayManager.setBrightness(lastBrightness);
    }
    else
    {
        MATRIX_OFF = true;
        lastBrightness = BRIGHTNESS;
        DisplayManager.setBrightness(0);
    }

    sender->setState(state);
}

void onBrightnessCommand(uint8_t brightness, HALight *sender)
{
    sender->setBrightness(brightness);
    if (AUTO_BRIGHTNESS)
        return;
    BRIGHTNESS = brightness;
    lastBrightness = brightness;
    saveSettings();
    DisplayManager.setBrightness(brightness);
}

void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length)
{
    String strTopic = String(topic);
    String strPayload = String((const char *)payload).substring(0, length);

    if (strTopic == MQTT_PREFIX + "/notify")
    {
        if (payload[0] != '{' || payload[length - 1] != '}')
        {
            return;
        }
        DisplayManager.generateNotification(strPayload);
        return;
    }

    if (strTopic == MQTT_PREFIX + "/timer")
    {
        DisplayManager.gererateTimer(strPayload);
        return;
    }

    if (strTopic == MQTT_PREFIX + "/notify/dismiss")
    {
        DisplayManager.dismissNotify();
        return;
    }

    if (strTopic == MQTT_PREFIX + "/switch")
    {
        DisplayManager.switchToApp(strPayload);
        return;
    }

    if (strTopic == MQTT_PREFIX + "/settings")
    {
        DisplayManager.setNewSettings(strPayload);
        return;
    }

    if (strTopic == MQTT_PREFIX + "/nextapp")
    {
        DisplayManager.nextApp();
        return;
    }

    if (strTopic == MQTT_PREFIX + "/previousapp")
    {
        DisplayManager.previousApp();
        return;
    }

    else if (strTopic.startsWith(MQTT_PREFIX + "/custom"))
    {
        String topic_str = topic;
        String prefix = MQTT_PREFIX + "/custom/";
        if (topic_str.startsWith(prefix))
        {
            topic_str = topic_str.substring(prefix.length());
        }

        DisplayManager.generateCustomPage(topic_str, strPayload);
        return;
    }
}

void onMqttConnected()
{
    String prefix = MQTT_PREFIX;
    mqtt.subscribe((prefix + String("/brightness")).c_str());
    mqtt.subscribe((prefix + String("/notify/dismiss")).c_str());
    mqtt.subscribe((prefix + String("/notify")).c_str());
    mqtt.subscribe((prefix + String("/timer")).c_str());
    mqtt.subscribe((prefix + String("/custom/#")).c_str());
    mqtt.subscribe((prefix + String("/switch")).c_str());
    mqtt.subscribe((prefix + String("/settings")).c_str());
    mqtt.subscribe((prefix + String("/previousapp")).c_str());
    mqtt.subscribe((prefix + String("/nextapp")).c_str());
    Serial.println("MQTT Connected");
}

void connect()
{
    mqtt.onMessage(onMqttMessage);
    mqtt.onConnected(onMqttConnected);

    if (MQTT_USER == "" || MQTT_PASS == "")
    {
        Serial.println("Connecting to MQTT w/o login");
        mqtt.begin(MQTT_HOST.c_str(), MQTT_PORT, nullptr, nullptr, MQTT_PREFIX.c_str());
    }
    else
    {
        Serial.println("Connecting to MQTT with login");
        mqtt.begin(MQTT_HOST.c_str(), MQTT_PORT, MQTT_USER.c_str(), MQTT_PASS.c_str(), MQTT_PREFIX.c_str());
    }
}

char matID[40], briID[40];
char btnAID[40], btnBID[40], btnCID[40], appID[40], tempID[40], humID[40], luxID[40], verID[40], batID[40], ramID[40], upID[40], sigID[40];

void MQTTManager_::setup()
{
    startTime = millis();
    if (HA_DISCOVERY)
    {
        Serial.println("Starting Homeassistant discorvery");

        uint8_t mac[6];
        WiFi.macAddress(mac);
        char *macStr = new char[18 + 1];
        snprintf(macStr, 24, "%02x%02x%02x", mac[3], mac[4], mac[5]);
        device.setUniqueId(mac, sizeof(mac));
        device.setName(uniqueID);
        device.setSoftwareVersion(VERSION);
        device.setManufacturer("Blueforcer");
        device.setModel("AWTRIX Light");
        device.setAvailability(true);

        String uniqueIDWithSuffix;

        sprintf(matID, "%s_mat", macStr);
        Matrix = new HALight(matID, HALight::BrightnessFeature | HALight::RGBFeature);

        Matrix->setIcon("mdi:lightbulb");
        Matrix->setName("Matrix");
        Matrix->onStateCommand(onStateCommand);
        Matrix->onBrightnessCommand(onBrightnessCommand);
        Matrix->onRGBColorCommand(onRGBColorCommand);
        Matrix->setCurrentState(true);
        Matrix->setBRIGHTNESS(BRIGHTNESS);

        HALight::RGBColor color;
        color.red = (TEXTCOLOR_565 >> 11) << 3;
        color.green = ((TEXTCOLOR_565 >> 5) & 0x3F) << 2;
        color.blue = (TEXTCOLOR_565 & 0x1F) << 3;
        Matrix->setCurrentRGBColor(color);
        Matrix->setState(true, true);

        sprintf(briID, "%s_bri", macStr);
        BriMode = new HASelect(briID);
        BriMode->setOptions("Manual;Auto"); // use semicolons as separator of options
        BriMode->onCommand(onSelectCommand);
        BriMode->setIcon("mdi:brightness-auto"); // optional
        BriMode->setName("Brightness mode");     // optional
        BriMode->setState(AUTO_BRIGHTNESS, true);

        sprintf(btnAID, "%s_btna", macStr);
        dismiss = new HAButton(btnAID);
        dismiss->setIcon("mdi:bell-off");
        dismiss->setName("Dismiss notification");

        sprintf(btnBID, "%s_btnb", macStr);
        nextApp = new HAButton(btnBID);
        nextApp->setIcon("mdi:arrow-right-bold");
        nextApp->setName("Next app");

        sprintf(btnCID, "%s_btnc", macStr);
        prevApp = new HAButton(btnCID);
        prevApp->setIcon("mdi:arrow-left-bold");
        prevApp->setName("Previous app");

        dismiss->onCommand(onButtonCommand);
        nextApp->onCommand(onButtonCommand);
        prevApp->onCommand(onButtonCommand);

        sprintf(appID, "%s_app", macStr);
        curApp = new HASensor(appID);
        curApp->setIcon("mdi:apps");
        curApp->setName("Current app");

        sprintf(tempID, "%s_temp", macStr);
        temperature = new HASensor(tempID);
        temperature->setIcon("mdi:thermometer");
        temperature->setName("Temperature");
        temperature->setDeviceClass("temperature");
        temperature->setUnitOfMeasurement("°C");

        sprintf(humID, "%s_hum", macStr);
        humidity = new HASensor(humID);
        humidity->setIcon("mdi:water-percent");
        humidity->setName("humidity");
        humidity->setDeviceClass("humidity");
        humidity->setUnitOfMeasurement("%");

        sprintf(batID, "%s_bat", macStr);
        battery = new HASensor(batID);
        battery->setIcon("mdi:battery-90");
        battery->setName("Battery");
        battery->setDeviceClass("battery");
        battery->setUnitOfMeasurement("%");

        sprintf(luxID, "%s_lux", macStr);
        illuminance = new HASensor(luxID);
        illuminance->setIcon("mdi:sun-wireless");
        illuminance->setName("Illuminance");
        illuminance->setDeviceClass("illuminance");
        illuminance->setUnitOfMeasurement("lx");

        sprintf(verID, "%s_ver", macStr);
        version = new HASensor(verID);
        version->setName("Version");

        sprintf(sigID, "%s_sig", macStr);
        strength = new HASensor(sigID);
        strength->setName("WiFi strength");
        strength->setDeviceClass("signal_strength");
        strength->setUnitOfMeasurement("dB");

        sprintf(upID, "%s_up", macStr);
        uptime = new HASensor(upID);
        uptime->setName("Uptime");
        uptime->setDeviceClass("duration");

        sprintf(ramID, "%s_ram", macStr);
        ram = new HASensor(ramID);
        ram->setDeviceClass("data_size");
        ram->setIcon("mdi:application-cog");
        ram->setName("Free ram");
        ram->setUnitOfMeasurement("B");
    }
    else
    {
        Serial.println("Homeassistant discovery disabled");
        mqtt.disableHA();
    }
    connect();
}

void MQTTManager_::tick()
{
    if (MQTT_HOST != "")
    {
        mqtt.loop();
    }
}

void MQTTManager_::publish(const char *topic, const char *payload)
{
    if (!mqtt.isConnected())
        return;
    char result[100];
    strcpy(result, MQTT_PREFIX.c_str());
    strcat(result, "/");
    strcat(result, topic);
    mqtt.publish(result, payload, false);
}

void MQTTManager_::setCurrentApp(String value)
{
    if (HA_DISCOVERY)
        curApp->setValue(value.c_str());
}

const char *readUptime()
{
    static char uptime[25]; // Make the array static to keep it from being destroyed when the function returns
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;
    unsigned long uptimeSeconds = elapsedTime / 1000;
    unsigned long uptimeMinutes = uptimeSeconds / 60;
    unsigned long uptimeHours = uptimeMinutes / 60;
    unsigned long uptimeDays = uptimeHours / 24;
    unsigned long hours = uptimeHours % 24;
    unsigned long minutes = uptimeMinutes % 60;
    unsigned long seconds = uptimeSeconds % 60;
    sprintf(uptime, "P%dDT%dH%dM%dS", uptimeDays, hours, minutes, seconds);
    return uptime;
}

void MQTTManager_::sendStats()
{
    if (HA_DISCOVERY)
    {
        char buffer[5];
        snprintf(buffer, 5, "%d", BATTERY_PERCENT);
        battery->setValue(buffer);

        snprintf(buffer, 5, "%.0f", CURRENT_TEMP);
        temperature->setValue(buffer);

        snprintf(buffer, 5, "%.0f", CURRENT_HUM);
        humidity->setValue(buffer);

        snprintf(buffer, 5, "%.0f", CURRENT_LUX);
        illuminance->setValue(buffer);

        BriMode->setState(AUTO_BRIGHTNESS, true);
        Matrix->setBRIGHTNESS(BRIGHTNESS);
        Matrix->setState(!MATRIX_OFF, false);

        int8_t rssiValue = WiFi.RSSI();
        char rssiString[4];
        snprintf(rssiString, sizeof(rssiString), "%d", rssiValue);
        strength->setValue(rssiString);

        char rambuffer[10];
        int freeHeapBytes = ESP.getFreeHeap();
        itoa(freeHeapBytes, rambuffer, 10);
        ram->setValue(rambuffer);
        uptime->setValue(readUptime());
        version->setValue(VERSION);
    }

    StaticJsonDocument<200> doc;
    char buffer[5];
    doc["bat"] = BATTERY_PERCENT;
    doc["batraw"] = BATTERY_RAW;
    snprintf(buffer, 5, "%.0f", CURRENT_LUX);
    doc["lux"] = buffer;
    doc["ldrraw"] = LDR_RAW;
    doc["bri"] = BRIGHTNESS;
    snprintf(buffer, 5, "%.0f", CURRENT_TEMP);
    doc["temp"] = buffer;
    snprintf(buffer, 5, "%.0f", CURRENT_HUM);
    doc["hum"] = buffer;
    doc["uptime"] = readUptime();
    doc["wifi"] = WiFi.RSSI();
    String jsonString;
    serializeJson(doc, jsonString);
    char topic[50];
    strcpy(topic, MQTT_PREFIX.c_str());
    strcat(topic, "/stats");
    mqtt.publish(topic, jsonString.c_str());
}