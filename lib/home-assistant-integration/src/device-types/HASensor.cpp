#include "HASensor.h"
#ifndef EX_ARDUINOHA_SENSOR

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HASensor::HASensor(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentSensor), uniqueId),
    _deviceClass(nullptr),
    _forceUpdate(false),
    _icon(nullptr),
    _unitOfMeasurement(nullptr),
    _jsonAttributes(false)
{

}

bool HASensor::setValue(const char* value)
{
    return publishOnDataTopic(AHATOFSTR(HAStateTopic), value, true);
}

bool HASensor::publishJsonAttributes(const char* json)
{
    if (!json) {
        return false;
    }

    return publishOnDataTopic(AHATOFSTR(HAJsonAttributesTopic), json, true);
}

void HASensor::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 10); // 10 - max properties nb (incl. json_attr_t)
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _deviceClass);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(AHATOFSTR(HAUnitOfMeasurementProperty), _unitOfMeasurement);

    // optional property
    if (_forceUpdate) {
        _serializer->set(
            AHATOFSTR(HAForceUpdateProperty),
            &_forceUpdate,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));

    if (_jsonAttributes) {
        _serializer->topic(AHATOFSTR(HAJsonAttributesTopic));
    }
}

void HASensor::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
}

#endif
