
#include <Arduino.h>

#include "HASerializerArray.h"
#include "HADictionary.h"

HASerializerArray::HASerializerArray(const uint8_t size, const bool progmemItems) :
    _progmemItems(progmemItems),
    _size(size),
    _itemsNb(0),
    _items(new ItemType[size])
{

}

HASerializerArray::~HASerializerArray()
{
    delete[] _items;
}

bool HASerializerArray::add(ItemType item)
{
    if (_itemsNb >= _size) {
        return false;
    }

    _items[_itemsNb++] = item;
    return true;
}

uint16_t HASerializerArray::calculateSize() const
{
    uint16_t size =
        strlen_P(HASerializerJsonArrayPrefix) +
        strlen_P(HASerializerJsonArraySuffix);

    if (_itemsNb == 0) {
        return size;
    }

    // separators between elements
    size += (_itemsNb - 1) * strlen_P(HASerializerJsonPropertiesSeparator);

    for (uint8_t i = 0; i < _itemsNb; i++) {
        size +=
            2 * strlen_P(HASerializerJsonEscapeChar)
            + (_progmemItems ? strlen_P(_items[i]) : strlen(_items[i]));
    }

    return size;
}

bool HASerializerArray::serialize(char* output) const
{
    if (!output) {
        return false;
    }

    strcat_P(output, HASerializerJsonArrayPrefix);

    for (uint8_t i = 0; i < _itemsNb; i++) {
        if (i > 0) {
            strcat_P(output, HASerializerJsonPropertiesSeparator);
        }

        strcat_P(output, HASerializerJsonEscapeChar);

        if (_progmemItems) {
            strcat_P(output, _items[i]);
        } else {
            strcat(output, _items[i]);
        }

        strcat_P(output, HASerializerJsonEscapeChar);
    }

    strcat_P(output, HASerializerJsonArraySuffix);
    return true;
}

void HASerializerArray::clear()
{
    _itemsNb = 0;
}
