#ifndef AHA_HATEXT_H
#define AHA_HATEXT_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_TEXT

#define HATEXT_CALLBACK(name) void (*name)(const char* message, uint16_t length, HAText* sender)

class HAText : public HABaseDeviceType
{
public:
    HAText(const char* uniqueId);

    inline void setIcon(const char* icon)
        { _icon = icon; }

    inline void setRetain(const bool retain)
        { _retain = retain; }

    inline void onMessage(HATEXT_CALLBACK(callback))
        { _messageCallback = callback; }

    bool setState(const char* value, bool force = false);

    /**
     * Enables or disables publishing of JSON attributes for this text entity.
     * When enabled, the discovery config advertises a `json_attributes_topic`
     * (`json_attr_t`) so Home Assistant reads extra attributes from that topic,
     * and publishJsonAttributes() publishes a retained JSON object onto it.
     * Disabled by default; while disabled the discovery payload is unchanged.
     *
     * @param enabled `true` to advertise the JSON attributes topic.
     */
    inline void setJsonAttributes(const bool enabled)
        { _jsonAttributes = enabled; }

    /**
     * Publishes the given JSON object as this text entity's attributes.
     * The message is retained and rides the same data-topic machinery as the
     * entity's state, so Home Assistant repopulates the attributes after an
     * HA or broker restart with no extra code.
     *
     * @param json A valid JSON object (e.g. `{"key":1}`).
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishJsonAttributes(const char* json);

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    const char* _icon;
    bool _retain;
    HATEXT_CALLBACK(_messageCallback);

    /// Whether the JSON attributes topic is advertised in the discovery config.
    bool _jsonAttributes;
};

#endif
#endif
