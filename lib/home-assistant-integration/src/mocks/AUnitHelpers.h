#ifndef AHA_AUNITHELPERS_H
#define AHA_AUNITHELPERS_H

#ifdef ARDUINOHA_TEST

#ifdef AUNITER
#include <Arduino.h>

#if defined(__AVR__)
#include <MemoryUsage.h>
#endif
#endif

#define initMqttTest(testDeviceId) \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device); \
    mqtt.setDataPrefix("testData"); \
    mqtt.begin("testHost", "testUser", "testPass");

#define assertNoMqttMessage() \
    assertTrue(mock->getFlushedMessagesNb() == 0);

#define assertMqttMessage(index, eTopic, eMessage, eRetained) { \
    const __FlashStringHelper* messageP = F(eMessage); \
    size_t messageLen = strlen_P(reinterpret_cast<const char *>(messageP)); \
    assertTrue(mock->getFlushedMessagesNb() > 0); \
    assertTrue(mock->getFlushedMessagesNb() > index); \
    MqttMessage* publishedMessage = mock->getFlushedMessages()[index]; \
    assertEqual(eTopic, publishedMessage->topic); \
    assertEqual(messageP, publishedMessage->buffer); \
    assertEqual(messageLen, publishedMessage->bufferSize - 1); \
    assertEqual(eRetained, publishedMessage->retained); \
}

#define assertSingleMqttMessage(eTopic, eMessage, eRetained) { \
    assertEqual(1, mock->getFlushedMessagesNb()); \
    assertMqttMessage(0, eTopic, eMessage, eRetained) \
}

#define assertEntityConfig(mock, entity, expectedJson) \
{ \
    mqtt.loop(); \
    assertMqttMessage(0, AHATOFSTR(ConfigTopic), expectedJson, true) \
    assertTrue(entity.getSerializer() == nullptr); \
}

#define assertEntityConfigOnTopic(mock, entity, topic, expectedJson) \
{ \
    mqtt.loop(); \
    assertMqttMessage(0, topic, expectedJson, true) \
    assertTrue(entity.getSerializer() == nullptr); \
}

#ifdef AUNITER

#if defined(__AVR__)
#define AHA_FREERAM mu_freeRam()
#elif defined(ESP8266) || defined(ESP32)
#define AHA_FREERAM ESP.getFreeHeap()
#else
#define AHA_FREERAM 0
#endif

#define AHA_LEAKTRACKSTART \
    int freeRam = AHA_FREERAM;

#define AHA_LEAKTRACKEND \
    int diff = freeRam - AHA_FREERAM; \
    if (diff < 0) { diff *= -1; } \
    if (diff != 0) { \
        Serial.print(Test::getName().getFString()); \
        Serial.print(F(" memory leak: ")); \
        Serial.print(diff); \
        Serial.println(F("b")); \
        Test::fail(); \
    }
#else
// EpoxyDuino doesn't support memory tracking
#define AHA_LEAKTRACKSTART
#define AHA_LEAKTRACKEND
#endif

#define AHA_TEST(suiteName, name) \
class suiteName##_##name : public aunit::TestOnce { \
public: \
    suiteName##_##name(); \
    void once() override; \
    void loop() override { \
        AHA_LEAKTRACKSTART \
        once(); \
        if (isNotDone()) { pass(); } \
        AHA_LEAKTRACKEND \
    } \
} suiteName##_##name##_instance; \
suiteName##_##name :: suiteName##_##name() { \
  init(AUNIT_F(#suiteName "_" #name)); \
} \
void suiteName##_##name :: once()

#endif
#endif
