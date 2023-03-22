#include "HASensorNumber.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../utils/HASerializer.h"

HASensorNumber::HASensorNumber(
    const char* uniqueId,
    const NumberPrecision precision
) :
    HASensor(uniqueId),
    _precision(precision),
    _currentValue()
{

}

bool HASensorNumber::setValue(const HANumeric& value, const bool force)
{
    if (value.getPrecision() != _precision) {
        return false;
    }

    if (!force && value == _currentValue) {
        return true;
    }

    if (publishValue(value)) {
        _currentValue = value;
        return true;
    }

    return false;
}

void HASensorNumber::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    HASensor::onMqttConnected();
    publishValue(_currentValue);
}

bool HASensorNumber::publishValue(const HANumeric& value)
{
    if (!value.isSet()) {
        return false;
    }

    uint8_t size = value.calculateSize();
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    value.toStr(str);

    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        str,
        true
    );
}

#endif