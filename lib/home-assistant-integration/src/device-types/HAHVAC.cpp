#include "HAHVAC.h"
#ifndef EX_ARDUINOHA_HVAC

#include "../HAMqtt.h"
#include "../utils/HAUtils.h"
#include "../utils/HASerializer.h"

const uint8_t HAHVAC::DefaultFanModes = AutoFanMode | LowFanMode | MediumFanMode | HighFanMode;
const uint8_t HAHVAC::DefaultSwingModes = OnSwingMode | OffSwingMode;
const uint8_t HAHVAC::DefaultModes = AutoMode | OffMode | CoolMode | HeatMode | DryMode | FanOnlyMode;

HAHVAC::HAHVAC(
    const char* uniqueId,
    const uint16_t features,
    const NumberPrecision precision
) :
    HABaseDeviceType(AHATOFSTR(HAComponentClimate), uniqueId),
    _features(features),
    _precision(precision),
    _icon(nullptr),
    _retain(false),
    _CURRENT_TEMPerature(),
    _action(UnknownAction),
    _temperatureUnit(DefaultUnit),
    _minTemp(),
    _maxTemp(),
    _tempStep(),
    _auxCallback(nullptr),
    _auxState(false),
    _powerCallback(nullptr),
    _fanMode(UnknownFanMode),
    _fanModes(DefaultFanModes),
    _fanModesSerializer(nullptr),
    _fanModeCallback(nullptr),
    _swingMode(UnknownSwingMode),
    _swingModes(DefaultSwingModes),
    _swingModesSerializer(nullptr),
    _swingModeCallback(nullptr),
    _mode(UnknownMode),
    _modes(DefaultModes),
    _modesSerializer(nullptr),
    _modeCallback(nullptr),
    _targetTemperature(),
    _targetTemperatureCallback(nullptr)
{
    if (_features & FanFeature) {
        _fanModesSerializer = new HASerializerArray(4);
    }

    if (_features & SwingFeature) {
        _swingModesSerializer = new HASerializerArray(2);
    }

    if (_features & ModesFeature) {
        _modesSerializer = new HASerializerArray(6);
    }
}

HAHVAC::~HAHVAC()
{
    if (_fanModesSerializer) {
        delete _fanModesSerializer;
    }

    if (_swingModesSerializer) {
        delete _swingModesSerializer;
    }

    if (_modesSerializer) {
        delete _modesSerializer;
    }
}

bool HAHVAC::setCurrentTemperature(const HANumeric& temperature, const bool force)
{
    if (temperature.getPrecision() != _precision) {
        return false;
    }

    if (!force && temperature == _CURRENT_TEMPerature) {
        return true;
    }

    if (publishCurrentTemperature(temperature)) {
        _CURRENT_TEMPerature = temperature;
        return true;
    }

    return false;
}

bool HAHVAC::setAction(const Action action, const bool force)
{
    if (!force && action == _action) {
        return true;
    }

    if (publishAction(action)) {
        _action = action;
        return true;
    }

    return false;
}

bool HAHVAC::setAuxState(const bool state, const bool force)
{
    if (!force && state == _auxState) {
        return true;
    }

    if (publishAuxState(state)) {
        _auxState = state;
        return true;
    }

    return false;
}

bool HAHVAC::setFanMode(const FanMode mode, const bool force)
{
    if (!force && mode == _fanMode) {
        return true;
    }

    if (publishFanMode(mode)) {
        _fanMode = mode;
        return true;
    }

    return false;
}

bool HAHVAC::setSwingMode(const SwingMode mode, const bool force)
{
    if (!force && mode == _swingMode) {
        return true;
    }

    if (publishSwingMode(mode)) {
        _swingMode = mode;
        return true;
    }

    return false;
}

bool HAHVAC::setMode(const Mode mode, const bool force)
{
    if (!force && mode == _mode) {
        return true;
    }

    if (publishMode(mode)) {
        _mode = mode;
        return true;
    }

    return false;
}

bool HAHVAC::setTargetTemperature(const HANumeric& temperature, const bool force)
{
    if (temperature.getPrecision() != _precision) {
        return false;
    }

    if (!force && temperature == _targetTemperature) {
        return true;
    }

    if (publishTargetTemperature(temperature)) {
        _targetTemperature = temperature;
        return true;
    }

    return false;
}

void HAHVAC::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 27); // 27 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAUniqueIdProperty), _uniqueId);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);

    if (_retain) {
        _serializer->set(
            AHATOFSTR(HARetainProperty),
            &_retain,
            HASerializer::BoolPropertyType
        );
    }

    if (_features & ActionFeature) {
        _serializer->topic(AHATOFSTR(HAActionTopic));
    }

    if (_features & AuxHeatingFeature) {
        _serializer->topic(AHATOFSTR(HAAuxCommandTopic));
        _serializer->topic(AHATOFSTR(HAAuxStateTopic));
    }

    if (_features & PowerFeature) {
        _serializer->topic(AHATOFSTR(HAPowerCommandTopic));
    }

    if (_features & FanFeature) {
        _serializer->topic(AHATOFSTR(HAFanModeCommandTopic));
        _serializer->topic(AHATOFSTR(HAFanModeStateTopic));

        if (_fanModes != DefaultFanModes) {
            _fanModesSerializer->clear();

            if (_fanModes & AutoFanMode) {
                _fanModesSerializer->add(HAFanModeAuto);
            }

            if (_fanModes & LowFanMode) {
                _fanModesSerializer->add(HAFanModeLow);
            }

            if (_fanModes & MediumFanMode) {
                _fanModesSerializer->add(HAFanModeMedium);
            }

            if (_fanModes & HighFanMode) {
                _fanModesSerializer->add(HAFanModeHigh);
            }

            _serializer->set(
                AHATOFSTR(HAFanModesProperty),
                _fanModesSerializer,
                HASerializer::ArrayPropertyType
            );
        }
    }

    if (_features & SwingFeature) {
        _serializer->topic(AHATOFSTR(HASwingModeCommandTopic));
        _serializer->topic(AHATOFSTR(HASwingModeStateTopic));

        if (_swingModes != DefaultSwingModes) {
            _swingModesSerializer->clear();

            if (_swingModes & OnSwingMode) {
                _swingModesSerializer->add(HASwingModeOn);
            }

            if (_swingModes & OffSwingMode) {
                _swingModesSerializer->add(HASwingModeOff);
            }

            _serializer->set(
                AHATOFSTR(HASwingModesProperty),
                _swingModesSerializer,
                HASerializer::ArrayPropertyType
            );
        }
    }

    if (_features & ModesFeature) {
        _serializer->topic(AHATOFSTR(HAModeCommandTopic));
        _serializer->topic(AHATOFSTR(HAModeStateTopic));

        if (_modes != DefaultModes) {
            _modesSerializer->clear();

            if (_modes & AutoMode) {
                _modesSerializer->add(HAModeAuto);
            }

            if (_modes & OffMode) {
                _modesSerializer->add(HAModeOff);
            }

            if (_modes & CoolMode) {
                _modesSerializer->add(HAModeCool);
            }

            if (_modes & HeatMode) {
                _modesSerializer->add(HAModeHeat);
            }

            if (_modes & DryMode) {
                _modesSerializer->add(HAModeDry);
            }

            if (_modes & FanOnlyMode) {
                _modesSerializer->add(HAModeFanOnly);
            }

            _serializer->set(
                AHATOFSTR(HAModesProperty),
                _modesSerializer,
                HASerializer::ArrayPropertyType
            );
        }
    }

    if (_features & TargetTemperatureFeature) {
        _serializer->topic(AHATOFSTR(HATemperatureCommandTopic));
        _serializer->topic(AHATOFSTR(HATemperatureStateTopic));
        _serializer->set(
            AHATOFSTR(HATemperatureCommandTemplateProperty),
            getCommandWithFloatTemplate(),
            HASerializer::ProgmemPropertyValue
        );
    }

    if (_temperatureUnit != DefaultUnit) {
        const __FlashStringHelper *unitStr = _temperatureUnit == CelsiusUnit
            ? AHATOFSTR(HATemperatureUnitC)
            : AHATOFSTR(HATemperatureUnitF);

        _serializer->set(
            AHATOFSTR(HATemperatureUnitProperty),
            unitStr,
            HASerializer::ProgmemPropertyValue
        );
    }

    if (_minTemp.isSet()) {
        _serializer->set(
            AHATOFSTR(HAMinTempProperty),
            &_minTemp,
            HASerializer::NumberPropertyType
        );
    }

    if (_maxTemp.isSet()) {
        _serializer->set(
            AHATOFSTR(HAMaxTempProperty),
            &_maxTemp,
            HASerializer::NumberPropertyType
        );
    }

    if (_tempStep.isSet()) {
        _serializer->set(
            AHATOFSTR(HATempStepProperty),
            &_tempStep,
            HASerializer::NumberPropertyType
        );
    }

    _serializer->topic(AHATOFSTR(HACurrentTemperatureTopic));
    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
}

void HAHVAC::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();

    if (!_retain) {
        publishCurrentTemperature(_CURRENT_TEMPerature);
        publishAction(_action);
        publishAuxState(_auxState);
        publishFanMode(_fanMode);
        publishSwingMode(_swingMode);
        publishMode(_mode);
        publishTargetTemperature(_targetTemperature);
    }

    if (_features & AuxHeatingFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAAuxCommandTopic));
    }

    if (_features & PowerFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAPowerCommandTopic));
    }

    if (_features & FanFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAFanModeCommandTopic));
    }

    if (_features & SwingFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HASwingModeCommandTopic));
    }

    if (_features & ModesFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HAModeCommandTopic));
    }

    if (_features & TargetTemperatureFeature) {
        subscribeTopic(uniqueId(), AHATOFSTR(HATemperatureCommandTopic));
    }
}

void HAHVAC::onMqttMessage(
    const char* topic,
    const uint8_t* payload,
    const uint16_t length
)
{
    if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAAuxCommandTopic)
    )) {
        handleAuxStateCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAPowerCommandTopic)
    )) {
        handlePowerCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAFanModeCommandTopic)
    )) {
        handleFanModeCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HASwingModeCommandTopic)
    )) {
        handleSwingModeCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HAModeCommandTopic)
    )) {
        handleModeCommand(payload, length);
    } else if (HASerializer::compareDataTopics(
        topic,
        uniqueId(),
        AHATOFSTR(HATemperatureCommandTopic)
    )) {
        handleTargetTemperatureCommand(payload, length);
    }
}

bool HAHVAC::publishCurrentTemperature(const HANumeric& temperature)
{
    if (!temperature.isSet()) {
        return false;
    }

    uint8_t size = temperature.calculateSize();
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    temperature.toStr(str);

    return publishOnDataTopic(
        AHATOFSTR(HACurrentTemperatureTopic),
        str,
        true
    );
}

bool HAHVAC::publishAction(const Action action)
{
    if (action == UnknownAction || !(_features & ActionFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (action) {
    case OffAction:
        stateStr = AHATOFSTR(HAActionOff);
        break;

    case HeatingAction:
        stateStr = AHATOFSTR(HAActionHeating);
        break;

    case CoolingAction:
        stateStr = AHATOFSTR(HAActionCooling);
        break;

    case DryingAction:
        stateStr = AHATOFSTR(HAActionDrying);
        break;

    case IdleAction:
        stateStr = AHATOFSTR(HAActionIdle);
        break;

    case FanAction:
        stateStr = AHATOFSTR(HAActionFan);
        break;

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAActionTopic),
        stateStr,
        true
    );
}

bool HAHVAC::publishAuxState(const bool state)
{
    if (!(_features & AuxHeatingFeature)) {
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAAuxStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

bool HAHVAC::publishFanMode(const FanMode mode)
{
    if (mode == UnknownFanMode || !(_features & FanFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (mode) {
    case AutoFanMode:
        stateStr = AHATOFSTR(HAFanModeAuto);
        break;

    case LowFanMode:
        stateStr = AHATOFSTR(HAFanModeLow);
        break;

    case MediumFanMode:
        stateStr = AHATOFSTR(HAFanModeMedium);
        break;

    case HighFanMode:
        stateStr = AHATOFSTR(HAFanModeHigh);
        break;

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAFanModeStateTopic),
        stateStr,
        true
    );
}

bool HAHVAC::publishSwingMode(const SwingMode mode)
{
    if (mode == UnknownSwingMode || !(_features & SwingFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (mode) {
    case OnSwingMode:
        stateStr = AHATOFSTR(HASwingModeOn);
        break;

    case OffSwingMode:
        stateStr = AHATOFSTR(HASwingModeOff);
        break;

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HASwingModeStateTopic),
        stateStr,
        true
    );
}

bool HAHVAC::publishMode(const Mode mode)
{
    if (mode == UnknownMode || !(_features & ModesFeature)) {
        return false;
    }

    const __FlashStringHelper *stateStr = nullptr;
    switch (mode) {
    case AutoMode:
        stateStr = AHATOFSTR(HAModeAuto);
        break;

    case OffMode:
        stateStr = AHATOFSTR(HAModeOff);
        break;

    case CoolMode:
        stateStr = AHATOFSTR(HAModeCool);
        break;

    case HeatMode:
        stateStr = AHATOFSTR(HAModeHeat);
        break;

    case DryMode:
        stateStr = AHATOFSTR(HAModeDry);
        break;

    case FanOnlyMode:
        stateStr = AHATOFSTR(HAModeFanOnly);
        break;

    default:
        return false;
    }

    return publishOnDataTopic(
        AHATOFSTR(HAModeStateTopic),
        stateStr,
        true
    );
}

bool HAHVAC::publishTargetTemperature(const HANumeric& temperature)
{
    if (!temperature.isSet()) {
        return false;
    }

    uint8_t size = temperature.calculateSize();
    if (size == 0) {
        return false;
    }

    char str[size + 1]; // with null terminator
    str[size] = 0;
    temperature.toStr(str);

    return publishOnDataTopic(
        AHATOFSTR(HATemperatureStateTopic),
        str,
        true
    );
}

void HAHVAC::handleAuxStateCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_auxCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _auxCallback(state, this);
}

void HAHVAC::handlePowerCommand(const uint8_t* cmd, const uint16_t length)
{
    (void)cmd;

    if (!_powerCallback) {
        return;
    }

    bool state = length == strlen_P(HAStateOn);
    _powerCallback(state, this);
}

void HAHVAC::handleFanModeCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_fanModeCallback) {
        return;
    }

    if (memcmp_P(cmd, HAFanModeAuto, length) == 0) {
        _fanModeCallback(AutoFanMode, this);
    } else if (memcmp_P(cmd, HAFanModeLow, length) == 0) {
        _fanModeCallback(LowFanMode, this);
    } else if (memcmp_P(cmd, HAFanModeMedium, length) == 0) {
        _fanModeCallback(MediumFanMode, this);
    } else if (memcmp_P(cmd, HAFanModeHigh, length) == 0) {
        _fanModeCallback(HighFanMode, this);
    }
}

void HAHVAC::handleSwingModeCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_swingModeCallback) {
        return;
    }

    if (memcmp_P(cmd, HASwingModeOn, length) == 0) {
        _swingModeCallback(OnSwingMode, this);
    } else if (memcmp_P(cmd, HASwingModeOff, length) == 0) {
        _swingModeCallback(OffSwingMode, this);
    }
}

void HAHVAC::handleModeCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_modeCallback) {
        return;
    }

    if (memcmp_P(cmd, HAModeAuto, length) == 0) {
        _modeCallback(AutoMode, this);
    } else if (memcmp_P(cmd, HAModeOff, length) == 0) {
        _modeCallback(OffMode, this);
    } else if (memcmp_P(cmd, HAModeCool, length) == 0) {
        _modeCallback(CoolMode, this);
    } else if (memcmp_P(cmd, HAModeHeat, length) == 0) {
        _modeCallback(HeatMode, this);
    }  else if (memcmp_P(cmd, HAModeDry, length) == 0) {
        _modeCallback(DryMode, this);
    }  else if (memcmp_P(cmd, HAModeFanOnly, length) == 0) {
        _modeCallback(FanOnlyMode, this);
    }
}

void HAHVAC::handleTargetTemperatureCommand(const uint8_t* cmd, const uint16_t length)
{
    if (!_targetTemperatureCallback) {
        return;
    }

    HANumeric number = HANumeric::fromStr(cmd, length);
    if (number.isSet()) {
        number.setPrecision(_precision);
        _targetTemperatureCallback(number, this);
    }
}

const __FlashStringHelper* HAHVAC::getCommandWithFloatTemplate()
{
    switch (_precision) {
    case PrecisionP1:
        return AHATOFSTR(HAValueTemplateFloatP1);

    case PrecisionP2:
        return AHATOFSTR(HAValueTemplateFloatP2);

    case PrecisionP3:
        return AHATOFSTR(HAValueTemplateFloatP3);

    default:
        return nullptr;
    }
}

#endif
