#ifndef AHA_HASELECT_H
#define AHA_HASELECT_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_SELECT

class HASerializerArray;

#define HASELECT_CALLBACK(name) void (*name)(int8_t index, HASelect* sender)

/**
 * HASelect adds a dropdown with options in the Home Assistant panel.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/button.mqtt/
 */
class HASelect : public HABaseDeviceType
{
public:
    /**
     * @param uniqueId The unique ID of the select. It needs to be unique in a scope of your device.
     */
    HASelect(const char* uniqueId);
    ~HASelect();

    /**
     * Sets the list of available options that will be listed in the dropdown.
     * The input string should contain options separated using semicolons.
     * For example: `setOptions("Option A;Option B;Option C");
     *
     * @param options The list of options that are separated by semicolons.
     * @note The options list can be set only once. Call resetOptions() first to
     *       replace an already-set list (e.g. a dynamic, discovery-driven select).
     */
    void setOptions(const char* options);

    /**
     * Releases the current option list so setOptions() can be called again, and
     * resets the current state to "no option selected" (-1). Useful for a select
     * whose options change at runtime: call resetOptions(), setOptions(newList),
     * then re-publish the discovery config so Home Assistant sees the new options.
     */
    void resetOptions();

    /**
     * Changes state of the select and publishes MQTT message.
     * State represents the index of the option that was set using the setOptions method.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the select.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns true if MQTT message has been published successfully.
     */
    bool setState(const int8_t state, const bool force = false);

    /**
     * Sets the current state of the select without publishing it to Home Assistant.
     * State represents the index of the option that was set using the setOptions method.
     * This method may be useful if you want to change the state before the connection
     * with the MQTT broker is acquired.
     *
     * @param state The new state of the cover.
     */
    inline void setCurrentState(const int8_t state)
        { _currentState = state; }

    /**
     * Returns last known state of the select.
     * State represents the index of the option that was set using the setOptions method.
     * By default the state is set to `-1`.
     */
    inline int8_t getCurrentState() const
        { return _currentState; }

    /**
     * Sets icon of the select.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the select's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the select state.
     * In this mode the select state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Registers callback that will be called each time the option is changed from the HA panel.
     * Please note that it's not possible to register multiple callbacks for the same select.
     *
     * @param callback
     * @note In non-optimistic mode, the selected option must be reported back to HA using the HASelect::setState method.
     */
    inline void onCommand(HASELECT_CALLBACK(callback))
        { _commandCallback = callback; }

    /**
     * Enables or disables publishing of JSON attributes for this select.
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
     * Publishes the given JSON object as this select's attributes.
     * The message is retained and rides the same data-topic machinery as the
     * select's state, so Home Assistant repopulates the attributes after an
     * HA or broker restart with no extra code.
     *
     * @param json A valid JSON object (e.g. `{"key":1}`).
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishJsonAttributes(const char* json);

#ifdef ARDUINOHA_TEST
    inline HASerializerArray* getOptions() const
        { return _options; }
#endif

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    /**
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(const int8_t state);

    /**
     * Counts the amount of options in the given string.
     */
    uint8_t countOptionsInString(const char* options) const;

    /// Array of options for the serializer.
    HASerializerArray* _options;

    /// Stores the current state (the current option's index). By default it's `-1`.
    int8_t _currentState;

    /// The icon of the select. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the select (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// Whether the JSON attributes topic is advertised in the discovery config.
    bool _jsonAttributes;

    /// The command callback that will be called when option is changed via the HA panel.
    HASELECT_CALLBACK(_commandCallback);
};

#endif
#endif
