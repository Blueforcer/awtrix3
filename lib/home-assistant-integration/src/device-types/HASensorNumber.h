#ifndef AHA_HASENSORNUMBER_H
#define AHA_HASENSORNUMBER_H

#include "HASensor.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_SENSOR

#define _SET_VALUE_OVERLOAD(type) \
    /** @overload */ \
    inline bool setValue(const type value, const bool force = false) \
        { return setValue(HANumeric(value, _precision), force); }

#define _SET_CURRENT_VALUE_OVERLOAD(type) \
    /** @overload */ \
    inline void setCurrentValue(const type value) \
        { setCurrentValue(HANumeric(value, _precision)); }

/**
 * HASensorInteger allows to publish numeric values of a sensor that will be displayed in the HA panel.
 *
 * @note You can find more information about this class in HASensor documentation.
 */
class HASensorNumber : public HASensor
{
public:
    /**
     * @param uniqueId The unique ID of the sensor. It needs to be unique in a scope of your device.
     * @param precision Precision of the floating point number that will be displayed in the HA panel.
     */
    HASensorNumber(
        const char* uniqueId,
        const NumberPrecision precision = PrecisionP0
    );

    /**
     * Changes value of the sensor and publish MQTT message.
     * Please note that if a new value is the same as the previous one the MQTT message won't be published.
     *
     * @param value New value of the sensor. THe precision of the value needs to match precision of the sensor.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool setValue(const HANumeric& value, const bool force = false);

    _SET_VALUE_OVERLOAD(int8_t)
    _SET_VALUE_OVERLOAD(int16_t)
    _SET_VALUE_OVERLOAD(int32_t)
    _SET_VALUE_OVERLOAD(uint8_t)
    _SET_VALUE_OVERLOAD(uint16_t)
    _SET_VALUE_OVERLOAD(uint32_t)
    _SET_VALUE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_VALUE_OVERLOAD(int)
#endif

    /**
     * Sets the current value of the sensor without publishing it to Home Assistant.
     * This method may be useful if you want to change the value before the connection with the MQTT broker is acquired.
     *
     * @param value New value of the sensor.
     */
    inline void setCurrentValue(const HANumeric& value)
        { if (value.getPrecision() == _precision) { _currentValue = value; } }

    _SET_CURRENT_VALUE_OVERLOAD(int8_t)
    _SET_CURRENT_VALUE_OVERLOAD(int16_t)
    _SET_CURRENT_VALUE_OVERLOAD(int32_t)
    _SET_CURRENT_VALUE_OVERLOAD(uint8_t)
    _SET_CURRENT_VALUE_OVERLOAD(uint16_t)
    _SET_CURRENT_VALUE_OVERLOAD(uint32_t)
    _SET_CURRENT_VALUE_OVERLOAD(float)

#ifdef __SAMD21G18A__
    _SET_CURRENT_VALUE_OVERLOAD(int)
#endif

    /**
     * Returns the last known value of the sensor.
     * By default the value is not set.
     */
    inline const HANumeric& getCurrentValue() const
        { return _currentValue; }

protected:
    virtual void onMqttConnected() override;

private:
    /**
     * Publishes the MQTT message with the given value.
     *
     * @param state The value to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishValue(const HANumeric& value);

    /// The precision of the sensor. By default it's `HASensorNumber::PrecisionP0`.
    const NumberPrecision _precision;

    /// The current value of the sensor. By default the value is not set.
    HANumeric _currentValue;
};

#endif
#endif