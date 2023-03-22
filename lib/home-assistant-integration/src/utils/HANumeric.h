
#ifndef AHA_NUMERIC_H
#define AHA_NUMERIC_H

#include <stdint.h>

/**
 * This class represents a numeric value that simplifies use of different types of numbers across the library.
 */
class HANumeric
{
public:
    /// The maximum number of digits that the base value can have (int64_t).
    static const uint8_t MaxDigitsNb;

    /**
     * Deserializes number from the given buffer.
     * Please note that the class expected buffer to contain the base number.
     * For example, deserializing `1234` number and setting precision to `1`
     * results in representation of `123.4` float.
     *
     * @param buffer The buffer that contains the number.
     * @param length The length of the buffer.
     */
    static HANumeric fromStr(const uint8_t* buffer, const uint16_t length);

    /**
     * Creates an empty number representation.
     */
    HANumeric();

    /**
     * Converts the given float into number representation of the given precision.
     * If the precision is set to zero the given float will be converted into integer.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const float value, const uint8_t precision);

    /**
     * Converts the given int8_t into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const int8_t value, const uint8_t precision);

    /**
     * Converts the given int16_t into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const int16_t value, const uint8_t precision);

    /**
     * Converts the given int32_t into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const int32_t value, const uint8_t precision);

    /**
     * Converts the given uint8_t into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const uint8_t value, const uint8_t precision);

    /**
     * Converts the given uint16_t into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const uint16_t value, const uint8_t precision);

    /**
     * Converts the given uint32_t into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const uint32_t value, const uint8_t precision);

#ifdef __SAMD21G18A__
    /**
     * Converts the given int into number representation of the given precision.
     * If the precision is greater than zero the given value will be converted to float.
     *
     * @param value The value that should be used as a base.
     * @param precision The number of digits in the decimal part.
     */
    HANumeric(const int value, const uint8_t precision);
#endif

    void operator= (const HANumeric& a) {
        if (!a.isSet()) {
            reset();
        } else {
            _isSet = a.isSet();
            _value = a.getBaseValue();
            _precision = a.getPrecision();
        }
    }

    bool operator== (const HANumeric& a) const {
        return (
            isSet() == a.isSet() &&
            getBaseValue() == a.getBaseValue() &&
            getPrecision() == a.getPrecision()
        );
    }

    /**
     * Returns multiplier that used to generate base value based on the precision.
     * The multiplier is generated using the formula: `pow(precision, 10)`.
     */
    uint32_t getPrecisionBase() const;

    /**
     * Returns size of the number
     */
    uint8_t calculateSize() const;

    /**
     * Converts the number to the string.
     *
     * @param dst Destination where the number will be saved.
     *            The null terminator is not added at the end.
     * @return The number of written characters.
     * @note The `dst` size should be calculated using HANumeric::calculateSize() method plus 1 extra byte for the null terminator.
     */
    uint16_t toStr(char* dst) const;

    /**
     * Returns true if the base value is set.
     */
    inline bool isSet() const
        { return _isSet; }

    /**
     * Sets the base value without converting it to the proper precision.
     */
    inline void setBaseValue(int64_t value)
        { _isSet = true; _value = value; }

    /**
     * Returns the base value of the number.
     */
    inline int64_t getBaseValue() const
        { return _value; }

    /**
     * Sets the precision of the number (number of digits in the decimal part).
     *
     * @param precision The precision to use.
     */
    inline void setPrecision(const uint8_t precision)
        { _precision = precision; }

    /**
     * Returns the precision of the number.
     */
    inline uint8_t getPrecision() const
        { return _precision; }

    /**
     * Resets the number to the defaults.
     */
    inline void reset()
        { _isSet = false; _value = 0; _precision = 0; }

    inline bool isUInt8() const
        { return _isSet && _precision == 0 && _value >= 0 && _value <= UINT8_MAX; }

    inline bool isUInt16() const
        { return _isSet && _precision == 0 && _value >= 0 && _value <= UINT16_MAX; }

    inline bool isUInt32() const
        { return _isSet && _precision == 0 && _value >= 0 && _value <= UINT32_MAX; }

    inline bool isInt8() const
        { return _isSet && _precision == 0 && _value >= INT8_MIN && _value <= INT8_MAX; }

    inline bool isInt16() const
        { return _isSet && _precision == 0 && _value >= INT16_MIN && _value <= INT16_MAX; }

    inline bool isInt32() const
        { return _isSet && _precision == 0 && _value >= INT32_MIN && _value <= INT32_MAX; }

    inline bool isFloat() const
        { return _isSet && _precision > 0; }

    inline uint8_t toUInt8() const
        { return static_cast<uint8_t>(_value); }

    inline uint16_t toUInt16() const
        { return static_cast<uint16_t>(_value); }

    inline uint32_t toUInt32() const
        { return static_cast<uint32_t>(_value); }

    inline int8_t toInt8() const
        { return static_cast<int8_t>(_value); }

    inline int16_t toInt16() const
        { return static_cast<int16_t>(_value); }

    inline int32_t toInt32() const
        { return static_cast<int32_t>(_value); }

    inline float toFloat() const
        { return _value / (float)getPrecisionBase(); }

private:
    bool _isSet;
    int64_t _value;
    uint8_t _precision;

    explicit HANumeric(const int64_t value);
};

#endif