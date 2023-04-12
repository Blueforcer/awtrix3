#include "HALight.h"
#ifndef EX_ARDUINOHA_LIGHT

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

const uint8_t HALight::RGBStringMaxLength = 3*4; // 4 characters per color

void HALight::RGBColor::fromBuffer(const uint8_t* data, const uint16_t length)
{
    if (length > RGBStringMaxLength) {
        return;
    }

    uint8_t firstCommaPos = 0;
    uint8_t secondCommaPos = 0;

    for (uint8_t i = 0; i < length; i++) {
        if (data[i] == ',') {
            if (firstCommaPos == 0) {
                firstCommaPos = i;
            } else if (secondCommaPos == 0) {
                secondCommaPos = i;
            }
        }
    }

    if (firstCommaPos == 0 || secondCommaPos == 0) {
        return;
    }

    const uint8_t redLen = firstCommaPos;
    const uint8_t greenLen = secondCommaPos - firstCommaPos - 1; // minus comma
    const uint8_t blueLen = length - redLen - greenLen - 2; // minus two commas

    const HANumeric& r = HANumeric::fromStr(data, redLen);
    const HANumeric& g = HANumeric::fromStr(&data[redLen + 1], greenLen);
    const HANumeric& b = HANumeric::fromStr(&data[redLen + greenLen + 2], blueLen);

    if (r.isUInt8() && g.isUInt8() && b.isUInt8()) {
        red = r.toUInt8();
        green = g.toUInt8();
        blue = b.toUInt8();
        isSet = true;
    }
}

HALight::HALight(const char* uniqueId, const uint8_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentLight), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _brightnessScale(),
    _currentState(false),
    _BRIGHTNESS(0),
    _minMireds(),
    _maxMireds(),
    _currentColorTemperature(0),
    _currentRGBColor(),
    _stateCallback(nullptr),
    _brightnessCallback(nullptr),
    _colorTemperatureCallback(nullptr),
    _rgbColorCallback(nullptr)
{

}

bool HALight::setState(const bool state, const bool force)
{
    if (!force && state == _currentState) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

bool HALight::setBrightness(const uint8_t brightness, const bool force)
{
    if (!force && brightness == _BRIGHTNESS) {
        return true;
    }

    if (publishBrightness(brightness)) {
        _BRIGHTNESS = brightness;
        return true;
    }

    return false;
}

bool HALight::setColorTemperature(const uint16_t temperature, const bool force)
{
    if (!force && temperature == _currentColorTemperature) {
        return true;
    }

    if (publishColorTemperature(temperature)) {
        _currentColorTemperature = temperature;
        return true;
    }

    return false;
}

bool HALight::setRGBColor(const RGBColor& color, const bool force)
{
    if (!force && color == _currentRGBColor) {

        return true;
    }

    if (publishRGBColor(color)) {
        _currentRGBColor = color;
        return true;
    }

    return false;
}

void HALight::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 18); // 18 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_optimistic) {
        _serializer->set(
            AHATOFSTR(HAOptimisticProperty),
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    if (_features & BrightnessFeature) {
        _serializer->topic(AHATOFSTR(HABrightnessStateTopic));
        _serializer->topic(AHATOFSTR(HABrightnessCommandTopic));

        if (_brightnessScale.isSet()) {
            _serializer->set(
                AHATOFSTR(HABrightnessScaleProperty),
                &_brightnessScale,
                HASerializer::NumberPropertyType
            );
        }
    }

    if (_features & ColorTemperatureFeature) {
        _serializer->topic(AHATOFSTR(HAColorTemperatureStateTopic));
        _serializer->topic(AHATOFSTR(HAColorTemperatureCommandTopic));

        if (_minMireds.isSet()) {
            _serializer->set(
                AHATOFSTR(HAMinMiredsProperty),
                &_minMireds,
                HASerializer::NumberPropertyType
            );
        }

        if (_maxMireds.isSet()) {
            _serializer->set(
                AHATOFSTR(HAMaxMiredsProperty),
                &_maxMireds,
                HASerializer::NumberPropertyType
            );
        }
    }

    if (_features & RGBFeature) {
        _serializer->topic(AHATOFSTR(HARGBCommandTopic));
        _serializer->topic(AHATOFSTR(HARGBStateTopic));
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HALight::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
        publishBrightness(_BRIGHTNESS);
        publishColorTemperature(_currentColorTemperature);
        publishRGBColor(_currentRGBColor);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));

    if (_features & BrightnessFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HABrightnessCommandTopic));
    }

    if (_features & ColorTemperatureFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAColorTemperatureCommandTopic));
    }

    if (_features & RGBFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HARGBCommandTopic));
    }
}

void HALight::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        handleStateCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HABrightnessCommandTopic)
    )) {
        handleBrightnessCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAColorTemperatureCommandTopic)
    )) {
        handleColorTemperatureCommand(payload, length);
    } else if (
        HASerializer::compareDataTopics(
            topic,
            uniqueId(),
            AHATOFSTR(HARGBCommandTopic)
        )
    ) {
        handleRGBCommand(payload, length);
    }
}

bool HALight::publishState(const bool state)
{
    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

bool HALight::publishBrightness(const uint8_t brightness)
{
    if (!(_features & BrightnessFeature)) {
        return false;
    }

    char str[3 + 1] = {0}; // uint8_t digits with null terminator
    HANumeric(brightness, 0).toStr(str);

    return publishOnDataTopic(AHATOFSTR(HABrightnessStateTopic), str, true);
}

bool HALight::publishColorTemperature(const uint16_t temperature)
{
    if (!(_features & ColorTemperatureFeature)) {
        return false;
    }

    char str[5 + 1] = {0}; // uint16_t digits with null terminator
    HANumeric(temperature, 0).toStr(str);

    return publishOnDataTopic(AHATOFSTR(HAColorTemperatureStateTopic), str, true);
}

bool HALight::publishRGBColor(const RGBColor& color)
{
    if (!(_features & RGBFeature) || !color.isSet) {
        return false;
    }

    char str[RGBStringMaxLength] = {0};
    uint16_t len = 0;

    // append red color with comma
    len += HANumeric(color.red, 0).toStr(&str[0]);
    str[len++] = ',';

    // append green color with comma
    len += HANumeric(color.green, 0).toStr(&str[len]);
    str[len++] = ',';

    // append blue color
    HANumeric(color.blue, 0).toStr(&str[len]);

    return publishOnDataTopic(AHATOFSTR(HARGBStateTopic), str, true);
}

void HALight::handleStateCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_stateCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _stateCallback(state, this);
}

void HALight::handleBrightnessCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_brightnessCallback) {
        return;
    }

    const HANumeric& number = HANumeric::fromStr(cmd, length);
    if (number.isUInt8()) {
        _brightnessCallback(number.toUInt8(), this);
    }
}

void HALight::handleColorTemperatureCommand(
    const uint8_t* cmd,
    const uint16_t length
)
{
    if (!_colorTemperatureCallback) {
        return;
    }

    const HANumeric& number = HANumeric::fromStr(cmd, length);
    if (number.isUInt16()) {
        _colorTemperatureCallback(number.toUInt16(), this);
    }
}

void HALight::handleRGBCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_rgbColorCallback) {
        return;
    }

    RGBColor color;
    color.fromBuffer(cmd, length);

    if (color.isSet) {
        _rgbColorCallback(color, this);
    }
}

#endif
