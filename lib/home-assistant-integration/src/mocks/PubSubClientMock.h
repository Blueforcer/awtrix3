#ifndef AHA_PUBSUBCLIENTMOCK_H
#define AHA_PUBSUBCLIENTMOCK_H

#ifdef ARDUINOHA_TEST

#include <Arduino.h>
#include <IPAddress.h>

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif

struct MqttMessage
{
    char* topic;
    size_t topicSize;
    char* buffer;
    size_t bufferSize;
    bool retained;

    MqttMessage() :
        topic(nullptr),
        topicSize(0),
        buffer(nullptr),
        bufferSize(0),
        retained(false)
    {

    }

    ~MqttMessage()
    {
        if (topic) {
            delete topic;
        }

        if (buffer) {
            delete buffer;
        }
    }
};

struct MqttSubscription {
    char* topic;

    MqttSubscription() :
        topic(nullptr)
    {

    }

    ~MqttSubscription()
    {
        if (topic) {
            delete topic;
        }
    }
};

struct MqttConnection
{
    bool connected;
    const char* domain;
    IPAddress ip;
    uint16_t port;
    const char* id;
    const char* user;
    const char* pass;

    MqttConnection() :
        connected(false),
        domain(nullptr),
        port(0),
        id(nullptr),
        user(nullptr),
        pass(nullptr)
    {

    }
};

struct MqttWill
{
    const char* topic;
    const char* message;
    bool retain;

    MqttWill() :
        topic(nullptr),
        message(nullptr),
        retain(false)
    {

    }
};

class PubSubClientMock
{
public:
    PubSubClientMock();
    ~PubSubClientMock();

    bool loop();
    void disconnect();
    bool connected();
    bool connect(
        const char *id,
        const char *user,
        const char *pass,
        const char* willTopic,
        uint8_t willQos,
        bool willRetain,
        const char* willMessage,
        bool cleanSession
    );
    bool connectDummy();
    PubSubClientMock& setServer(IPAddress ip, uint16_t port);
    PubSubClientMock& setServer(const char* domain, uint16_t port);
    PubSubClientMock& setCallback(MQTT_CALLBACK_SIGNATURE);

    bool beginPublish(const char* topic, unsigned int plength, bool retained);
    size_t write(const uint8_t *buffer, size_t size);
    size_t print(const __FlashStringHelper* buffer);
    int endPublish();
    bool subscribe(const char* topic);

    inline uint8_t getFlushedMessagesNb() const
        { return _flushedMessagesNb; }

    inline MqttMessage** getFlushedMessages() const
        { return _flushedMessages; }

    inline uint8_t getSubscriptionsNb() const
        { return _subscriptionsNb; }

    inline MqttSubscription** getSubscriptions() const
        { return _subscriptions; }      

    inline const MqttConnection& getConnection() const
        { return _connection; }

    inline const MqttWill& getLastWill() const
        { return _lastWill; }

    void clearFlushedMessages();
    void clearSubscriptions();
    void fakeMessage(const char* topic, const char* message);
    void fakeMessage(const __FlashStringHelper* topic, const char* message);
    void fakeMessage(const __FlashStringHelper* topic, const __FlashStringHelper* message);

private:
    MqttMessage* _pendingMessage;
    MqttMessage** _flushedMessages;
    uint8_t _flushedMessagesNb;
    MqttSubscription** _subscriptions;
    uint8_t _subscriptionsNb;
    MqttConnection _connection;
    MqttWill _lastWill;
    MQTT_CALLBACK_SIGNATURE;
};

#endif
#endif
