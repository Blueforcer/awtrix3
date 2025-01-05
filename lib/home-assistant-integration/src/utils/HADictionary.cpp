#include <Arduino.h>
#include "HADictionary.h"

// components
const char HAComponentBinarySensor[] PROGMEM = {"binary_sensor"};
const char HAComponentButton[] PROGMEM = {"button"};
const char HAComponentCamera[] PROGMEM = {"camera"};
const char HAComponentCover[] PROGMEM = {"cover"};
const char HAComponentDeviceTracker[] PROGMEM = {"device_tracker"};
const char HAComponentDeviceAutomation[] PROGMEM = {"device_automation"};
const char HAComponentLock[] PROGMEM = {"lock"};
const char HAComponentNumber[] PROGMEM = {"number"};
const char HAComponentSelect[] PROGMEM = {"select"};
const char HAComponentSensor[] PROGMEM = {"sensor"};
const char HAComponentSwitch[] PROGMEM = {"switch"};
const char HAComponentTag[] PROGMEM = {"tag"};
const char HAComponentScene[] PROGMEM = {"scene"};
const char HAComponentFan[] PROGMEM = {"fan"};
const char HAComponentLight[] PROGMEM = {"light"};
const char HAComponentClimate[] PROGMEM = {"climate"};

// decorators
const char HASerializerSlash[] PROGMEM = {"/"};
const char HASerializerJsonDataPrefix[] PROGMEM = {"{"};
const char HASerializerJsonDataSuffix[] PROGMEM = {"}"};
const char HASerializerJsonPropertyPrefix[] PROGMEM = {"\""};
const char HASerializerJsonPropertySuffix[] PROGMEM = {"\":"};
const char HASerializerJsonEscapeChar[] PROGMEM = {"\""};
const char HASerializerJsonPropertiesSeparator[] PROGMEM = {","};
const char HASerializerJsonArrayPrefix[] PROGMEM = {"["};
const char HASerializerJsonArraySuffix[] PROGMEM = {"]"};
const char HASerializerUnderscore[] PROGMEM = {"_"};

// properties
const char HADeviceIdentifiersProperty[] PROGMEM = {"ids"};
const char HADeviceManufacturerProperty[] PROGMEM = {"mf"};
const char HADeviceModelProperty[] PROGMEM = {"mdl"};
const char HADeviceConfigurationUrlProperty[] PROGMEM = {"cu"};
const char HADeviceSoftwareVersionProperty[] PROGMEM = {"sw"};
const char HANameProperty[] PROGMEM = {"name"};
const char HAIPProperty[] PROGMEM = {"cu"};
const char HAUniqueIdProperty[] PROGMEM = {"uniq_id"};
const char HADeviceProperty[] PROGMEM = {"dev"};
const char HADeviceClassProperty[] PROGMEM = {"dev_cla"};
const char HAIconProperty[] PROGMEM = {"ic"};
const char HARetainProperty[] PROGMEM = {"ret"};
const char HASourceTypeProperty[] PROGMEM = {"src_type"};
const char HAEncodingProperty[] PROGMEM = {"e"};
const char HAOptimisticProperty[] PROGMEM = {"opt"};
const char HAAutomationTypeProperty[] PROGMEM = {"atype"};
const char HATypeProperty[] PROGMEM = {"type"};
const char HASubtypeProperty[] PROGMEM = {"stype"};
const char HAForceUpdateProperty[] PROGMEM = {"frc_upd"};
const char HAUnitOfMeasurementProperty[] PROGMEM = {"unit_of_meas"};
const char HAValueTemplateProperty[] PROGMEM = {"val_tpl"};
const char HAOptionsProperty[] PROGMEM = {"options"};
const char HAMinProperty[] PROGMEM = {"min"};
const char HAMaxProperty[] PROGMEM = {"max"};
const char HAStepProperty[] PROGMEM = {"step"};
const char HAModeProperty[] PROGMEM = {"mode"};
const char HACommandTemplateProperty[] PROGMEM = {"cmd_tpl"};
const char HASpeedRangeMaxProperty[] PROGMEM = {"spd_rng_max"};
const char HASpeedRangeMinProperty[] PROGMEM = {"spd_rng_min"};
const char HABrightnessScaleProperty[] PROGMEM = {"bri_scl"};
const char HAMinMiredsProperty[] PROGMEM = {"min_mirs"};
const char HAMaxMiredsProperty[] PROGMEM = {"max_mirs"};
const char HATemperatureUnitProperty[] PROGMEM = {"temp_unit"};
const char HAMinTempProperty[] PROGMEM = {"min_temp"};
const char HAMaxTempProperty[] PROGMEM = {"max_temp"};
const char HATempStepProperty[] PROGMEM = {"temp_step"};
const char HAFanModesProperty[] PROGMEM = {"fan_modes"};
const char HASwingModesProperty[] PROGMEM = {"swing_modes"};
const char HAModesProperty[] PROGMEM = {"modes"};
const char HATemperatureCommandTemplateProperty[] PROGMEM = {"temp_cmd_tpl"};
const char HAPayloadOnProperty[] PROGMEM = {"pl_on"};

// topics
const char HAConfigTopic[] PROGMEM = {"config"};
const char HAAvailabilityTopic[] PROGMEM = {"avty_t"};
const char HATopic[] PROGMEM = {"t"};
const char HAStateTopic[] PROGMEM = {"stat_t"};
const char HACommandTopic[] PROGMEM = {"cmd_t"};
const char HAPositionTopic[] PROGMEM = {"pos_t"};
const char HAPercentageStateTopic[] PROGMEM = {"pct_stat_t"};
const char HAPercentageCommandTopic[] PROGMEM = {"pct_cmd_t"};
const char HABrightnessCommandTopic[] PROGMEM = {"bri_cmd_t"};
const char HABrightnessStateTopic[] PROGMEM = {"bri_stat_t"};
const char HAColorTemperatureCommandTopic[] PROGMEM = {"clr_temp_cmd_t"};
const char HAColorTemperatureStateTopic[] PROGMEM = {"clr_temp_stat_t"};
const char HACurrentTemperatureTopic[] PROGMEM = {"curr_temp_t"};
const char HAActionTopic[] PROGMEM = {"act_t"};
const char HAAuxCommandTopic[] PROGMEM = {"aux_cmd_t"};
const char HAAuxStateTopic[] PROGMEM = {"aux_stat_t"};
const char HAPowerCommandTopic[] PROGMEM = {"pow_cmd_t"};
const char HAFanModeCommandTopic[] PROGMEM = {"fan_mode_cmd_t"};
const char HAFanModeStateTopic[] PROGMEM = {"fan_mode_stat_t"};
const char HASwingModeCommandTopic[] PROGMEM = {"swing_mode_cmd_t"};
const char HASwingModeStateTopic[] PROGMEM = {"swing_mode_stat_t"};
const char HAModeCommandTopic[] PROGMEM = {"mode_cmd_t"};
const char HAModeStateTopic[] PROGMEM = {"mode_stat_t"};
const char HATemperatureCommandTopic[] PROGMEM = {"temp_cmd_t"};
const char HATemperatureStateTopic[] PROGMEM = {"temp_stat_t"};
const char HARGBCommandTopic[] PROGMEM = {"rgb_cmd_t"};
const char HARGBStateTopic[] PROGMEM = {"rgb_stat_t"};

// misc
const char HAOnline[] PROGMEM = {"online"};
const char HAOffline[] PROGMEM = {"offline"};
const char HAStateOn[] PROGMEM = {"ON"};
const char HAStateOff[] PROGMEM = {"OFF"};
const char HAStateLocked[] PROGMEM = {"LOCKED"};
const char HAStateUnlocked[] PROGMEM = {"UNLOCKED"};
const char HAStateNone[] PROGMEM = {"None"};
const char HATrue[] PROGMEM = {"true"};
const char HAFalse[] PROGMEM = {"false"};
const char HAHome[] PROGMEM = {"home"};
const char HANotHome[] PROGMEM = {"not_home"};
const char HATrigger[] PROGMEM = {"trigger"};
const char HAModeBox[] PROGMEM = {"box"};
const char HAModeSlider[] PROGMEM = {"slider"};

// covers
const char HAClosedState[] PROGMEM = {"closed"};
const char HAClosingState[] PROGMEM = {"closing"};
const char HAOpenState[] PROGMEM = {"open"};
const char HAOpeningState[] PROGMEM = {"opening"};
const char HAStoppedState[] PROGMEM = {"stopped"};

// commands
const char HAOpenCommand[] PROGMEM = {"OPEN"};
const char HACloseCommand[] PROGMEM = {"CLOSE"};
const char HAStopCommand[] PROGMEM = {"STOP"};
const char HALockCommand[] PROGMEM = {"LOCK"};
const char HAUnlockCommand[] PROGMEM = {"UNLOCK"};

// device tracker
const char HAGPSType[] PROGMEM = {"gps"};
const char HARouterType[] PROGMEM = {"router"};
const char HABluetoothType[] PROGMEM = {"bluetooth"};
const char HABluetoothLEType[] PROGMEM = {"bluetooth_le"};

// camera
const char HAEncodingBase64[] PROGMEM = {"b64"};

// trigger
const char HAButtonShortPressType[] PROGMEM = {"button_short_press"};
const char HAButtonShortReleaseType[] PROGMEM = {"button_short_release"};
const char HAButtonLongPressType[] PROGMEM = {"button_long_press"};
const char HAButtonLongReleaseType[] PROGMEM = {"button_long_release"};
const char HAButtonDoublePressType[] PROGMEM = {"button_double_press"};
const char HAButtonTriplePressType[] PROGMEM = {"button_triple_press"};
const char HAButtonQuadruplePressType[] PROGMEM = {"button_quadruple_press"};
const char HAButtonQuintuplePressType[] PROGMEM = {"button_quintuple_press"};
const char HATurnOnSubtype[] PROGMEM = {"turn_on"};
const char HATurnOffSubtype[] PROGMEM = {"turn_off"};
const char HAButton1Subtype[] PROGMEM = {"button_1"};
const char HAButton2Subtype[] PROGMEM = {"button_2"};
const char HAButton3Subtype[] PROGMEM = {"button_3"};
const char HAButton4Subtype[] PROGMEM = {"button_4"};
const char HAButton5Subtype[] PROGMEM = {"button_5"};
const char HAButton6Subtype[] PROGMEM = {"button_6"};

// actions
const char HAActionOff[] PROGMEM = {"off"};
const char HAActionHeating[] PROGMEM = {"heating"};
const char HAActionCooling[] PROGMEM = {"cooling"};
const char HAActionDrying[] PROGMEM = {"drying"};
const char HAActionIdle[] PROGMEM = {"idle"};
const char HAActionFan[] PROGMEM = {"fan"};

// fan modes
const char HAFanModeAuto[] PROGMEM = {"auto"};
const char HAFanModeLow[] PROGMEM = {"low"};
const char HAFanModeMedium[] PROGMEM = {"medium"};
const char HAFanModeHigh[] PROGMEM = {"high"};

// swing modes
const char HASwingModeOn[] PROGMEM = {"on"};
const char HASwingModeOff[] PROGMEM = {"off"};

// HVAC modes
const char HAModeAuto[] PROGMEM = {"auto"};
const char HAModeOff[] PROGMEM = {"off"};
const char HAModeCool[] PROGMEM = {"cool"};
const char HAModeHeat[] PROGMEM = {"heat"};
const char HAModeDry[] PROGMEM = {"dry"};
const char HAModeFanOnly[] PROGMEM = {"fan_only"};

// other
const char HAHexMap[] PROGMEM = {"0123456789abcdef"};

// value templates
const char HAValueTemplateFloatP1[] PROGMEM = {"{{int(float(value)*10**1)}}"};
const char HAValueTemplateFloatP2[] PROGMEM = {"{{int(float(value)*10**2)}}"};
const char HAValueTemplateFloatP3[] PROGMEM = {"{{int(float(value)*10**3)}}"};
const char HATemperatureUnitC[] PROGMEM = {"C"};
const char HATemperatureUnitF[] PROGMEM = {"F"};
