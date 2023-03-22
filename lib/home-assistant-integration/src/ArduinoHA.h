#ifndef AHA_ARDUINOHA_H
#define AHA_ARDUINOHA_H

#include "HADevice.h"
#include "HAMqtt.h"
#include "device-types/HABinarySensor.h"
#include "device-types/HAButton.h"
//#include "device-types/HACamera.h"
//#include "device-types/HACover.h"
//#include "device-types/HADeviceTracker.h"
//#include "device-types/HADeviceTrigger.h"
//#include "device-types/HAFan.h"
//#include "device-types/HAHVAC.h"
#include "device-types/HALight.h"
//#include "device-types/HALock.h"
#include "device-types/HANumber.h"
//#include "device-types/HAScene.h"
#include "device-types/HASelect.h"
#include "device-types/HASensor.h"
#include "device-types/HASensorNumber.h"
#include "device-types/HASwitch.h"
//#include "device-types/HATagScanner.h"
#include "utils/HAUtils.h"
#include "utils/HANumeric.h"

#ifdef ARDUINOHA_TEST
#include "mocks/AUnitHelpers.h"
#include "mocks/PubSubClientMock.h"
#include "utils/HADictionary.h"
#include "utils/HASerializer.h"
#endif

#endif
