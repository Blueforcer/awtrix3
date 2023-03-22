#ifndef AHA_HASENSOR_H
#define AHA_HASENSOR_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_SENSOR

/**
 * HASensor allows to publish textual sensor values that will be displayed in the HA panel.
 * If you need to publish numbers then HASensorNumber is what you're looking for.
 *
 * @note It's not possible to define a sensor that publishes mixed values (e.g. string + integer values).
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/sensor.mqtt/
 */
class HASensor : public HABaseDeviceType
{
public:
    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     */
    HASensor(const char* uniqueId);

    /**
     * Publishes the MQTT message with the given value.
     * Unlike the other device types, the HASensor doesn't store the previous value that was set.
     * It means that the MQTT message is produced each time the setValue method is called.
     *
     * @param value String representation of the sensor's value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setValue(const char* value);

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/sensor/#device-class
     *
     * @param deviceClass The class name.
     */
    inline void setDeviceClass(const char* deviceClass)
        { _deviceClass = deviceClass; }

    /**
     * Forces HA panel to process each incoming value (MQTT message).
     * It's useful if you want to have meaningful value graphs in history.
     *
     * @param forceUpdate
     */
    inline void setForceUpdate(bool forceUpdate)
        { _forceUpdate = forceUpdate; }

    /**
     * Sets icon of the sensor.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param class The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Defines the units of measurement of the sensor, if any.
     *
     * @param units For example: °C, %
     */
    inline void setUnitOfMeasurement(const char* unitOfMeasurement)
        { _unitOfMeasurement = unitOfMeasurement; }

protected:
    virtual void buildSerializer() override final;
    virtual void onMqttConnected() override;

private:
    /// The device class. It can be nullptr.
    const char* _deviceClass;

    /// The force update flag for the HA panel.
    bool _forceUpdate;

    /// The icon of the sensor. It can be nullptr.
    const char* _icon;

    /// The unit of measurement for the sensor. It can be nullptr.
    const char* _unitOfMeasurement;
};

#endif
#endif
