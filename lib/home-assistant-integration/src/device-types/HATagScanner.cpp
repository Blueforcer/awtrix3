#include "HATagScanner.h"
#ifndef EX_ARDUINOHA_TAG_SCANNER

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HATagScanner::HATagScanner(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentTag), uniqueId)
{

}

bool HATagScanner::tagScanned(const char* tag)
{
    if (!tag || strlen(tag) == 0) {
        return false;
    }

    return publishOnDataTopic(AHATOFSTR(HATopic), tag);
}

void HATagScanner::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 2); // 2 - max properties nb
    _serializer->set(HASerializer::WithDevice);
    _serializer->topic(AHATOFSTR(HATopic));
}

void HATagScanner::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
}

#endif
