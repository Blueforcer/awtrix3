#include <MQTTManager.h>
#include "Globals.h"
#include "DisplayManager.h"
#include "ServerManager.h"
#include <ArduinoHA.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Dictionary.h"
#include "PeripheryManager.h"
#include "UpdateManager.h"

WiFiClient espClient;
HADevice device;
HAMqtt mqtt(espClient, device, 25);

HALight *Matrix, *Indikator1, *Indikator2 = nullptr;
HASelect *BriMode = nullptr;
HAButton *dismiss, *nextApp, *prevApp, *doUpdate = nullptr;
HASwitch *transition = nullptr;
#ifdef ULANZI
HASensor *battery = nullptr;
#endif
HASensor *temperature, *humidity, *illuminance, *uptime, *strength, *version, *ram, *curApp = nullptr;
HABinarySensor *btnleft, *btnmid, *btnright = nullptr;

char matID[40], ind1ID[40], ind2ID[40], briID[40], btnAID[40], btnBID[40], btnCID[40], appID[40], tempID[40], humID[40], luxID[40], verID[40], ramID[40], upID[40], sigID[40], btnLID[40], btnMID[40], btnRID[40], transID[40], doUpdateID[40], batID[40];

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
    else if (sender == doUpdate)
    {
        if (UPDATE_AVAILABLE)
            UpdateManager.updateFirmware();
    }
}

void onSwitchCommand(bool state, HASwitch *sender)
{
    AUTO_TRANSITION = state;
    DisplayManager.setAutoTransition(state);
    saveSettings();
    sender->setState(state);
}

void onSelectCommand(int8_t index, HASelect *sender)
{

    sender->setState(index); // report the selected option back to the HA panel
    switch (index)
    {
    case 0:
        AUTO_BRIGHTNESS = false;
        Matrix->setBrightness(BRIGHTNESS, true);
        break;
    case 1:
        AUTO_BRIGHTNESS = true;
        break;
    }

    saveSettings();
}

void onRGBColorCommand(HALight::RGBColor color, HALight *sender)
{
    if (sender == Matrix)
    {
        TEXTCOLOR_565 = ((color.red & 0x1F) << 11) | ((color.green & 0x3F) << 5) | (color.blue & 0x1F);
        saveSettings();
    }
    else if (sender == Indikator1)
    {
        DisplayManager.setIndicator1Color(((color.red & 0x1F) << 11) | ((color.green & 0x3F) << 5) | (color.blue & 0x1F));
    }
    else if (sender == Indikator2)
    {
        DisplayManager.setIndicator2Color(((color.red & 0x1F) << 11) | ((color.green & 0x3F) << 5) | (color.blue & 0x1F));
    }
    sender->setRGBColor(color); // report color back to the Home Assistant
}

void onStateCommand(bool state, HALight *sender)
{
    if (sender == Matrix)
    {
        DisplayManager.setPower(state);
    }
    else if (sender == Indikator1)
    {
        DisplayManager.setIndicator1State(state);
    }
    else if (sender == Indikator2)
    {
        DisplayManager.setIndicator2State(state);
    }
    sender->setState(state);
}

void onBrightnessCommand(uint8_t brightness, HALight *sender)
{
    sender->setBrightness(brightness);
    if (AUTO_BRIGHTNESS)
        return;
    BRIGHTNESS = brightness;
    saveSettings();
    DisplayManager.setBrightness(brightness);
}

void onMqttMessage(const char *topic, const uint8_t *payload, uint16_t length)
{
    DEBUG_PRINTF("MQTT message received at topic %s", topic);
    String strTopic = String(topic);
    char *payloadCopy = new char[length + 1];
    memcpy(payloadCopy, payload, length);
    payloadCopy[length] = '\0';
    DEBUG_PRINTF("Payload:  %s", payloadCopy);
    ++RECEIVED_MESSAGES;
    if (strTopic.equals(MQTT_PREFIX + "/notify"))
    {
        if (payload[0] != '{' || payload[length - 1] != '}')
        {
            delete[] payloadCopy;
            return;
        }
        DisplayManager.generateNotification(payloadCopy);
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/timer"))
    {
        DisplayManager.gererateTimer(payloadCopy);
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/notify/dismiss"))
    {
        DisplayManager.dismissNotify();
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/apps"))
    {
        DisplayManager.updateAppVector(payloadCopy);
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/switch"))
    {
        DisplayManager.switchToApp(payloadCopy);
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/settings"))
    {
        DisplayManager.setNewSettings(payloadCopy);
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/nextapp"))
    {
        DisplayManager.nextApp();
        delete[] payloadCopy;
        return;
    }

    if (strTopic.equals(MQTT_PREFIX + "/previousapp"))
    {
        DisplayManager.previousApp();
        delete[] payloadCopy;
        return;
    }
    if (strTopic.equals(MQTT_PREFIX + "/doupdate"))
    {
        if (UpdateManager.checkUpdate(true))
        {
            UpdateManager.updateFirmware();
        }
        delete[] payloadCopy;
        return;
    }
    if (strTopic.equals(MQTT_PREFIX + "/power"))
    {
        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error)
        {
            DEBUG_PRINTLN(F("Failed to parse json"));
            return;
        }
        if (doc.containsKey("power"))
        {
            DisplayManager.setPower(doc["power"].as<bool>());
        }

        delete[] payloadCopy;
        return;
    }
    if (strTopic.equals(MQTT_PREFIX + "/indicator1"))
    {
        DisplayManager.indicatorParser(1, payloadCopy);
        delete[] payloadCopy;
        return;
    }
    if (strTopic.equals(MQTT_PREFIX + "/indicator2"))
    {
        DisplayManager.indicatorParser(2, payloadCopy);
        delete[] payloadCopy;
        return;
    }
    if (strTopic.equals(MQTT_PREFIX + "/reboot"))
    {
        DEBUG_PRINTLN("REBOOT COMMAND RECEIVED")
        delay(1000);
        ESP.restart();
        delete[] payloadCopy;
        return;
    }
    if (strTopic.equals(MQTT_PREFIX + "/sound"))
    {
        PeripheryManager.parseSound(payloadCopy);
        delete[] payloadCopy;
        return;
    }
    if (strTopic.startsWith(MQTT_PREFIX + "/custom"))
    {
        String topic_str = topic;
        String prefix = MQTT_PREFIX + "/custom/";
        if (topic_str.startsWith(prefix))
        {
            topic_str = topic_str.substring(prefix.length());
            DisplayManager.generateCustomPage(topic_str, payloadCopy);
        }

        delete[] payloadCopy;
        return;
    }
    DEBUG_PRINTLN(F("Unknown MQTT command!"));
}

void onMqttConnected()
{
    DEBUG_PRINTLN(F("MQTT Connected"));
    String prefix = MQTT_PREFIX;
    const char *topics[] PROGMEM = {
        "/brightness",
        "/notify/dismiss",
        "/notify",
        "/timer",
        "/custom/#",
        "/switch",
        "/settings",
        "/previousapp",
        "/nextapp",
        "/doupdate",
        "/nextapp",
        "/apps",
        "/power",
        "/indicator1",
        "/indicator2",
        "/timeformat",
        "/dateformat",
        "/reboot",
        "/sound"};
    for (const char *topic : topics)
    {
        DEBUG_PRINTF("Subscribe to topic %s", topic);
        String fullTopic = prefix + topic;
        mqtt.subscribe(fullTopic.c_str());
    }
}

void connect()
{
    mqtt.onMessage(onMqttMessage);
    mqtt.onConnected(onMqttConnected);

    if (MQTT_USER == "" || MQTT_PASS == "")
    {
        DEBUG_PRINTLN(F("Connecting to MQTT w/o login"));
        mqtt.begin(MQTT_HOST.c_str(), MQTT_PORT, nullptr, nullptr, MQTT_PREFIX.c_str());
    }
    else
    {
        DEBUG_PRINTLN(F("Connecting to MQTT with login"));
        mqtt.begin(MQTT_HOST.c_str(), MQTT_PORT, MQTT_USER.c_str(), MQTT_PASS.c_str(), MQTT_PREFIX.c_str());
    }
}

void MQTTManager_::setup()
{
    if (HA_DISCOVERY)
    {
        DEBUG_PRINTLN(F("Starting Homeassistant discovery"));

        uint8_t mac[6];
        WiFi.macAddress(mac);
        char *macStr = new char[18 + 1];
        snprintf(macStr, 24, "%02x%02x%02x", mac[3], mac[4], mac[5]);
        device.setUniqueId(mac, sizeof(mac));
        device.setName(uniqueID);
        device.setSoftwareVersion(VERSION);
        device.setManufacturer(HAmanufacturer);
        device.setModel(HAmodel);
        device.setAvailability(true);
        device.enableSharedAvailability();
        device.enableLastWill();

        String uniqueIDWithSuffix;

        sprintf(matID, HAmatID, macStr);
        Matrix = new HALight(matID, HALight::BrightnessFeature | HALight::RGBFeature);

        Matrix->setIcon(HAmatIcon);
        Matrix->setName(HAmatName);
        Matrix->onStateCommand(onStateCommand);
        Matrix->onBrightnessCommand(onBrightnessCommand);
        Matrix->onRGBColorCommand(onRGBColorCommand);
        Matrix->setCurrentState(true);
        Matrix->setBRIGHTNESS(BRIGHTNESS);

        HALight::RGBColor color;
        color.isSet = true;
        color.red = (TEXTCOLOR_565 >> 11) & 0x1F;  // Bitverschiebung um 11 Bits und Maskierung mit 0x1F
        color.green = (TEXTCOLOR_565 >> 5) & 0x3F; // Bitverschiebung um 5 Bits und Maskierung mit 0x3F
        color.blue = TEXTCOLOR_565 & 0x1F;         // Maskierung mit 0x1F
        color.red <<= 3;
        color.green <<= 2;
        color.blue <<= 3;
        Matrix->setCurrentRGBColor(color);
        Matrix->setState(true, true);

        sprintf(ind1ID, HAi1ID, macStr);
        Indikator1 = new HALight(ind1ID, HALight::RGBFeature);
        Indikator1->setIcon(HAi1Icon);
        Indikator1->setName(HAi1Name);
        Indikator1->onStateCommand(onStateCommand);
        Indikator1->onRGBColorCommand(onRGBColorCommand);

        sprintf(ind2ID, HAi2ID, macStr);
        Indikator2 = new HALight(ind2ID, HALight::RGBFeature);
        Indikator2->setIcon(HAi2Icon);
        Indikator2->setName(HAi2Name);
        Indikator2->onStateCommand(onStateCommand);
        Indikator2->onRGBColorCommand(onRGBColorCommand);

        sprintf(briID, HAbriID, macStr);
        BriMode = new HASelect(briID);
        BriMode->setOptions(HAbriOptions);
        BriMode->onCommand(onSelectCommand);
        BriMode->setIcon(HAbriIcon);
        BriMode->setName(HAbriName);
        BriMode->setState(AUTO_BRIGHTNESS, true);

        sprintf(btnAID, HAbtnaID, macStr);
        dismiss = new HAButton(btnAID);
        dismiss->setIcon(HAbtnaIcon);
        dismiss->setName(HAbtnaName);

        sprintf(doUpdateID, HAdoUpID, macStr);
        doUpdate = new HAButton(doUpdateID);
        doUpdate->setIcon(HAdoUpIcon);
        doUpdate->setName(HAdoUpName);
        doUpdate->onCommand(onButtonCommand);

        sprintf(transID, HAtransID, macStr);
        transition = new HASwitch(transID);
        transition->setIcon(HAtransIcon);
        transition->setName(HAtransName);
        transition->onCommand(onSwitchCommand);

        sprintf(appID, HAappID, macStr);
        curApp = new HASensor(appID);
        curApp->setIcon(HAappIcon);
        curApp->setName(HAappName);

        sprintf(btnBID, HAbtnbID, macStr);
        nextApp = new HAButton(btnBID);
        nextApp->setIcon(HAbtnbIcon);
        nextApp->setName(HAbtnbName);

        sprintf(btnCID, HAbtncID, macStr);
        prevApp = new HAButton(btnCID);
        prevApp->setIcon(HAbtncIcon);
        prevApp->setName(HAbtncName);

        dismiss->onCommand(onButtonCommand);
        nextApp->onCommand(onButtonCommand);
        prevApp->onCommand(onButtonCommand);

        sprintf(tempID, HAtempID, macStr);
        temperature = new HASensor(tempID);
        temperature->setIcon(HAtempIcon);
        temperature->setName(HAtempName);
        temperature->setDeviceClass(HAtempClass);
        temperature->setUnitOfMeasurement(HAtempUnit);

        sprintf(humID, HAhumID, macStr);
        humidity = new HASensor(humID);
        humidity->setIcon(HAhumIcon);
        humidity->setName(HAhumName);
        humidity->setDeviceClass(HAhumClass);
        humidity->setUnitOfMeasurement(HAhumUnit);

#ifdef ULANZI
        sprintf(batID, HAbatID, macStr);
        battery = new HASensor(batID);
        battery->setIcon(HAbatIcon);
        battery->setName(HAbatName);
        battery->setDeviceClass(HAbatClass);
        battery->setUnitOfMeasurement(HAbatUnit);

#endif
        sprintf(luxID, HAluxID, macStr);
        illuminance = new HASensor(luxID);
        illuminance->setIcon(HAluxIcon);
        illuminance->setName(HAluxName);
        illuminance->setDeviceClass(HAluxClass);
        illuminance->setUnitOfMeasurement(HAluxUnit);

        sprintf(verID, HAverID, macStr);
        version = new HASensor(verID);
        version->setName(HAverName);

        sprintf(sigID, HAsigID, macStr);
        strength = new HASensor(sigID);
        strength->setName(HAsigName);
        strength->setDeviceClass(HAsigClass);
        strength->setUnitOfMeasurement(HAsigUnit);

        sprintf(upID, HAupID, macStr);
        uptime = new HASensor(upID);
        uptime->setName(HAupName);
        uptime->setDeviceClass(HAupClass);
        uptime->setUnitOfMeasurement("s");

        sprintf(btnLID, HAbtnLID, macStr);
        btnleft = new HABinarySensor(btnLID);
        btnleft->setName(HAbtnLName);

        sprintf(btnMID, HAbtnMID, macStr);
        btnmid = new HABinarySensor(btnMID);
        btnmid->setName(HAbtnMName);

        sprintf(btnRID, HAbtnRID, macStr);
        btnright = new HABinarySensor(btnRID);
        btnright->setName(HAbtnRName);

        sprintf(ramID, HAramRID, macStr);
        ram = new HASensor(ramID);
        ram->setDeviceClass(HAramClass);
        ram->setIcon(HAramIcon);
        ram->setName(HAramName);
        ram->setUnitOfMeasurement(HAramUnit);
    }
    else
    {
        Serial.println(F("Homeassistant discovery disabled"));
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

void MQTTManager_::setCurrentApp(String appName)
{
    DEBUG_PRINTF("Publish current app %s", appName.c_str());
    if (HA_DISCOVERY)
        curApp->setValue(appName.c_str());

    publish("currentApp", appName.c_str());
}

void MQTTManager_::sendStats()
{
    if (HA_DISCOVERY)
    {
        char buffer[5];
#ifdef ULANZI
        snprintf(buffer, 5, "%d", BATTERY_PERCENT);
        battery->setValue(buffer);

#endif

        if (SENSOR_READING)
        {
            snprintf(buffer, 5, "%.0f", CURRENT_TEMP);
            temperature->setValue(buffer);
            snprintf(buffer, 5, "%.0f", CURRENT_HUM);
            humidity->setValue(buffer);
        }

        snprintf(buffer, 5, "%.0f", CURRENT_LUX);
        illuminance->setValue(buffer);

        BriMode->setState(AUTO_BRIGHTNESS, false);
        Matrix->setBrightness(BRIGHTNESS);
        Matrix->setState(!MATRIX_OFF, false);
        HALight::RGBColor color;
        color.isSet = true;
        color.red = (TEXTCOLOR_565 >> 11) & 0x1F;
        color.green = (TEXTCOLOR_565 >> 5) & 0x3F;
        color.blue = TEXTCOLOR_565 & 0x1F;
        color.red <<= 3;
        color.green <<= 2;
        color.blue <<= 3;
        Matrix->setRGBColor(color);
        int8_t rssiValue = WiFi.RSSI();
        char rssiString[4];
        snprintf(rssiString, sizeof(rssiString), "%d", rssiValue);
        strength->setValue(rssiString);

        char rambuffer[10];
        int freeHeapBytes = ESP.getFreeHeap();
        itoa(freeHeapBytes, rambuffer, 10);
        ram->setValue(rambuffer);
        uptime->setValue(PeripheryManager.readUptime());

        transition->setState(AUTO_TRANSITION, false);
        version->setValue(VERSION);
        // update->setState(UPDATE_AVAILABLE, false);
    }
    else
    {
    }

    publish(StatsTopic, DisplayManager.getStats().c_str());
}

void MQTTManager_::sendButton(byte btn, bool state)
{
    static bool btn0State, btn1State, btn2State;

    switch (btn)
    {
    case 0:
        if (btn0State != state)
        {
            if (HA_DISCOVERY)
                btnleft->setState(state, false);
            btn0State = state;
            publish(ButtonLeftTopic, state ? State1 : State0);
        }
        break;
    case 1:
        if (btn1State != state)
        {
            if (HA_DISCOVERY)
                btnmid->setState(state, false);
            btn1State = state;
            publish(ButtonSelectTopic, state ? State1 : State0);
        }

        break;
    case 2:
        if (btn2State != state)
        {
            if (HA_DISCOVERY)
                btnright->setState(state, false);
            btn2State = state;
            publish(ButtonRightTopic, state ? State1 : State0);
        }
        break;
    default:
        break;
    }
}

void MQTTManager_::setIndicatorState(uint8_t indicator, bool state, uint16_t color)
{
    if (HA_DISCOVERY)
    {
        HALight::RGBColor c;
        c.isSet = true;
        c.red = (color >> 11) & 0x1F;
        c.green = (color >> 5) & 0x3F;
        c.blue = color & 0x1F;
        c.red <<= 3;
        c.green <<= 2;
        c.blue <<= 3;

        switch (indicator)
        {
        case 1:
            Indikator1->setRGBColor(c);
            Indikator1->setState(state);
            break;
        case 2:
            Indikator2->setRGBColor(c);
            Indikator2->setState(state);
            break;
        default:
            break;
        }
    }
}