#include "HAScene.h"
#ifndef EX_ARDUINOHA_SCENE

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HAScene::HAScene(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentScene), uniqueId),
    _icon(nullptr),
    _retain(false),
    _commandCallback(nullptr)
{

}

void HAScene::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 7); // 7 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);

    // optional property
    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    // HA 2022.10 throws an exception if this property is not set
    _serializer->set(
        AHATOFSTR(HAPayloadOnProperty),
        AHATOFSTR(HAStateOn),
        HASerializer::ProgmemPropertyValue
    );

    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HAScene::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HAScene::onMqttMessage(
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
