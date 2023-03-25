#include <MQTTManager.h>
#include "Globals.h"
#include "DisplayManager.h"
#include "ServerManager.h"
#include <ArduinoHA.h>
#include <WiFi.h>

WiFiClient espClient;
uint8_t lastBrightness;
HADevice device;
HAMqtt mqtt(espClient, device, 15);

unsigned long reconnectTimer = 0;
const unsigned long reconnectInterval = 30000; // 30 Sekunden

HALight Matrix("bri", HALight::BrightnessFeature | HALight::RGBFeature);
HASelect BriMode("BriMode");
HAButton dismiss("myButtonA");
HAButton nextApp("myButtonB");
HAButton prevApp("myButtonC");
HASensor curApp("curApp");
HASensor battery("battery");
HASensor temperature("temperature");
HASensor humidity("huminity");
HASensor illuminance("illuminance");

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
    if (sender == &dismiss)
    {
        DisplayManager.dismissNotify();
    }
    else if (sender == &nextApp)
    {
        DisplayManager.nextApp();
    }
    else if (sender == &prevApp)
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
    Matrix.setBrightness(BRIGHTNESS);
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

void MQTTManager_::setup()
{
    byte mac[6];
    WiFi.macAddress(mac);

    if (HA_DISCOVERY)
    {
        Serial.println("Starting Homeassistant discorvery");

        device.setUniqueId(mac, sizeof(mac));
        device.setName(MQTT_PREFIX.c_str());
        device.setSoftwareVersion(VERSION);
        device.setManufacturer("Blueforcer");
        device.setModel("AWTRIX Light");
        device.setAvailability(true);
        Matrix.setIcon("mdi:lightbulb");
        Matrix.setName("Matrix");
        Matrix.onStateCommand(onStateCommand);
        Matrix.onBrightnessCommand(onBrightnessCommand);
        Matrix.onRGBColorCommand(onRGBColorCommand);
        Matrix.setCurrentState(true);
        Matrix.setBRIGHTNESS(BRIGHTNESS);

        HALight::RGBColor color;
        color.red = (TEXTCOLOR_565 >> 11) << 3;
        color.green = ((TEXTCOLOR_565 >> 5) & 0x3F) << 2;
        color.blue = (TEXTCOLOR_565 & 0x1F) << 3;
        Matrix.setCurrentRGBColor(color);
        Matrix.setState(true, true);

        BriMode.setOptions("Manual;Auto"); // use semicolons as separator of options
        BriMode.onCommand(onSelectCommand);
        BriMode.setIcon("mdi:brightness-auto"); // optional
        BriMode.setName("Brightness mode");     // optional
        BriMode.setState(AUTO_BRIGHTNESS, true);

        dismiss.setIcon("mdi:bell-off");
        dismiss.setName("Dismiss notification");
        nextApp.setIcon("mdi:arrow-right-bold");
        nextApp.setName("Next app");
        prevApp.setIcon("mdi:arrow-left-bold");
        prevApp.setName("Previous app");

        dismiss.onCommand(onButtonCommand);
        nextApp.onCommand(onButtonCommand);
        prevApp.onCommand(onButtonCommand);

        curApp.setIcon("mdi:apps");
        curApp.setName("Current app");

        temperature.setIcon("mdi:thermometer");
        temperature.setName("Temperature");
        temperature.setUnitOfMeasurement("°C");

        humidity.setIcon("mdi:water-percent");
        humidity.setName("humidity");
        humidity.setUnitOfMeasurement("%");

        battery.setIcon("mdi:battery-90");
        battery.setName("Battery");
        battery.setUnitOfMeasurement("%");

        illuminance.setIcon("mdi:sun-wireless");
        illuminance.setName("Illuminance");
        illuminance.setUnitOfMeasurement("lx");
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
        curApp.setValue(value.c_str());
}

void MQTTManager_::sendStats()
{
    if (HA_DISCOVERY)
    {
        char buffer[5];
        snprintf(buffer, 5, "%d", BATTERY_PERCENT); // Formatieren von BATTERY_PERCENT als Integer
        battery.setValue(buffer);                   // Senden von BATTERY_PERCENT als const char*

        snprintf(buffer, 5, "%.0f", CURRENT_TEMP); // Formatieren von CURRENT_TEMP als Float ohne Nachkommastellen
        temperature.setValue(buffer);              // Senden von CURRENT_TEMP als const char*

        snprintf(buffer, 5, "%.0f", CURRENT_HUM); // Formatieren von CURRENT_HUM als Float ohne Nachkommastellen
        humidity.setValue(buffer);                // Senden von CURRENT_HUM als const char*

        snprintf(buffer, 5, "%.0f", CURRENT_LUX); // Formatieren von CURRENT_LUX als Double ohne Nachkommastellen
        illuminance.setValue(buffer);             // Senden von CURRENT_LUX als const char*

        BriMode.setState(AUTO_BRIGHTNESS, true);
        Matrix.setBRIGHTNESS(BRIGHTNESS);
        Matrix.setState(!MATRIX_OFF, false);
    }
}