#ifndef AHA_HASWITCH_H
#define AHA_HASWITCH_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_SWITCH

#define HASWITCH_CALLBACK(name) void (*name)(bool state, HASwitch* sender)

/**
 * HASwitch allows to display on/off switch in the HA panel and receive commands on your device.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/switch.mqtt/
 */
class HASwitch : public HABaseDeviceType
{
public:
    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     */
    HASwitch(const char* uniqueId);

    /**
     * Changes state of the switch and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the switch.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Alias for `setState(true)`.
     */
    inline bool turnOn()
        { return setState(true); }

    /**
     * Alias for `setState(false)`.
     */
    inline bool turnOff()
        { return setState(false); }

    /**
     * Sets current state of the switch without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the switch.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns last known state of the switch.
     * By default it's `false`.
     */
    inline bool getCurrentState() const
        { return _currentState; }

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/switch/#device-class
     *
     * @param deviceClass The class name.
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the switch command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the switch state.
     * In this mode the switch state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Registers callback that will be called each time the on/off command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same switch.
     *
     * @param callback
     * @note In non-optimistic mode, the state must be reported back to HA using the HASwitch::setState method.
     */
    inline void onCommand(HASWITCH_CALLBACK(callback))
        { _commandCallback = callback; }

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
    bool publishState(const bool state);

    /// The device class. It can be nullptr.
    const char* _class;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the switch (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// The current state of the switch. By default it's `false`.
    bool _currentState;

    /// The callback that will be called when switch command is received from the HA.
    HASWITCH_CALLBACK(_commandCallback);
};

#endif
#endif
