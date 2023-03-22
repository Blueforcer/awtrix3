
#ifndef AHA_SERIALIZER_H
#define AHA_SERIALIZER_H

#include <stdint.h>

#include "HADictionary.h"
#include "HASerializerArray.h"

class HAMqtt;
class HABaseDeviceType;

/**
 * This class allows to create JSON objects easily.
 * Its main purpose is to handle configuration of a device type that's going to
 * be published to the MQTT broker.
 */
class HASerializer
{
public:
    /// Type of the object's entry.
    enum EntryType {
        UnknownEntryType = 0,
        PropertyEntryType,
        TopicEntryType,
        FlagEntryType
    };

    /// The type of a flag for a FlagEntryType.
    enum FlagType {
        WithDevice = 1,
        WithAvailability
    };

    /// Available data types of entries.
    enum PropertyValueType {
        UnknownPropertyValueType = 0,
        ConstCharPropertyValue,
        ProgmemPropertyValue,
        BoolPropertyType,
        NumberPropertyType,
        ArrayPropertyType
    };

    /// Representation of a single entry in the object.
    struct SerializerEntry {
        /// Type of the entry.
        EntryType type;

        /// Subtype of the entry. It can be `FlagType`, `PropertyValueType` or `TopicType`.
        uint8_t subtype;

        /// Pointer to the property name (progmem string).
        const __FlashStringHelper* property;

        /// Pointer to the property value. The value type is determined by `subtype`.
        const void* value;

        SerializerEntry():
            type(UnknownEntryType),
            subtype(0),
            property(nullptr),
            value(nullptr)
        { }
    };

    /**
     * Calculates the size of a configuration topic for the given component and object ID.
     * The configuration topic has structure as follows: `[discovery prefix]/[component]/[device ID]_[objectId]/config`
     *
     * @param component The name of the HA component (e.g. `binary_sensor`).
     * @param objectId The unique ID of a device type that's going to publish the config.
     */
    static uint16_t calculateConfigTopicLength(
        const __FlashStringHelper* component,
        const char* objectId
    );

    /**
     * Generates the configuration topic for the given component and object ID.
     * The topic will be stored in the `output` variable.
     *
     * @param output Buffer where the topic will be written.
     * @param component The name of the HA component (e.g. `binary_sensor`).
     * @param objectId The unique ID of a device type that's going to publish the config.
     */
    static bool generateConfigTopic(
        char* output,
        const __FlashStringHelper* component,
        const char* objectId
    );

    /**
     * Calculates the size of the given data topic for the given objectId.
     * The data topic has structure as follows: `[data prefix]/[device ID]_[objectId]/[topic]`
     *
     * @param objectId The unique ID of a device type that's going to publish the data.
     * @param topic The topic name (progmem string).
     */
    static uint16_t calculateDataTopicLength(
        const char* objectId,
        const __FlashStringHelper* topic
    );

    /**
     * Generates the data topic for the given object ID.
     * The topic will be stored in the `output` variable.
     *
     * @param output Buffer where the topic will be written.
     * @param objectId The unique ID of a device type that's going to publish the data.
     * @param topic The topic name (progmem string).
     */
    static bool generateDataTopic(
        char* output,
        const char* objectId,
        const __FlashStringHelper* topic
    );

    /**
     * Checks whether the given topic matches the data topic that can be generated
     * using the given objectId and topicP.
     * This method can be used to check if the received message matches some data topic.
     *
     * @param actualTopic The actual topic to compare.
     * @param objectId The unique ID of a device type that may be the owner of the topic.
     * @param topic The topic name (progmem string).
     */
    static bool compareDataTopics(
        const char* actualTopic,
        const char* objectId,
        const __FlashStringHelper* topic
    );

    /**
     * Creates instance of the serializer for the given device type.
     * Please note that the number JSON object's entries needs to be known upfront.
     * This approach reduces number of memory allocations.
     *
     * @param deviceType The device type that owns the serializer.
     * @param maxEntriesNb Maximum number of the output object entries.
     */
    HASerializer(HABaseDeviceType* deviceType, const uint8_t maxEntriesNb);

    /**
     * Frees the dynamic memory allocated by the class.
     */
    ~HASerializer();

    /**
     * Returns the number of items that were added to the serializer.
     */
    inline uint8_t getEntriesNb() const
        { return _entriesNb; }

    /**
     * Returns pointer to the serializer's entries.
     */
    inline SerializerEntry* getEntries() const
        { return _entries; }

    /**
     * Adds a new entry to the serialized with a type of `PropertyEntryType`.
     *
     * @param property Pointer to the name of the property (progmem string).
     * @param value Pointer to the value that's being set.
     * @param valueType The type of the value that's passed to the method.
     */
    void set(
        const __FlashStringHelper* property,
        const void* value,
        PropertyValueType valueType = ConstCharPropertyValue
    );

    /**
     * Adds a new entry to the serializer with a type of `FlagEntryType`.
     *
     * @param flag Flag to add.
     */
    void set(const FlagType flag);

    /**
     * Adds a new entry to the serialize with a type of `TopicEntryType`.
     *
     * @param topic The topic name to add (progmem string).
     */
    void topic(const __FlashStringHelper* topic);

    /**
     * Calculates the output size of the serialized JSON object.
     */
    uint16_t calculateSize() const;

    /**
     * Flushes the JSON object to the MQTT stream.
     * Please note that this method only writes the MQTT payload.
     * The MQTT session needs to be opened before.
     */
    bool flush() const;

private:
    /// Pointer to the device type that owns the serializer.
    HABaseDeviceType* _deviceType;

    /// The number of entries added to the serializer.
    uint8_t _entriesNb;

    /// Maximum number of entries that can be added to the serializer.
    uint8_t _maxEntriesNb;

    /// Pointer to the serializer entries.
    SerializerEntry* _entries;

    /**
     * Creates a new entry in the serializer's memory.
     * If the limit of entries is hit, the nullptr is returned.
     */
    SerializerEntry* addEntry();

    /**
     * Calculates the serialized size of the given entry.
     * Internally, this method recognizes the type of the entry and calls
     * a proper calculate method listed below.
     */
    uint16_t calculateEntrySize(const SerializerEntry* entry) const;

    /**
     *  Calculates the size of the entry of type `TopicEntryType`.
     */
    uint16_t calculateTopicEntrySize(const SerializerEntry* entry) const;

    /**
     * Calculates the size of the entry of type `FlagEntryType`.
     */
    uint16_t calculateFlagSize(const FlagType flag) const;

    /**
     * Calculates the size of the entry's value if the entry is `PropertyEntryType`.
     */
    uint16_t calculatePropertyValueSize(const SerializerEntry* entry) const;

    /**
     * Calculates the size of the array if the property's value is a type of `ArrayPropertyType`.
     */
    uint16_t calculateArraySize(const HASerializerArray* array) const;

    /**
     * Flushes the given entry to the MQTT.
     * Internally this method recognizes the type of the entry and calls
     * a proper flush method listed below.
     */
    bool flushEntry(const SerializerEntry* entry) const;

    /**
     * Flushes the value of the `PropertyEntryType` entry.
     */
    bool flushEntryValue(const SerializerEntry* entry) const;

    /**
     * Flushes the entry of type `TopicEntryType` to the MQTT.
     */
    bool flushTopic(const SerializerEntry* entry) const;

    /**
     * Flushes the entry of type `FlagEntryType` to the MQTT.
     */
    bool flushFlag(const SerializerEntry* entry) const;
};

#endif
