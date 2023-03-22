
#ifndef AHA_SERIALIZERARRAY_H
#define AHA_SERIALIZERARRAY_H

#include <stdint.h>

/**
 * HASerializerArray represents array of items that can be used as a HASerializer property.
 */
class HASerializerArray
{
public:
    typedef const char* ItemType;

    /**
     * Constructs HASerializerArray with the static size (number of elements).
     * The array is allocated dynamically in the memory based on the given size.
     *
     * @param size The desired number of elements that will be stored in the array.
     * @param progmemItems Specifies whether items are going to be stored in the flash memory.
     */
    HASerializerArray(const uint8_t size, const bool progmemItems = true);
    ~HASerializerArray();

    /**
     * Returns the number of elements that were added to the array.
     * It can be lower than size of the array.
     */
    inline uint8_t getItemsNb() const
        { return _itemsNb; }

    /**
     * Returns pointer to the array.
     */
    inline ItemType* getItems() const
        { return _items; }

    /**
     * Adds a new element to the array.
     * 
     * @param itemP Item to add (string).
     * @returns Returns `true` if item has been added to the array successfully.
     */
    bool add(ItemType item);

    /**
     * Calculates the size of the serialized array (JSON representation).
     */
    uint16_t calculateSize() const;

    /**
     * Serializes array as JSON to the given output.
     */
    bool serialize(char* output) const;

    /**
     * Clears the array.
     */
    void clear();

private:
    /// Specifies whether items are stored in the flash memory.
    const bool _progmemItems;

    /// The maximum size of the array.
    const uint8_t _size;

    /// The number of items that were added to the array.
    uint8_t _itemsNb;

    /// Pointer to the array elements.
    ItemType* _items;
};

#endif
