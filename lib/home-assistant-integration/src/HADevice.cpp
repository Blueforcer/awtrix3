#include "ArduinoHADefines.h"
#include "HADevice.h"
#include "HAMqtt.h"
#include "utils/HAUtils.h"
#include "utils/HASerializer.h"

#define HADEVICE_INIT \
    _ownsUniqueId(false), \
    _serializer(new HASerializer(nullptr, 6)), \
    _availabilityTopic(nullptr), \
    _sharedAvailability(false), \
    _available(true) // device will be available by default

HADevice::HADevice() :
    _uniqueId(nullptr),
    HADEVICE_INIT
{

}

HADevice::HADevice(const char* uniqueId) :
    _uniqueId(uniqueId),
    HADEVICE_INIT
{
    _serializer->set(AHATOFSTR(HADeviceIdentifiersProperty), _uniqueId);
}

HADevice::HADevice(const byte* uniqueId, const uint16_t length) :
    _uniqueId(HAUtils::byteArrayToStr(uniqueId, length)),
    HADEVICE_INIT
{
    _ownsUniqueId = true;
    _serializer->set(AHATOFSTR(HADeviceIdentifiersProperty), _uniqueId);
}

HADevice::~HADevice()
{
    delete _serializer;

    if (_availabilityTopic) {
        delete _availabilityTopic;
    }

    if (_ownsUniqueId) {
        delete[] _uniqueId;
    }
}

bool HADevice::setUniqueId(const byte* uniqueId, const uint16_t length)
{
    if (_uniqueId) {
        return false; // unique ID cannot be changed at runtime once it's set
    }

    _uniqueId = HAUtils::byteArrayToStr(uniqueId, length);
    _ownsUniqueId = true;
    _serializer->set(AHATOFSTR(HADeviceIdentifiersProperty), _uniqueId);
    return true;
}

void HADevice::setManufacturer(const char* manufacturer)
{
    _serializer->set(AHATOFSTR(HADeviceManufacturerProperty), manufacturer);
}

void HADevice::setModel(const char* model)
{
    _serializer->set(AHATOFSTR(HADeviceModelProperty), model);
}

void HADevice::setURL(const char* url)
{
    _serializer->set(AHATOFSTR(HAIPProperty), url);
}

void HADevice::setName(const char* name)
{
    _serializer->set(AHATOFSTR(HANameProperty), name);
}

void HADevice::setSoftwareVersion(const char* softwareVersion)
{
    _serializer->set(
        AHATOFSTR(HADeviceSoftwareVersionProperty),
        softwareVersion
    );
}

void HADevice::setAvailability(bool online)
{
    _available = online;
    publishAvailability();
}

bool HADevice::enableSharedAvailability()
{
    if (_sharedAvailability) {
        return true; // already enabled
    }

    const uint16_t topicLength = HASerializer::calculateDataTopicLength(
        nullptr,
        AHATOFSTR(HAAvailabilityTopic)
    );
    if (topicLength == 0) {
        return false;
    }

    _availabilityTopic = new char[topicLength];

    if (HASerializer::generateDataTopic(
        _availabilityTopic,
        nullptr,
        AHATOFSTR(HAAvailabilityTopic)
    ) > 0) {
        _sharedAvailability = true;
        return true;
    }

    return false;
}

void HADevice::enableLastWill()
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (!mqtt || !_availabilityTopic) {
        return;
    }

    mqtt->setLastWill(
        _availabilityTopic,
        "offline",
        true
    );
}

void HADevice::publishAvailability() const
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (!_availabilityTopic || !mqtt) {
        return;
    }

    const char* payload = _available ? HAOnline : HAOffline;
    const uint16_t length = strlen_P(payload);

    if (mqtt->beginPublish(_availabilityTopic, length, true)) {
        mqtt->writePayload(AHATOFSTR(payload));
        mqtt->endPublish();
    }
}
