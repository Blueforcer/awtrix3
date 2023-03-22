#include "HANumeric.h"

const uint8_t HANumeric::MaxDigitsNb = 19;

HANumeric HANumeric::fromStr(const uint8_t* buffer, const uint16_t length)
{
    if (length == 0) {
        return HANumeric();
    }

    const uint8_t* firstCh = &buffer[0];
    int64_t out = 0;
    bool isSigned = false;

    if (*firstCh == '-') {
        isSigned = true;
        firstCh++;
    }

    uint8_t digitsNb = isSigned ? length - 1 : length;
    if (digitsNb > MaxDigitsNb) {
        return HANumeric();
    }

    uint64_t base = 1;
    const uint8_t* ptr = &buffer[length - 1];

    while (ptr >= firstCh) {
        uint8_t digit = *ptr - '0';
        if (digit > 9) {
            return HANumeric();
        }

        out += digit * base;
        ptr--;
        base *= 10;
    }

    return HANumeric(isSigned ? out * -1 : out);
}

HANumeric::HANumeric():
    _isSet(false),
    _value(0),
    _precision(0)
{

}

HANumeric::HANumeric(const float value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * static_cast<float>(getPrecisionBase());
}

HANumeric::HANumeric(const int8_t value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * static_cast<int32_t>(getPrecisionBase());
}

HANumeric::HANumeric(const int16_t value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * static_cast<int32_t>(getPrecisionBase());
}

HANumeric::HANumeric(const int32_t value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * static_cast<int32_t>(getPrecisionBase());
}

HANumeric::HANumeric(const uint8_t value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * getPrecisionBase();
}

HANumeric::HANumeric(const uint16_t value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * getPrecisionBase();
}

HANumeric::HANumeric(const uint32_t value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * getPrecisionBase();
}

#ifdef __SAMD21G18A__
HANumeric::HANumeric(const int value, const uint8_t precision):
    _isSet(true),
    _precision(precision)
{
    _value = value * static_cast<int>(getPrecisionBase());
}
#endif

HANumeric::HANumeric(const int64_t value):
    _isSet(true),
    _value(value),
    _precision(0)
{

}

uint32_t HANumeric::getPrecisionBase() const
{
    // using pow() increases the flash size by ~2KB
    switch (_precision) {
    case 1:
        return 10;

    case 2:
        return 100;

    case 3:
        return 1000;

    default:
        return 1;
    }
}

uint8_t HANumeric::calculateSize() const
{
    if (!_isSet) {
        return 0;
    }

    int64_t value = _value;
    const bool isSigned = value < 0;

    if (isSigned) {
        value *= -1;
    }

    uint8_t digitsNb = 1;
    while (value > 9) {
        value /= 10;
        digitsNb++;
    }

    if (isSigned) {
        digitsNb++; // sign
    }

    if (_precision > 0) {
        if (value == 0) {
            return 1;
        }

        // one digit + dot + decimal digits (+ sign)
        const uint8_t minValue = isSigned ? _precision + 3 : _precision + 2;
        return digitsNb >= minValue ? digitsNb + 1 : minValue;
    }

    return digitsNb;
}

uint16_t HANumeric::toStr(char* dst) const
{
    char* prefixCh = &dst[0];
    if (!_isSet || _value == 0) {
        *prefixCh = '0';
        return 1;
    }

    int64_t value = _value;
    const uint8_t numberLength = calculateSize();
    if (value < 0) {
        value *= -1;
        *prefixCh = '-';
        prefixCh++;
    }

    if (_precision > 0) {
        uint8_t i = _precision;
        char* dotPtr = prefixCh + 1;
        do {
            *prefixCh = '0';
            prefixCh++;
        } while(i-- > 0);

        *dotPtr = '.';
    }

    char* ch = &dst[numberLength - 1];
    char* lastCh = ch;
    char* dotPos = _precision > 0 ? &dst[numberLength - 1 - _precision] : nullptr;

    while (value != 0) {
        if (ch == dotPos) {
            *dotPos = '.';
            ch--;
            continue;
        }

        *ch = (value % 10) + '0';
        value /= 10;
        ch--;
    }

    return lastCh - &dst[0] + 1;
}
