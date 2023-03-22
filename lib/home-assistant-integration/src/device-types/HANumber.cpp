#include "HANumber.h"
#ifndef EX_ARDUINOHA_NUMBER

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HANumber::HANumber(const char* uniqueId, const NumberPrecision precision) :
    HABaseDeviceType(AHATOFSTR(HAComponentNumber), uniqueId),
    _precision(precision),
    _class(nullptr),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _mode(ModeAuto),
    _unitOfMeasurement(nullptr),
    _minValue(),
    _maxValue(),
    _step(),
    _currentState(),
    _commandCallback(nullptr)
{

}

bool HANumber::setState(const HANumeric& state, const bool force)
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

void HANumber::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 15); // 15 - max properties nb

    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(AHATOFSTR(HAUnitOfMeasurementProperty), _unitOfMeasurement);
    _serializer->set(
        AHATOFSTR(HAModeProperty),
        getModeProperty(),
        HASerializer::ProgmemPropertyValue
    );
    _serializer->set(
        AHATOFSTR(HACommandTemplateProperty),
        getCommandTemplate(),
        HASerializer::ProgmemPropertyValue
    );

    if (_minValue.isSet()) {
        _serializer->set(
            AHATOFSTR(HAMinProperty),
            &_minValue,
            HASerializer::NumberPropertyType
        );
    }

    if (_maxValue.isSet()) {
        _serializer->set(
            AHATOFSTR(HAMaxProperty),
            &_maxValue,
            HASerializer::NumberPropertyType
        );
    }

    if (_step.isSet()) {
        _serializer->set(
            AHATOFSTR(HAStepProperty),
            &_step,
            HASerializer::NumberPropertyType
        );
    }

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
}

void HANumber::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HANumber::onMqttMessage(
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

bool HANumber::publishState(const HANumeric& state)
{
    if (!state.isSet()) {
        return publishOnDataTopic(
            AHATOFSTR(HAStateTopic),
            AHATOFSTR(HAStateNone),
            true
        );
    }

    const uint8_t size = state.calculateSize();
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    state.toStr(str);

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        str,
        true
    );
}

void HANumber::handleCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_commandCallback) {
        return;
    }

    if (memcmp_P(cmd, HAStateNone, length) == 0) {
        _commandCallback(HANumeric(), this);
    } else {
        HANumeric number = HANumeric::fromStr(cmd, length);
        if (number.isSet()) {
            number.setPrecision(_precision);
            _commandCallback(number, this);
        }
    }
}

const __FlashStringHelper* HANumber::getModeProperty() const
{
    switch (_mode) {
    case ModeBox:
        return AHATOFSTR(HAModeBox);

    case ModeSlider:
        return AHATOFSTR(HAModeSlider);

    default:
        return nullptr;
    }
}

const __FlashStringHelper* HANumber::getCommandTemplate()
{
    switch (_precision) {
    case PrecisionP1:
        return AHATOFSTR(HAValueTemplateFloatP1);

    case PrecisionP2:
        return AHATOFSTR(HAValueTemplateFloatP2);

    case PrecisionP3:
        return AHATOFSTR(HAValueTemplateFloatP3);

    default:
        return nullptr;
    }
}

#endif
