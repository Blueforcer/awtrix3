#ifndef AHA_HAFAN_H
#define AHA_HAFAN_H

#include "HABaseDeviceType.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_FAN

#define HAFAN_STATE_CALLBACK(name) void (*name)(bool state, HAFan* sender)
#define HAFAN_SPEED_CALLBACK(name) void (*name)(uint16_t speed, HAFan* sender)

/**
 * HAFan allows adding a controllable fan in the Home Assistant panel.
 * The library supports only the state and speed of the fan.
 * If you want more features please open a new GitHub issue.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/fan.mqtt/
 */
class HAFan : public HABaseDeviceType
{
public:
    enum Features {
        DefaultFeatures = 0,
        SpeedsFeature = 1
    };

    /**
     * @param uniqueId The unique ID of the fan. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the fan.
     */
    HAFan(const char* uniqueId, const uint8_t features = DefaultFeatures);

    /**
     * Changes state of the fan and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the fan.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Changes the speed of the fan and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param speed The new speed of the fan. It should be in range of min and max value.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setSpeed(const uint16_t speed, const bool force = false);

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
     * Sets current state of the fan without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the fan.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns last known state of the fan.
     * By default it's `false`.
     */
    inline bool getCurrentState() const
        { return _currentState; }

    /**
     * Sets the current speed of the fan without pushing the value to Home Assistant.
     * This method may be useful if you want to change the speed before the connection
     * with the MQTT broker is acquired.
     *
     * @param speed The new speed of the fan. It should be in range of min and max value.
     */
    inline void setCurrentSpeed(const uint16_t speed)
        { _currentSpeed = speed; }

    /**
     * Returns the last known speed of the fan.
     * By default speed is set to `0`.
     */
    inline uint16_t getCurrentSpeed() const
        { return _currentSpeed; }

    /**
     * Sets icon of the fan.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the fan's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the fan state.
     * In this mode the fan state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Sets the maximum of numeric output range (representing 100%).
     * The number of speeds within the speed_range / 100 will determine the percentage step.
     * By default the maximum range is `100`.
     *
     * @param max The maximum of numeric output range.
     */
    inline void setSpeedRangeMax(const uint16_t max)
        { _speedRangeMax.setBaseValue(max); }

    /**
     * Sets the minimum of numeric output range (off is not included, so speed_range_min - 1 represents 0 %).
     * The number of speeds within the speed_range / 100 will determine the percentage step.
     * By default the minimum range is `1`.
     *
     * @param min The minimum of numeric output range.
     */
    inline void setSpeedRangeMin(const uint16_t min)
        { _speedRangeMin.setBaseValue(min); }

    /**
     * Registers callback that will be called each time the state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same fan.
     *
     * @param callback
     * @note In non-optimistic mode, the state must be reported back to HA using the HAFan::setState method.
     */
    inline void onStateCommand(HAFAN_STATE_CALLBACK(callback))
        { _stateCallback = callback; }

    /**
     * Registers callback that will be called each time the speed command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same fan.
     *
     * @param callback
     * @note In non-optimistic mode, the speed must be reported back to HA using the HAFan::setSpeed method.
     */
    inline void onSpeedCommand(HAFAN_SPEED_CALLBACK(callback))
        { _speedCallback = callback; }

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

    /**
     * Publishes the MQTT message with the given speed.
     *
     * @param speed The speed to publish. It should be in range of min and max value.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishSpeed(const uint16_t speed);

    /**
     * Parses the given state command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleStateCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given speed command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleSpeedCommand(const uint8_t* cmd, const uint16_t length);

    /// Features enabled for the fan.
    const uint8_t _features;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the fan (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// The maximum of numeric output range.
    HANumeric _speedRangeMax;

    /// The minimum of numeric output range.
    HANumeric _speedRangeMin;

    /// The current state of the fan. By default it's `false`.
    bool _currentState;

    /// The current speed of the fan. By default it's `0`.
    uint16_t _currentSpeed;

    /// The callback that will be called when the state command is received from the HA.
    HAFAN_STATE_CALLBACK(_stateCallback);

    /// The callback that will be called when the speed command is received from the HA.
    HAFAN_SPEED_CALLBACK(_speedCallback);
};

#endif
#endif
