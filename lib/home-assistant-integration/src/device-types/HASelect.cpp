#include "HASelect.h"
#ifndef EX_ARDUINOHA_SELECT

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HASelect::HASelect(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentSelect), uniqueId),
    _options(nullptr),
    _currentState(-1),
    _icon(nullptr),
    _retain(false),
    _optimistic(false),
    _commandCallback(nullptr)
{

}

HASelect::~HASelect()
{
    if (_options) {
        const uint8_t optionsNb = _options->getItemsNb();
        const HASerializerArray::ItemType* options = _options->getItems();

        if (optionsNb > 1) {
            for (uint8_t i = 0; i < optionsNb; i++) {
                delete options[i];
            }
        }

        delete _options;
    }
}

void HASelect::setOptions(const char* options)
{
    if (!options || _options) { // options can be set only once
        return;
    }

    const uint16_t optionsNb = countOptionsInString(options);
    if (optionsNb == 0) {
        return;
    }

    const uint16_t optionsLen = strlen(options) + 1; // include null terminator
    _options = new HASerializerArray(optionsNb, false);

    if (optionsNb == 1) {
        _options->add(options);
        return;
    }

    uint8_t optionLen = 0;
    for (uint16_t i = 0; i < optionsLen; i++) {
        if (options[i] == ';' || options[i] == 0) {
            if (optionLen == 0) {
                break;
            }

            char* option = new char[optionLen + 1]; // including null terminator
            option[optionLen] = 0;
            memcpy(option, &options[i - optionLen], optionLen);

            _options->add(option);
            optionLen = 0;
            continue;
        }

        optionLen++;
    }
}

bool HASelect::setState(const int8_t state, const bool force)
{
    if (!force && _currentState == state) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

void HASelect::buildSerializer()
{
    if (_serializer || !uniqueId() || !_options) {
        return;
    }

    _serializer = new HASerializer(this, 10); // 10 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(
        AHATOFSTR(HAOptionsProperty),
        _options,
        HASerializer::ArrayPropertyType
    );

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_optimistic) {
        _serializer->set(
            AHATOFSTR(HAOptimisticProperty),
            &_optimistic,
            HASerializer::BoolPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
    _serializer->topic(AHATOFSTR(HACommandTopic));
}

void HASelect::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishState(_currentState);
    }

    subscribeTopic(uniqueId(), AHATOFSTR(HACommandTopic));
}

void HASelect::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (_commandCallback && HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HACommandTopic)
    )) {
        const uint8_t optionsNb = _options->getItemsNb();
        const HASerializerArray::ItemType* options = _options->getItems();

        for (uint8_t i = 0; i < optionsNb; i++) {
            if (memcmp(payload, options[i], length) == 0) {
                _commandCallback(i, this);
                return;
            }
        }
    }
}

bool HASelect::publishState(const int8_t state)
{
    if (state == -1 || !_options || state >= _options->getItemsNb()) {
        return false;
    }

    const char* item = _options->getItems()[state];
    if (!item) {
        return false;
    }

    return publishOnDataTopic(AHATOFSTR(HAStateTopic), item, true);
}

uint8_t HASelect::countOptionsInString(const char* options) const
{
    // the given string is treated as a single option if there are no semicolons
    uint8_t optionsNb = 1;
    const uint16_t optionsLen = strlen(options);

    if (optionsLen == 0) {
        return 0;
    }

    for (uint8_t i = 0; i < optionsLen; i++) {
        if (options[i] == ';') {
            optionsNb++;
        }
    }

    return optionsNb;
}

#endif
