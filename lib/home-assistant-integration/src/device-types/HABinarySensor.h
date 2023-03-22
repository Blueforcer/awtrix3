#ifndef AHA_HABINARYSENSOR_H
#define AHA_HABINARYSENSOR_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_BINARY_SENSOR

/**
 * HABinarySensor represents a binary sensor that allows publishing on/off state to the Home Assistant panel.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/binary_sensor.mqtt/
 */
class HABinarySensor : public HABaseDeviceType
{
public:
    /**
     * @param uniqueId The unique ID of the button. It needs to be unique in a scope of your device.
     */
    HABinarySensor(const char* uniqueId);

    /**
     * Changes state of the sensor and publish MQTT message.
     * Please note that if a new value is the same as the previous one the MQTT message won't be published.
     *
     * @param state New state of the sensor (`true` - on, `false` - off).
     * @param force Forces to update the state without comparing it to a previous known state.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Sets the current state of the sensor without publishing it to Home Assistant.
     * This method may be useful if you want to change the state before the connection with the MQTT broker is acquired.
     *
     * @param state New state of the sensor.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns the last known state of the sensor.
     */
    inline bool getCurrentState() const
        { return _currentState; }

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/binary_sensor/#device-class
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

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    /**
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(bool state);

    /// The device class. It can be nullptr.
    const char* _class;

    /// The icon of the sensor. It can be nullptr.
    const char* _icon;

    /// Current state of the sensor. By default it's false.
    bool _currentState;
};

#endif
#endif
