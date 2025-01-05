#ifndef AHA_HADICTIONARY_H
#define AHA_HADICTIONARY_H

// components
extern const char HAComponentBinarySensor[];
extern const char HAComponentButton[];
extern const char HAComponentCamera[];
extern const char HAComponentCover[];
extern const char HAComponentDeviceTracker[];
extern const char HAComponentDeviceAutomation[];
extern const char HAComponentLock[];
extern const char HAComponentNumber[];
extern const char HAComponentSelect[];
extern const char HAComponentSensor[];
extern const char HAComponentSwitch[];
extern const char HAComponentTag[];
extern const char HAComponentScene[];
extern const char HAComponentFan[];
extern const char HAComponentLight[];
extern const char HAComponentClimate[];

// decorators
extern const char HASerializerSlash[];
extern const char HASerializerJsonDataPrefix[];
extern const char HASerializerJsonDataSuffix[];
extern const char HASerializerJsonPropertyPrefix[];
extern const char HASerializerJsonPropertySuffix[];
extern const char HASerializerJsonEscapeChar[];
extern const char HASerializerJsonPropertiesSeparator[];
extern const char HASerializerJsonArrayPrefix[];
extern const char HASerializerJsonArraySuffix[];
extern const char HASerializerUnderscore[];

// properties
extern const char HADeviceIdentifiersProperty[];
extern const char HADeviceManufacturerProperty[];
extern const char HADeviceModelProperty[];
extern const char HAIPProperty[];
extern const char HADeviceConfigurationUrlProperty[];
extern const char HADeviceSoftwareVersionProperty[];
extern const char HANameProperty[];
extern const char HAUniqueIdProperty[];
extern const char HADeviceProperty[];
extern const char HADeviceClassProperty[];
extern const char HAIconProperty[];
extern const char HARetainProperty[];
extern const char HASourceTypeProperty[];
extern const char HAEncodingProperty[];
extern const char HAOptimisticProperty[];
extern const char HAAutomationTypeProperty[];
extern const char HATypeProperty[];
extern const char HASubtypeProperty[];
extern const char HAForceUpdateProperty[];
extern const char HAUnitOfMeasurementProperty[];
extern const char HAValueTemplateProperty[];
extern const char HAOptionsProperty[];
extern const char HAMinProperty[];
extern const char HAMaxProperty[];
extern const char HAStepProperty[];
extern const char HAModeProperty[];
extern const char HACommandTemplateProperty[];
extern const char HASpeedRangeMaxProperty[];
extern const char HASpeedRangeMinProperty[];
extern const char HABrightnessScaleProperty[];
extern const char HAMinMiredsProperty[];
extern const char HAMaxMiredsProperty[];
extern const char HATemperatureUnitProperty[];
extern const char HAMinTempProperty[];
extern const char HAMaxTempProperty[];
extern const char HATempStepProperty[];
extern const char HAFanModesProperty[];
extern const char HASwingModesProperty[];
extern const char HAModesProperty[];
extern const char HATemperatureCommandTemplateProperty[];
extern const char HAPayloadOnProperty[];

// topics
extern const char HAConfigTopic[];
extern const char HAAvailabilityTopic[];
extern const char HATopic[];
extern const char HAStateTopic[];
extern const char HACommandTopic[];
extern const char HAPositionTopic[];
extern const char HAPercentageStateTopic[];
extern const char HAPercentageCommandTopic[];
extern const char HABrightnessCommandTopic[];
extern const char HABrightnessStateTopic[];
extern const char HAColorTemperatureCommandTopic[];
extern const char HAColorTemperatureStateTopic[];
extern const char HACurrentTemperatureTopic[];
extern const char HAActionTopic[];
extern const char HAAuxCommandTopic[];
extern const char HAAuxStateTopic[];
extern const char HAPowerCommandTopic[];
extern const char HAFanModeCommandTopic[];
extern const char HAFanModeStateTopic[];
extern const char HASwingModeCommandTopic[];
extern const char HASwingModeStateTopic[];
extern const char HAModeCommandTopic[];
extern const char HAModeStateTopic[];
extern const char HATemperatureCommandTopic[];
extern const char HATemperatureStateTopic[];
extern const char HARGBCommandTopic[];
extern const char HARGBStateTopic[];

// misc
extern const char HAOnline[];
extern const char HAOffline[];
extern const char HAStateOn[];
extern const char HAStateOff[];
extern const char HAStateLocked[];
extern const char HAStateUnlocked[];
extern const char HAStateNone[];
extern const char HATrue[];
extern const char HAFalse[];
extern const char HAHome[];
extern const char HANotHome[];
extern const char HATrigger[];
extern const char HAModeBox[];
extern const char HAModeSlider[];

// covers
extern const char HAClosedState[];
extern const char HAClosingState[];
extern const char HAOpenState[];
extern const char HAOpeningState[];
extern const char HAStoppedState[];

// commands
extern const char HAOpenCommand[];
extern const char HACloseCommand[];
extern const char HAStopCommand[];
extern const char HALockCommand[];
extern const char HAUnlockCommand[];

// device tracker
extern const char HAGPSType[];
extern const char HARouterType[];
extern const char HABluetoothType[];
extern const char HABluetoothLEType[];

// camera
extern const char HAEncodingBase64[];

// trigger
extern const char HAButtonShortPressType[];
extern const char HAButtonShortReleaseType[];
extern const char HAButtonLongPressType[];
extern const char HAButtonLongReleaseType[];
extern const char HAButtonDoublePressType[];
extern const char HAButtonTriplePressType[];
extern const char HAButtonQuadruplePressType[];
extern const char HAButtonQuintuplePressType[];
extern const char HATurnOnSubtype[];
extern const char HATurnOffSubtype[];
extern const char HAButton1Subtype[];
extern const char HAButton2Subtype[];
extern const char HAButton3Subtype[];
extern const char HAButton4Subtype[];
extern const char HAButton5Subtype[];
extern const char HAButton6Subtype[];

// actions
extern const char HAActionOff[];
extern const char HAActionHeating[];
extern const char HAActionCooling[];
extern const char HAActionDrying[];
extern const char HAActionIdle[];
extern const char HAActionFan[];

// fan modes
extern const char HAFanModeAuto[];
extern const char HAFanModeLow[];
extern const char HAFanModeMedium[];
extern const char HAFanModeHigh[];

// swing modes
extern const char HASwingModeOn[];
extern const char HASwingModeOff[];

// HVAC modes
extern const char HAModeAuto[];
extern const char HAModeOff[];
extern const char HAModeCool[];
extern const char HAModeHeat[];
extern const char HAModeDry[];
extern const char HAModeFanOnly[];

// other
extern const char HAHexMap[];

// value templates
extern const char HAValueTemplateFloatP1[];
extern const char HAValueTemplateFloatP2[];
extern const char HAValueTemplateFloatP3[];
extern const char HATemperatureUnitC[];
extern const char HATemperatureUnitF[];

#endif
