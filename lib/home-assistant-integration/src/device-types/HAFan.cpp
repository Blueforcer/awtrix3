#include "HAFan.h"
#ifndef EX_ARDUINOHA_FAN

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

HAFan::HAFan(const char* uniqueId, const uint8_t features) :
    HABaseDeviceType(AHATOFSTR(HAComponentFan), uniqueId),
    _features(features),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _speedRangeMax(),
    _speedRangeMin(),
    _currentState(false),
    _currentSpeed(0),
    _stateCallback(nullptr),
    _speedCallback(nullptr)
{

}

bool HAFan::setState(const bool state, const bool force)
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

bool HAFan::setSpeed(const uint16_t speed, const bool force)
{
    if (!force && speed == _currentSpeed) {
        return true;
    }

    if (publishSpeed(speed)) {
        _currentSpeed = speed;
        return true;
    }

    return false;
}

void HAFan::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 13); // 13 - max properties nb
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

    if (_features & SpeedsFeature) {
        _serializer->topic(AHATOFSTR(HAPercentageStateTopic));
        _serializer->topic(AHATOFSTR(HAPercentageCommandTopic));

        if (_speedRangeMax.isSet()) {
            _serializer->set(
                AHATOFSTR(HASpeedRangeMaxProperty),
                &_speedRangeMax,
                HASerializer::NumberPropertyType
            );
        }

        if (_speedRangeMin.isSet()) {
            _serializer->set(
                AHATOFSTR(HASpeedRangeMinProperty),
                &_speedRangeMin,
                HASerializer::NumberPropertyType
            );
        }
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HAFan::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
        publishSpeed(_currentSpeed);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));

    if (_features & SpeedsFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAPercentageCommandTopic));
    }
}

void HAFan::onMqttMessage(
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
        AHATOFSTR(HAPercentageCommandTopic)
    )) {
        handleSpeedCommand(payload, length);
    }
}

bool HAFan::publishState(const bool state)
{
    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

bool HAFan::publishSpeed(const uint16_t speed)
{
    if (!(_features & SpeedsFeature)) {
        return false;
    }

    char str[5 + 1] = {0}; // uint16_t digits with null terminator
    HANumeric(speed, 0).toStr(str);

    return publishOnDataTopic(AHATOFSTR(HAPercentageStateTopic), str, true);
}

void HAFan::handleStateCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_stateCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _stateCallback(state, this);
}

void HAFan::handleSpeedCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_speedCallback) {
        return;
    }

    const HANumeric& number = HANumeric::fromStr(cmd, length);
    if (number.isUInt16()) {
        _speedCallback(number.toUInt16(), this);
    }
}

#endif
