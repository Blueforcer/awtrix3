#ifndef AHA_HABASEDEVICETYPE_H
#define AHA_HABASEDEVICETYPE_H

#include <Arduino.h>
#include "../ArduinoHADefines.h"

class HAMqtt;
class HASerializer;

class HABaseDeviceType
{
public:
    enum NumberPrecision {
        /// No digits after the decimal point.
        PrecisionP0 = 0,

        /// One digit after the decimal point.
        PrecisionP1,

        /// Two digits after the decimal point.
        PrecisionP2,

        /// Three digits after the decimal point.
        PrecisionP3
    };

    /**
     * Creates a new device type instance and registers it in the HAMqtt class.
     *
     * @param componentName The name of the Home Assistant component (e.g. `binary_sensor`).
     *                      You can find all available component names in the Home Assistant documentation.
     *                      The component name needs to be stored in the flash memory.
     * @param uniqueId The unique ID of the device type. It needs to be unique in a scope of the HADevice.
     */
    HABaseDeviceType(
        const __FlashStringHelper* componentName,
        const char* uniqueId
    );

    /**
     * Returns unique ID of the device type.
     */
    inline const char* uniqueId() const
        { return _uniqueId; }

    /**
     * Returns component name defined by the device type.
     * It's used for the MQTT discovery topic.
     */
    inline const __FlashStringHelper* componentName() const
        { return _componentName; }

    /**
     * Returns `true` if the availability was configured for this device type.
     */
    inline bool isAvailabilityConfigured() const
        { return (_availability != AvailabilityDefault); }

    /**
     * Returns online state of the device type.
     */
    inline bool isOnline() const
        { return (_availability == AvailabilityOnline); }

    /**
     * Sets name of the device type that will be used as a label in the HA panel.
     * Keep the name short to save the resources.
     *
     * @param name The device type name.
     */
    inline void setName(const char* name)
        { _name = name; }

    /**
     * Returns name of the deviced type that was assigned via setName method.
     * It can be nullptr if there is no name assigned.
     */
    inline const char* getName() const
        { return _name; }

    /**
     * Sets availability of the device type.
     * Setting the initial availability enables availability reporting for this device type.
     * Please note that not all device types support this feature.
     * Follow HA documentation of a specific device type to get more information.
     *
     * @param online Specifies whether the device type is online.
     */
    virtual void setAvailability(bool online);

#ifdef ARDUINOHA_TEST
    inline HASerializer* getSerializer() const
        { return _serializer; }

    inline void buildSerializerTest()
        { buildSerializer(); }
#endif

protected:
    /**
     * Returns instance of the HAMqtt class.
     */
    static HAMqtt* mqtt();

    /**
     * Subscribes to the given data topic.
     *
     * @param uniqueId THe unique ID of the device type assigned via the constructor.
     * @param topic Topic to subscribe (progmem string).
     */
    static void subscribeTopic(
        const char* uniqueId,
        const __FlashStringHelper* topic
    );

    /**
     * This method should build serializer that will be used for publishing the configuration.
     * The serializer is built each time the MQTT connection is acquired.
     * Follow implementation of the existing device types to get better understanding of the logic.
     */
    virtual void buildSerializer() { };

    /**
     * This method is called each time the MQTT connection is acquired.
     * Each device type should publish its configuration and availability.
     * It can be also used for subscribing to MQTT topics.
     */
    virtual void onMqttConnected() = 0;

    /**
     * This method is called each time the device receives a MQTT message.
     * It can be any MQTT message so the method should always verify the topic.
     *
     * @param topic The topic on which the message was produced.
     * @param payload The payload of the message. It can be nullptr.
     * @param length The length of the payload.
     */
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    );

    /**
     * Destroys the existing serializer.
     */
    void destroySerializer();

    /**
     * Publishes configuration of this device type on the HA discovery topic.
     */
    void publishConfig();

    /**
     * Publishes current availability of the device type.
     * The message is only produced if the availability is configured for this device type.
     */
    void publishAvailability();

    /**
     * Publishes the given flash string on the data topic.
     *
     * @param topic The topic to publish on (progmem string).
     * @param payload The message's payload (progmem string).
     * @param retained Specifies whether the message should be retained.
     */
    bool publishOnDataTopic(
        const __FlashStringHelper* topic,
        const __FlashStringHelper* payload,
        bool retained = false
    );

    /**
     * Publishes the given string on the data topic.
     *
     * @param topic The topic to publish on (progmem string).
     * @param payload The message's payload.
     * @param retained Specifies whether the message should be retained.
     */
    bool publishOnDataTopic(
        const __FlashStringHelper* topic,
        const char* payload,
        bool retained = false
    );

    /**
     * Publishes the given data on the data topic.
     *
     * @param topic The topic to publish on (progmem string).
     * @param payload The message's payload.
     * @param length The length of the payload.
     * @param retained Specifies whether the message should be retained.
     * @param isProgmemData Specifies whether the given data is stored in the flash memory.
     */
    bool publishOnDataTopic(
        const __FlashStringHelper* topic,
        const uint8_t* payload,
        const uint16_t length,
        bool retained = false,
        bool isProgmemData = false
    );

    /// The component name that was assigned via the constructor.
    const __FlashStringHelper* const _componentName;

    /// The unique ID that was assigned via the constructor.
    const char* _uniqueId;

    /// The name that was set using setName method. It can be nullptr.
    const char* _name;

    /// HASerializer that belongs to this device type. It can be nullptr.
    HASerializer* _serializer;

private:
    enum Availability {
        AvailabilityDefault = 0,
        AvailabilityOnline,
        AvailabilityOffline
    };

    /// The current availability of this device type. AvailabilityDefault means that the initial availability was never set.
    Availability _availability;
    friend class HAMqtt;
};

#endif
