#include "HAButton.h"
#ifndef EX_ARDUINOHA_BUTTON

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HAButton::HAButton(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentButton), uniqueId),
    _class(nullptr),
    _icon(nullptr),
    _retain(false),
    _commandCallback(nullptr)
{

}

void HAButton::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 8); // 8 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _class);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);

    // optional property
    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HAButton::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HAButton::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    (void)payload;
    (void)length;

    if (_commandCallback && HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        _commandCallback(this);
    }
}

#endif
