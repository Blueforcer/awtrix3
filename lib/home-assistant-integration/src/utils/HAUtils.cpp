#ifdef ARDUINO_ARCH_SAMD
#include <avr/dtostrf.h>
#endif

#include <Arduino.h>
#include "HAUtils.h"
#include "HADictionary.h"

bool HAUtils::endsWith(const char* str, const char* suffix)
{
    if (str == nullptr || suffix == nullptr) {
        return false;
    }

    const uint16_t lenstr = strlen(str);
    const uint16_t lensuffix = strlen(suffix);

    if (lensuffix > lenstr || lenstr == 0 || lensuffix == 0) {
        return false;
    }

    return (strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0);
}

void HAUtils::byteArrayToStr(
    char* dst,
    const byte* src,
    const uint16_t length
)
{
    for (uint8_t i = 0; i < length; i++) {
        dst[i*2] = pgm_read_byte(&HAHexMap[((char)src[i] & 0XF0) >> 4]);
        dst[i*2+1] = pgm_read_byte(&HAHexMap[((char)src[i] & 0x0F)]);
    }

    dst[length * 2] = 0;
}

char* HAUtils::byteArrayToStr(
    const byte* src,
    const uint16_t length
)
{
    char* dst = new char[(length * 2) + 1]; // include null terminator
    byteArrayToStr(dst, src, length);

    return dst;
}
