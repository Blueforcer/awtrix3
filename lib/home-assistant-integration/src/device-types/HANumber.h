#ifndef AHA_HANUMBER_H
#define AHA_HANUMBER_H

#include "HABaseDeviceType.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_NUMBER

#define _SET_STATE_OVERLOAD(type) \
    /** @overload */ \
    inline bool setState(const type state, const bool force = false) \
        { return setState(HANumeric(state, _precision), force); }

#define _SET_CURRENT_STATE_OVERLOAD(type) \
    /** @overload */ \
    inline void setCurrentState(const type state) \
        { setCurrentState(HANumeric(state, _precision)); }

#define HANUMBER_CALLBACK(name) void (*name)(HANumeric number, HANumber* sender)

/**
 * HANumber adds a slider or a box in the Home Assistant panel
 * that controls the numeric value stored on your device.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/number.mqtt/
 */
class HANumber : public HABaseDeviceType
{
public:
    /// Represents mode of the number.
    enum Mode {
        ModeAuto = 0,
        ModeBox,
        ModeSlider
    };

    /**
     * Creates instance of the HANumber entity with the given numbers precision.
     * The given precision applies to the state, min, max and step values.
     *
     * @param uniqueId The unique ID of the number. It needs to be unique in a scope of your device.
     * @param precision Precision of the floating point number that will be displayed in the HA panel.
     */
    HANumber(const char* uniqueId, const NumberPrecision precision = PrecisionP0);

    /**
     * Changes state of the number and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the number.
     * @param force Forces to update state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const HANumeric& state, const bool force = false);

    _SET_STATE_OVERLOAD(int8_t)
    _SET_STATE_OVERLOAD(int16_t)
    _SET_STATE_OVERLOAD(int32_t)
    _SET_STATE_OVERLOAD(uint8_t)
    _SET_STATE_OVERLOAD(uint16_t)
    _SET_STATE_OVERLOAD(uint32_t)
    _SET_STATE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_STATE_OVERLOAD(int)
#endif

    /**
     * Sets current state of the number without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the number.
     */
    inline void setCurrentState(const HANumeric& state)
        { if (state.getPrecision() == _precision) { _currentState = state; } }

    _SET_CURRENT_STATE_OVERLOAD(int8_t)
    _SET_CURRENT_STATE_OVERLOAD(int16_t)
    _SET_CURRENT_STATE_OVERLOAD(int32_t)
    _SET_CURRENT_STATE_OVERLOAD(uint8_t)
    _SET_CURRENT_STATE_OVERLOAD(uint16_t)
    _SET_CURRENT_STATE_OVERLOAD(uint32_t)
    _SET_CURRENT_STATE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_CURRENT_STATE_OVERLOAD(int)
#endif

    /**
     * Returns last known state of the number.
     * If setState method wasn't called the initial value will be returned.
     */
    inline const HANumeric& getCurrentState() const
        { return _currentState; }

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/number/#device-class
     *
     * @param deviceClass The class name.
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the number.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the number's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the number state.
     * In this mode the number state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Sets mode of the number.
     * It controls how the number should be displayed in the UI.
     * By default it's `HANumber::ModeAuto`.
     *
     * @param mode Mode to set.
     */
    inline void setMode(const Mode mode)
        { _mode = mode; }

    /**
     * Defines the units of measurement of the number, if any.
     *
     * @param units For example: °C, %
     */
    inline void setUnitOfMeasurement(const char* unitOfMeasurement)
        { _unitOfMeasurement = unitOfMeasurement; }

    /**
     * Sets the minimum value that can be set from the Home Assistant panel.
     *
     * @param min The minimal value. By default the value is not set.
     */
    inline void setMin(const float min)
        { _minValue = HANumeric(min, _precision); }

    /**
     * Sets the maximum value that can be set from the Home Assistant panel.
     *
     * @param min The maximum value. By default the value is not set.
     */
    inline void setMax(const float max)
        { _maxValue = HANumeric(max, _precision); }

    /**
     * Sets step of the slider's movement in the Home Assistant panel.
     *
     * @param step The step value. Smallest value `0.001`. By default the value is not set.
     */
    inline void setStep(const float step)
        { _step = HANumeric(step, _precision); }

    /**
     * Registers callback that will be called each time the number is changed in the HA panel.
     * Please note that it's not possible to register multiple callbacks for the same number.
     *
     * @param callback
     * @note In non-optimistic mode, the number must be reported back to HA using the HANumber::setState method.
     */
    inline void onCommand(HANUMBER_CALLBACK(callback))
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
    bool publishState(const HANumeric& state);

    /**
     * Parses the given command and executes the number's callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Returns progmem string representing mode of the number
     */
    const __FlashStringHelper* getModeProperty() const;

    /**
     * Returns progmem string representing value template for the command.
     */
    const __FlashStringHelper* getCommandTemplate();

    /// The precision of the number. By default it's `HANumber::PrecisionP0`.
    const NumberPrecision _precision;

    /// The device class. It can be nullptr.
    const char* _class;

    /// The icon of the number. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the number (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// Controls how the number should be displayed in the UI. By default it's `HANumber::ModeAuto`.
    Mode _mode;

    /// The unit of measurement for the sensor. It can be nullptr.
    const char* _unitOfMeasurement;

    /// The minimal value that can be set from the HA panel.
    HANumeric _minValue;

    /// The maximum value that can be set from the HA panel.
    HANumeric _maxValue;

    /// The step of the slider's movement.
    HANumeric _step;

    /// The current state of the number. By default the value is not set.
    HANumeric _currentState;

    /// The callback that will be called when the command is received from the HA.
    HANUMBER_CALLBACK(_commandCallback);
};

#endif
#endif
