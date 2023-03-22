
#include <Arduino.h>

#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include "HASerializer.h"
#include "../ArduinoHADefines.h"
#include "../HADevice.h"
#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HANumeric.h"
#include "../device-types/HABaseDeviceType.h"

uint16_t HASerializer::calculateConfigTopicLength(
    const __FlashStringHelper* componentName,
    const char* objectId
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !componentName ||
        !objectId ||
        !mqtt ||
        !mqtt->getDiscoveryPrefix() ||
        !mqtt->getDevice()
    ) {
        return 0;
    }

    return
        strlen(mqtt->getDiscoveryPrefix()) + 1 + // prefix with slash
        strlen_P(AHAFROMFSTR(componentName)) + 1 + // component name with slash
        strlen(mqtt->getDevice()->getUniqueId()) + 1 + // device ID with slash
        strlen(objectId) + 1 + // object ID with slash
        strlen_P(HAConfigTopic) + 1; // including null terminator
}

bool HASerializer::generateConfigTopic(
    char* output,
    const __FlashStringHelper* componentName,
    const char* objectId
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !output ||
        !componentName ||
        !objectId ||
        !mqtt ||
        !mqtt->getDiscoveryPrefix() ||
        !mqtt->getDevice()
    ) {
        return false;
    }

    strcpy(output, mqtt->getDiscoveryPrefix());
    strcat_P(output, HASerializerSlash);

    strcat_P(output, AHAFROMFSTR(componentName));
    strcat_P(output, HASerializerSlash);

    strcat(output, mqtt->getDevice()->getUniqueId());
    strcat_P(output, HASerializerSlash);

    strcat(output, objectId);
    strcat_P(output, HASerializerSlash);

    strcat_P(output, HAConfigTopic);
    return true;
}

uint16_t HASerializer::calculateDataTopicLength(
    const char* objectId,
    const __FlashStringHelper* topic
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !topic ||
        !mqtt ||
        !mqtt->getDataPrefix() ||
        !mqtt->getDevice()
    ) {
        return 0;
    }

    uint16_t size =
        strlen(mqtt->getDataPrefix()) + 1 + // prefix with slash
        strlen(mqtt->getDevice()->getUniqueId()) + 1 + // device ID with slash
        strlen_P(AHAFROMFSTR(topic));

    if (objectId) {
        size += strlen(objectId) + 1; // object ID with slash;
    }

    return size + 1; // including null terminator
}

bool HASerializer::generateDataTopic(
    char* output,
    const char* objectId,
    const __FlashStringHelper* topic
)
{
    const HAMqtt* mqtt = HAMqtt::instance();
    if (
        !output ||
        !topic ||
        !mqtt ||
        !mqtt->getDataPrefix() ||
        !mqtt->getDevice()
    ) {
        return false;
    }

    strcpy(output, mqtt->getDataPrefix());
    strcat_P(output, HASerializerSlash);

    strcat(output, mqtt->getDevice()->getUniqueId());
    strcat_P(output, HASerializerSlash);

    if (objectId) {
        strcat(output, objectId);
        strcat_P(output, HASerializerSlash);
    }

    strcat_P(output, AHAFROMFSTR(topic));
    return true;
}

bool HASerializer::compareDataTopics(
    const char* actualTopic,
    const char* objectId,
    const __FlashStringHelper* topic
)
{
    if (!actualTopic) {
        return false;
    }

    const uint16_t topicLength = calculateDataTopicLength(objectId, topic);
    if (topicLength == 0) {
        return false;
    }

    char expectedTopic[topicLength];
    if (!generateDataTopic(expectedTopic, objectId, topic)) {
        return false;
    }

    return memcmp(actualTopic, expectedTopic, topicLength) == 0;
}

HASerializer::HASerializer(
    HABaseDeviceType* deviceType,
    const uint8_t maxEntriesNb
) :
    _deviceType(deviceType),
    _entriesNb(0),
    _maxEntriesNb(maxEntriesNb),
    _entries(new SerializerEntry[maxEntriesNb])
{

}

HASerializer::~HASerializer()
{
    delete[] _entries;
}

void HASerializer::set(
    const __FlashStringHelper* property,
    const void* value,
    PropertyValueType valueType
)
{
    if (!property || !value) {
        return;
    }

    SerializerEntry* entry = addEntry();
    entry->type = PropertyEntryType;
    entry->subtype = static_cast<uint8_t>(valueType);
    entry->property = property;
    entry->value = value;
}

void HASerializer::set(const FlagType flag)
{
    if (flag == WithDevice) {
        SerializerEntry* entry = addEntry();
        entry->type = FlagEntryType;
        entry->subtype = static_cast<uint8_t>(WithDevice);
        entry->property = nullptr;
        entry->value = nullptr;
    } else if (flag == WithAvailability) {
        HAMqtt* mqtt = HAMqtt::instance();
        const bool isSharedAvailability = mqtt->getDevice()->isSharedAvailabilityEnabled();
        const bool isAvailabilityConfigured = _deviceType->isAvailabilityConfigured();

        if (!isSharedAvailability && !isAvailabilityConfigured) {
            return; // not configured
        }

        SerializerEntry* entry = addEntry();
        entry->type = TopicEntryType;
        entry->property = AHATOFSTR(HAAvailabilityTopic);
        entry->value = isSharedAvailability
            ? mqtt->getDevice()->getAvailabilityTopic()
            : nullptr;
    }
}

void HASerializer::topic(const __FlashStringHelper* topic)
{
    if (!_deviceType || !topic) {
        return;
    }

    SerializerEntry* entry = addEntry();
    entry->type = TopicEntryType;
    entry->property = topic;
}

HASerializer::SerializerEntry* HASerializer::addEntry()
{
    return &_entries[_entriesNb++]; // intentional lack of protection against overflow
}

uint16_t HASerializer::calculateSize() const
{
    uint16_t size =
        strlen_P(HASerializerJsonDataPrefix) +
        strlen_P(HASerializerJsonDataSuffix);

    for (uint8_t i = 0; i < _entriesNb; i++) {
        const uint16_t entrySize = calculateEntrySize(&_entries[i]);
        if (entrySize == 0) {
            continue;
        }

        size += entrySize;

        // items separator
        if (i > 0) {
            size += strlen_P(HASerializerJsonPropertiesSeparator);
        }
    }

    return size;
}

bool HASerializer::flush() const
{
    HAMqtt* mqtt = HAMqtt::instance();
    if (!mqtt || (_deviceType && !mqtt->getDevice())) {
        return false;
    }

    mqtt->writePayload(AHATOFSTR(HASerializerJsonDataPrefix));

    for (uint8_t i = 0; i < _entriesNb; i++) {
        if (i > 0) {
            mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertiesSeparator));
        }

        if (!flushEntry(&_entries[i])) {
            return false;
        }
    }

    mqtt->writePayload(AHATOFSTR(HASerializerJsonDataSuffix));
    return true;
}

uint16_t HASerializer::calculateEntrySize(const SerializerEntry* entry) const
{
    switch (entry->type) {
    case PropertyEntryType:
        return
            // property name
            strlen_P(HASerializerJsonPropertyPrefix) +
            strlen_P(AHAFROMFSTR(entry->property)) +
            strlen_P(HASerializerJsonPropertySuffix) +
            // property value
            calculatePropertyValueSize(entry);

    case TopicEntryType:
        return calculateTopicEntrySize(entry);

    case FlagEntryType:
        return calculateFlagSize(
            static_cast<FlagType>(entry->subtype)
        );

    default:
        return 0;
    }
}

uint16_t HASerializer::calculateTopicEntrySize(
    const SerializerEntry* entry
) const
{
    uint16_t size = 0;

    // property name
    size +=
        strlen_P(HASerializerJsonPropertyPrefix) +
        strlen_P(AHAFROMFSTR(entry->property)) +
        strlen_P(HASerializerJsonPropertySuffix);

    // topic escape
    size += 2 * strlen_P(HASerializerJsonEscapeChar);

    // topic
    if (entry->value) {
        size += strlen(static_cast<const char*>(entry->value));
    } else {
        if (!_deviceType) {
            return 0;
        }

        size += calculateDataTopicLength(
            _deviceType->uniqueId(),
            entry->property
        ) - 1; // exclude null terminator
    }

    return size;
}

uint16_t HASerializer::calculateFlagSize(const FlagType flag) const
{
    const HAMqtt* mqtt = HAMqtt::instance();
    const HADevice* device = mqtt->getDevice();

    if (flag == WithDevice && device->getSerializer()) {
        const uint16_t deviceLength = device->getSerializer()->calculateSize();
        if (deviceLength == 0) {
            return 0;
        }

        return
            strlen_P(HASerializerJsonPropertyPrefix) +
            strlen_P(HADeviceProperty) +
            strlen_P(HASerializerJsonPropertySuffix) +
            deviceLength;
    }

    return 0;
}

uint16_t HASerializer::calculatePropertyValueSize(
    const SerializerEntry* entry
) const
{
    switch (entry->subtype) {
    case ConstCharPropertyValue:
    case ProgmemPropertyValue: {
        const char* value = static_cast<const char*>(entry->value);
        const uint16_t len =
            entry->subtype == ConstCharPropertyValue ? strlen(value) : strlen_P(value);
        return 2 * strlen_P(HASerializerJsonEscapeChar) + len;
    }

    case BoolPropertyType: {
        const bool value = *static_cast<const bool*>(entry->value);
        return value ? strlen_P(HATrue) : strlen_P(HAFalse);
    }

    case NumberPropertyType: {
        const HANumeric* value = static_cast<const HANumeric*>(
            entry->value
        );
        return value->calculateSize();
    }

    case ArrayPropertyType: {
        const HASerializerArray* array = static_cast<const HASerializerArray*>(
            entry->value
        );
        return array->calculateSize();
    }

    default:
        return 0;
    }
}

bool HASerializer::flushEntry(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();

    switch (entry->type) {
    case PropertyEntryType: {
        mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertyPrefix));
        mqtt->writePayload(entry->property);
        mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertySuffix));

        return flushEntryValue(entry);
    }

    case TopicEntryType:
        return flushTopic(entry);

    case FlagEntryType:
        return flushFlag(entry);

    default:
        return true;
    }
}

bool HASerializer::flushEntryValue(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();

    switch (entry->subtype) {
    case ConstCharPropertyValue:
    case ProgmemPropertyValue: {
        const char* value = static_cast<const char*>(entry->value);
        mqtt->writePayload(AHATOFSTR(HASerializerJsonEscapeChar));

        if (entry->subtype == ConstCharPropertyValue) {
            mqtt->writePayload(value, strlen(value));
        } else {
            mqtt->writePayload(AHATOFSTR(value));
        }

        mqtt->writePayload(AHATOFSTR(HASerializerJsonEscapeChar));
        return true;
    }

    case BoolPropertyType: {
        const bool value = *static_cast<const bool*>(entry->value);
        mqtt->writePayload(AHATOFSTR(value ? HATrue : HAFalse));
        return true;
    }

    case NumberPropertyType: {
        const HANumeric* value = static_cast<const HANumeric*>(
            entry->value
        );

        char tmp[HANumeric::MaxDigitsNb + 1];
        const uint16_t length = value->toStr(tmp);

        mqtt->writePayload(tmp, length);
        return true;
    }

    case ArrayPropertyType: {
        const HASerializerArray* array = static_cast<const HASerializerArray*>(
            entry->value
        );
        const uint16_t size = array->calculateSize();
        char tmp[size + 1]; // including null terminator
        tmp[0] = 0;
        array->serialize(tmp);
        mqtt->writePayload(tmp, size);

        return true;
    }

    default:
        return false;
    }
}

bool HASerializer::flushTopic(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();

    // property name
    mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertyPrefix));
    mqtt->writePayload(entry->property);
    mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertySuffix));

    // value (escaped)
    mqtt->writePayload(AHATOFSTR(HASerializerJsonEscapeChar));

    if (entry->value) {
        const char* topic = static_cast<const char*>(entry->value);
        mqtt->writePayload(topic, strlen(topic));
    } else {
        const uint16_t length = calculateDataTopicLength(
            _deviceType->uniqueId(),
            entry->property
        );
        if (length == 0) {
            return false;
        }

        char topic[length];
        generateDataTopic(
            topic,
            _deviceType->uniqueId(),
            entry->property
        );

        mqtt->writePayload(topic, length - 1);
    }

    mqtt->writePayload(AHATOFSTR(HASerializerJsonEscapeChar));
    return true;
}

bool HASerializer::flushFlag(const SerializerEntry* entry) const
{
    HAMqtt* mqtt = HAMqtt::instance();
    const HADevice* device = mqtt->getDevice();
    const FlagType flag = static_cast<FlagType>(entry->subtype);

    if (flag == WithDevice && device) {
        mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertyPrefix));
        mqtt->writePayload(AHATOFSTR(HADeviceProperty));
        mqtt->writePayload(AHATOFSTR(HASerializerJsonPropertySuffix));

        return device->getSerializer()->flush();
    }

    return false;
}