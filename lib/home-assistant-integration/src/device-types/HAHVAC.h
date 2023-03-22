#ifndef AHA_HAHVAC_H
#define AHA_HAHVAC_H

#include "HABaseDeviceType.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_HVAC

#define _SET_CURRENT_TEMPERATURE_OVERLOAD(type) \
    /** @overload */ \
    inline bool setCurrentTemperature(const type temperature, const bool force = false) \
        { return setCurrentTemperature(HANumeric(temperature, _precision), force); }

#define _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(type) \
    /** @overload */ \
    inline void setCurrentCurrentTemperature(const type temperature) \
        { setCurrentCurrentTemperature(HANumeric(temperature, _precision)); }

#define _SET_TARGET_TEMPERATURE_OVERLOAD(type) \
    /** @overload */ \
    inline bool setTargetTemperature(const type temperature, const bool force = false) \
        { return setTargetTemperature(HANumeric(temperature, _precision), force); }

#define _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(type) \
    /** @overload */ \
    inline void setCurrentTargetTemperature(const type temperature) \
        { setCurrentTargetTemperature(HANumeric(temperature, _precision)); }

#define HAHVAC_CALLBACK_BOOL(name) void (*name)(bool state, HAHVAC* sender)
#define HAHVAC_CALLBACK_TARGET_TEMP(name) void (*name)(HANumeric temperature, HAHVAC* sender)
#define HAHVAC_CALLBACK_FAN_MODE(name) void (*name)(FanMode mode, HAHVAC* sender)
#define HAHVAC_CALLBACK_SWING_MODE(name) void (*name)(SwingMode mode, HAHVAC* sender)
#define HAHVAC_CALLBACK_MODE(name) void (*name)(Mode mode, HAHVAC* sender)

class HASerializerArray;

/**
 * HAHVAC lets you control your HVAC devices.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/climate.mqtt/
 */
class HAHVAC : public HABaseDeviceType
{
public:
    static const uint8_t DefaultFanModes;
    static const uint8_t DefaultSwingModes;
    static const uint8_t DefaultModes;

    /// The list of features available in the HVAC. They're used in the constructor.
    enum Features {
        DefaultFeatures = 0,
        ActionFeature = 1,
        AuxHeatingFeature = 2,
        PowerFeature = 4,
        FanFeature = 8,
        SwingFeature = 16,
        ModesFeature = 32,
        TargetTemperatureFeature = 64
    };

    /// The list of available actions of the HVAC.
    enum Action {
        UnknownAction = 0,
        OffAction,
        HeatingAction,
        CoolingAction,
        DryingAction,
        IdleAction,
        FanAction
    };

    /// The list of available fan modes.
    enum FanMode {
        UnknownFanMode = 0,
        AutoFanMode = 1,
        LowFanMode = 2,
        MediumFanMode = 4,
        HighFanMode = 8
    };

    /// The list of available swing modes.
    enum SwingMode {
        UnknownSwingMode = 0,
        OnSwingMode = 1,
        OffSwingMode = 2
    };

    /// The list of available HVAC's modes.
    enum Mode {
        UnknownMode = 0,
        AutoMode = 1,
        OffMode = 2,
        CoolMode = 4,
        HeatMode = 8,
        DryMode = 16,
        FanOnlyMode = 32
    };

    /// Temperature units available in the HVAC.
    enum TemperatureUnit {
        DefaultUnit = 1,
        CelsiusUnit,
        FahrenheitUnit
    };

    /**
     * @param uniqueId The unique ID of the HVAC. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the HVAC.
     * @param precision The precision of temperatures reported by the HVAC.
     */
    HAHVAC(
        const char* uniqueId,
        const uint16_t features = DefaultFeatures,
        const NumberPrecision precision = PrecisionP1
    );

    /**
     * Frees memory allocated for the arrays serialization.
     */
    ~HAHVAC();

    /**
     * Changes current temperature of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param temperature New current temperature.
     * @param force Forces to update the temperature without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setCurrentTemperature(const HANumeric& temperature, const bool force = false);

    _SET_CURRENT_TEMPERATURE_OVERLOAD(int8_t)
    _SET_CURRENT_TEMPERATURE_OVERLOAD(int16_t)
    _SET_CURRENT_TEMPERATURE_OVERLOAD(int32_t)
    _SET_CURRENT_TEMPERATURE_OVERLOAD(uint8_t)
    _SET_CURRENT_TEMPERATURE_OVERLOAD(uint16_t)
    _SET_CURRENT_TEMPERATURE_OVERLOAD(uint32_t)
    _SET_CURRENT_TEMPERATURE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_CURRENT_TEMPERATURE_OVERLOAD(int)
#endif

    /**
     * Changes action of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param action New action.
     * @param force Forces to update the action without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setAction(const Action action, const bool force = false);

    /**
     * Changes state of the aux heating and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state The new state.
     * @param force Forces to update the state without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setAuxState(const bool state, const bool force = false);

    /**
     * Changes mode of the fan of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param mode New fan's mode.
     * @param force Forces to update the mode without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setFanMode(const FanMode mode, const bool force = false);

    /**
     * Changes swing mode of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param mode New swing mode.
     * @param force Forces to update the mode without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setSwingMode(const SwingMode mode, const bool force = false);

    /**
     * Changes mode of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param mode New HVAC's mode.
     * @param force Forces to update the mode without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setMode(const Mode mode, const bool force = false);

    /**
     * Changes target temperature of the HVAC and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param temperature Target temperature to set.
     * @param force Forces to update the mode without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setTargetTemperature(const HANumeric& temperature, const bool force = false);

    _SET_TARGET_TEMPERATURE_OVERLOAD(int8_t)
    _SET_TARGET_TEMPERATURE_OVERLOAD(int16_t)
    _SET_TARGET_TEMPERATURE_OVERLOAD(int32_t)
    _SET_TARGET_TEMPERATURE_OVERLOAD(uint8_t)
    _SET_TARGET_TEMPERATURE_OVERLOAD(uint16_t)
    _SET_TARGET_TEMPERATURE_OVERLOAD(uint32_t)
    _SET_TARGET_TEMPERATURE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_TARGET_TEMPERATURE_OVERLOAD(int)
#endif

    /**
     * Sets current temperature of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change temperature before connection
     * with MQTT broker is acquired.
     *
     * @param temperature New current temperature.
     */
    inline void setCurrentCurrentTemperature(const HANumeric& temperature)
        { if (temperature.getPrecision() == _precision) { _CURRENT_TEMPerature = temperature; } }

    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(int8_t)
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(int16_t)
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(int32_t)
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(uint8_t)
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(uint16_t)
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(uint32_t)
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_CURRENT_CURRENT_TEMPERATURE_OVERLOAD(int)
#endif

    /**
     * Returns last known current temperature of the HVAC.
     * If setCurrentTemperature method wasn't called the initial value will be returned.
     */
    inline const HANumeric& getCurrentTemperature() const
        { return _CURRENT_TEMPerature; }

    /**
     * Sets action of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change the action before connection
     * with MQTT broker is acquired.
     *
     * @param action New action.
     */
    inline void setCurrentAction(const Action action)
        { _action = action; }

    /**
     * Returns last known action of the HVAC.
     * If setAction method wasn't called the initial value will be returned.
     */
    inline Action getCurrentAction() const
        { return _action; }

    /**
     * Sets aux heating state without publishing it to Home Assistant.
     * This method may be useful if you want to change the state before connection
     * with MQTT broker is acquired.
     *
     * @param state The new state.
     */
    inline void setCurrentAuxState(const bool state)
        { _auxState = state; }

    /**
     * Returns last known state of the aux heating.
     * If setAuxState method wasn't called the initial value will be returned.
     */
    inline bool getCurrentAuxState() const
        { return _auxState; }

    /**
     * Sets fan's mode of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change the mode before connection
     * with MQTT broker is acquired.
     *
     * @param mode New fan's mode.
     */
    inline void setCurrentFanMode(const FanMode mode)
        { _fanMode = mode; }

    /**
     * Returns last known fan's mode of the HVAC.
     * If setFanMode method wasn't called the initial value will be returned.
     */
    inline FanMode getCurrentFanMode() const
        { return _fanMode; }

    /**
     * Sets available fan modes.
     *
     * @param modes The modes to set (for example: `HAHVAC::AutoFanMode | HAHVAC::HighFanMode`).
     */
    inline void setFanModes(const uint8_t modes)
        { _fanModes = modes; }

    /**
     * Sets swing mode of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change the mode before connection
     * with MQTT broker is acquired.
     *
     * @param mode New swing mode.
     */
    inline void setCurrentSwingMode(const SwingMode mode)
        { _swingMode = mode; }

    /**
     * Returns last known swing mode of the HVAC.
     * If setSwingMode method wasn't called the initial value will be returned.
     */
    inline SwingMode getCurrentSwingMode() const
        { return _swingMode; }

    /**
     * Sets available swing modes.
     *
     * @param modes The modes to set (for example: `HAHVAC::OnSwingMode`).
     */
    inline void setSwingModes(const uint8_t modes)
        { _swingModes = modes; }

    /**
     * Sets mode of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change the mode before connection
     * with MQTT broker is acquired.
     *
     * @param mode New HVAC's mode.
     */
    inline void setCurrentMode(const Mode mode)
        { _mode = mode; }

    /**
     * Returns last known mode of the HVAC.
     * If setMode method wasn't called the initial value will be returned.
     */
    inline Mode getCurrentMode() const
        { return _mode; }

    /**
     * Sets available HVAC's modes.
     *
     * @param modes The modes to set (for example: `HAHVAC::CoolMode | HAHVAC::HeatMode`).
     */
    inline void setModes(const uint8_t modes)
        { _modes = modes; }

    /**
     * Sets target temperature of the HVAC without publishing it to Home Assistant.
     * This method may be useful if you want to change the target before connection
     * with MQTT broker is acquired.
     *
     * @param temperature Target temperature to set.
     */
    inline void setCurrentTargetTemperature(const HANumeric& temperature)
        { if (temperature.getPrecision() == _precision) { _targetTemperature = temperature; } }

    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(int8_t)
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(int16_t)
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(int32_t)
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(uint8_t)
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(uint16_t)
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(uint32_t)
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_CURRENT_TARGET_TEMPERATURE_OVERLOAD(int)
#endif

    /**
     * Returns last known target temperature of the HVAC.
     * If setTargetTemperature method wasn't called the initial value will be returned.
     */
    inline const HANumeric& getCurrentTargetTemperature() const
        { return _targetTemperature; }

    /**
     * Sets icon of the HVAC.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the HVAC's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Changes the temperature unit.
     *
     * @param unit See the TemperatureUnit enum above.
     */
    inline void setTemperatureUnit(TemperatureUnit unit)
        { _temperatureUnit = unit; }

    /**
     * Sets the minimum temperature that can be set from the Home Assistant panel.
     *
     * @param min The minimum value.
     */
    inline void setMinTemp(const float min)
        { _minTemp = HANumeric(min, _precision); }

    /**
     * Sets the maximum temperature that can be set from the Home Assistant panel.
     *
     * @param min The maximum value.
     */
    inline void setMaxTemp(const float max)
        { _maxTemp = HANumeric(max, _precision); }

    /**
     * Sets the step of the temperature that can be set from the Home Assistant panel.
     *
     * @param step The setp value. By default it's `1`.
     */
    inline void setTempStep(const float step)
        { _tempStep = HANumeric(step, _precision); }

    /**
     * Registers callback that will be called each time the aux state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     * @note The aux state must be reported back to HA using the HAHVAC::setAuxState method.
     */
    inline void onAuxStateCommand(HAHVAC_CALLBACK_BOOL(callback))
        { _auxCallback = callback; }

    /**
     * Registers callback that will be called each time the power command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     */
    inline void onPowerCommand(HAHVAC_CALLBACK_BOOL(callback))
        { _powerCallback = callback; }

    /**
     * Registers callback that will be called each time the fan mode command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     * @note The fan mode must be reported back to HA using the HAHVAC::setFanMode method.
     */
    inline void onFanModeCommand(HAHVAC_CALLBACK_FAN_MODE(callback))
        { _fanModeCallback = callback; }

    /**
     * Registers callback that will be called each time the swing mode command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     * @note The swing mode must be reported back to HA using the HAHVAC::setSwingMode method.
     */
    inline void onSwingModeCommand(HAHVAC_CALLBACK_SWING_MODE(callback))
        { _swingModeCallback = callback; }

    /**
     * Registers callback that will be called each time the HVAC mode command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     * @note The mode must be reported back to HA using the HAHVAC::setMode method.
     */
    inline void onModeCommand(HAHVAC_CALLBACK_MODE(callback))
        { _modeCallback = callback; }

    /**
     * Registers callback that will be called each time the target temperature is set via HA panel.
     * Please note that it's not possible to register multiple callbacks for the same HVAC.
     *
     * @param callback
     * @note The target temperature must be reported back to HA using the HAHVAC::setTargetTemperature method.
     */
    inline void onTargetTemperatureCommand(HAHVAC_CALLBACK_TARGET_TEMP(callback))
        { _targetTemperatureCallback = callback; }

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
     * Publishes the MQTT message with the given current temperature.
     *
     * @param temperature The temperature to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishCurrentTemperature(const HANumeric& temperature);

    /**
     * Publishes the MQTT message with the given action.
     *
     * @param action The action to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishAction(const Action action);

    /**
     * Publishes the MQTT message with the given aux heating state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishAuxState(const bool state);

    /**
     * Publishes the MQTT message with the given fan mode.
     *
     * @param mode The mode to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishFanMode(const FanMode mode);

    /**
     * Publishes the MQTT message with the given swing mode.
     *
     * @param mode The mode to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishSwingMode(const SwingMode mode);

    /**
     * Publishes the MQTT message with the given mode.
     *
     * @param mode The mode to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishMode(const Mode mode);

    /**
     * Publishes the MQTT message with the given target temperature.
     *
     * @param temperature The temperature to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishTargetTemperature(const HANumeric& temperature);

    /**
     * Parses the given aux state command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleAuxStateCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given power command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handlePowerCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given fan mode command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleFanModeCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given swing mode command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleSwingModeCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given HVAC's mode command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleModeCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given HVAC's target temperature command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleTargetTemperatureCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Returns progmem string representing value template for the command
     * that contains floating point numbers.
     */
    const __FlashStringHelper* getCommandWithFloatTemplate();

    /// Features enabled for the HVAC.
    const uint16_t _features;

    /// The precision of temperatures. By default it's `HANumber::PrecisionP1`.
    const NumberPrecision _precision;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The current temperature of the HVAC. By default it's not set.
    HANumeric _CURRENT_TEMPerature;

    /// The current action of the HVAC. By default it's `HAHVAC::UnknownAction`.
    Action _action;

    /// The temperature unit for the HVAC. By default it's `HAHVAC::DefaultUnit`.
    TemperatureUnit _temperatureUnit;

    /// The minimum temperature that can be set.
    HANumeric _minTemp;

    /// The maximum temperature that can be set.
    HANumeric _maxTemp;

    /// The step of the temperature that can be set.
    HANumeric _tempStep;

    /// Callback that will be called when the aux state command is received from the HA.
    HAHVAC_CALLBACK_BOOL(_auxCallback);

    /// The state of the aux heating. By default it's `false`.
    bool _auxState;

    /// Callback that will be called when the power command is received from the HA.
    HAHVAC_CALLBACK_BOOL(_powerCallback);

    /// The current mode of the fan. By default it's `HAHVAC::UnknownFanMode`.
    FanMode _fanMode;

    /// The supported fan modes. By default it's `HAHVAC::DefaultFanModes`.
    uint8_t _fanModes;

    /// The serializer for the fan modes. It's `nullptr` if the fan feature is disabled.
    HASerializerArray* _fanModesSerializer;

    /// Callback that will be called when the fan mode command is received from the HA.
    HAHVAC_CALLBACK_FAN_MODE(_fanModeCallback);

    /// The current swing mode. By default it's `HAHVAC::UnknownSwingMode`.
    SwingMode _swingMode;

    /// The supported swing modes. By default it's `HAHVAC::DefaultSwingModes`.
    uint8_t _swingModes;

    /// The serializer for the swing modes. It's `nullptr` if the swing feature is disabled.
    HASerializerArray* _swingModesSerializer;

    /// Callback that will be called when the swing mode command is received from the HA.
    HAHVAC_CALLBACK_SWING_MODE(_swingModeCallback);

    /// The current mode. By default it's `HAHVAC::UnknownMode`.
    Mode _mode;

    /// The supported modes. By default it's `HAHVAC::DefaultModes`.
    uint8_t _modes;

    /// The serializer for the modes. It's `nullptr` if the modes feature is disabled.
    HASerializerArray* _modesSerializer;

    /// Callback that will be called when the mode command is received from the HA.
    HAHVAC_CALLBACK_MODE(_modeCallback);

    /// The target temperature of the HVAC. By default it's not set.
    HANumeric _targetTemperature;

    /// Callback that will be called when the target temperature is changed via the HA panel.
    HAHVAC_CALLBACK_TARGET_TEMP(_targetTemperatureCallback);
};

#endif
#endif
