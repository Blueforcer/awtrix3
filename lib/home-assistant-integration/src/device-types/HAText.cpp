#include "HAText.h"
#ifndef EX_ARDUINOHA_TEXT

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HAText::HAText(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentText), uniqueId),
    _icon(nullptr),
    _retain(false),
    _messageCallback(nullptr),
    _jsonAttributes(false)
{

}

bool HAText::setState(const char* value, bool force)
{
    return publishOnDataTopic(AHATOFSTR(HAStateTopic), value, true);
}

bool HAText::publishJsonAttributes(const char* json)
{
    if (!json) {
        return false;
    }

    return publishOnDataTopic(AHATOFSTR(HAJsonAttributesTopic), json, true);
}

void HAText::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 9); // 9 - max properties nb (incl. json_attr_t)
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

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));

    if (_jsonAttributes) {
        _serializer->topic(AHATOFSTR(HAJsonAttributesTopic));
    }
}

void HAText::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
    setState("", true);
    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HAText::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (_messageCallback && HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        static constexpr uint16_t kMaxMsg = 255;
        char buf[kMaxMsg + 1];
        uint16_t copyLen = length > kMaxMsg ? kMaxMsg : length;
        memcpy(buf, payload, copyLen);
        buf[copyLen] = '\0';
        _messageCallback(buf, copyLen, this);
    }
}

#endif
