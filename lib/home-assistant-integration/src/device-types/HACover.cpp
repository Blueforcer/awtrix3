#include "HACover.h"
#ifndef EX_ARDUINOHA_COVER

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HANumeric.h"
#include "../utils/HASerializer.h"

HACover::HACover(const char* uniqueId, const Features features) :
    HABaseDeviceType(AHATOFSTR(HAComponentCover), uniqueId),
    _features(features),
    _currentState(StateUnknown),
    _currentPosition(DefaultPosition),
    _class(nullptr),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _commandCallback(nullptr)
{

}

bool HACover::setState(const CoverState state, const bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

bool HACover::setPosition(const int16_t position, const bool force)
{
    if (!force && _currentPosition == position) {
        return true;
    }

    if (publishPosition(position)) {
        _currentPosition = position;
        return true;
    }

    return false;
}

void HACover::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 11); // 11 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
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

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));

    if (_features & PositionFeature) {
        _serializer->topic(AHATOFSTR(HAPositionTopic));
    }
}

void HACover::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
        publishPosition(_currentPosition);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HACover::onMqttMessage(
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
        handleCommand(payload, length);
    }
}

bool HACover::publishState(CoverState state)
{
    if (state == StateUnknown) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (state) {
    case StateClosed:
        stateStr = AHATOFSTR(HAClosedState);
        break;

    case StateClosing:
        stateStr = AHATOFSTR(HAClosingState);
        break;

    case StateOpen:
        stateStr = AHATOFSTR(HAOpenState);
        break;

    case StateOpening:
        stateStr = AHATOFSTR(HAOpeningState);
        break;

    case StateStopped:
        stateStr = AHATOFSTR(HAStoppedState);
        break;

    default:
        return false;
    }

    return publishOnDataTopic(AHATOFSTR(HAStateTopic), stateStr, true);
}

bool HACover::publishPosition(int16_t position)
{
    if (position == DefaultPosition || !(_features & PositionFeature)) {
        return false;
    }

    char str[6 + 1] = {0}; // int16_t digits with null terminator
    HANumeric(position, 0).toStr(str);

    return publishOnDataTopic(AHATOFSTR(HAPositionTopic), str, true);
}

void HACover::handleCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_commandCallback) {
        return;
    }

    if (memcmp_P(cmd, HACloseCommand, length) == 0) {
        _commandCallback(CommandClose, this);
    } else if (memcmp_P(cmd, HAOpenCommand, length) == 0) {
        _commandCallback(CommandOpen, this);
    } else if (memcmp_P(cmd, HAStopCommand, length) == 0) {
        _commandCallback(CommandStop, this);
    }
}

#endif
