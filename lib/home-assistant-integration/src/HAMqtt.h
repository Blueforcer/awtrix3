#ifndef AHA_HAMQTT_H
#define AHA_HAMQTT_H

#include <Arduino.h>
#include <Client.h>
#include <IPAddress.h>
#include "ArduinoHADefines.h"

#define HAMQTT_CALLBACK(name) void (*name)()
#define HAMQTT_MESSAGE_CALLBACK(name) void (*name)(const char *topic, const uint8_t *payload, uint16_t length)
#define HAMQTT_DEFAULT_PORT 1883

#ifdef ARDUINOHA_TEST
class PubSubClientMock;
#else
class PubSubClient;
#endif

class HADevice;
class HABaseDeviceType;

#if defined(ARDUINO_API_VERSION)
using namespace arduino;
#endif

/**
 * This class is a wrapper for the PubSub API.
 * It's a central point of the library where instances of all device types are stored.
 */
class HAMqtt
{
public:
    /**
     * Returns existing instance (singleton) of the HAMqtt class.
     * It may be a null pointer if the HAMqtt object was never constructed or it was destroyed.
     */
    inline static HAMqtt *instance()
    {
        return _instance;
    }

#ifdef ARDUINOHA_TEST
    explicit HAMqtt(
        PubSubClientMock *pubSub,
        HADevice &device,
        const uint8_t maxDevicesTypesNb = 6);
#else
    /**
     * Creates a new instance of the HAMqtt class.
     * Please note that only one instance of the class can be initialized at the same time.
     *
     * @param netClient The EthernetClient or WiFiClient that's going to be used for the network communication.
     * @param device An instance of the HADevice class representing your device.
     * @param maxDevicesTypesNb The maximum number of device types (sensors, switches, etc.) that you're going to implement.
     */
    explicit HAMqtt(
        Client &netClient,
        HADevice &device,
        const uint8_t maxDevicesTypesNb = 6);
#endif

    /**
     * Removes singleton of the HAMqtt class.
     */
    ~HAMqtt();

    /**
     * Sets the prefix of the Home Assistant discovery topics.
     * It needs to match the prefix set in the HA admin panel.
     * The default prefix is "homeassistant".
     *
     * @param prefix The discovery topics' prefix.
     */
    inline void setDiscoveryPrefix(const char *prefix)
    {
        _discoveryPrefix = prefix;
    }

    /**
     * Returns the discovery topics' prefix.
     */
    inline const char *getDiscoveryPrefix() const
    {
        return _discoveryPrefix;
    }

    /**
     * Sets prefix of the data topics.
     * It may be useful if you want to pass MQTT traffic through a bridge.
     * The default prefix is "aha".
     *
     * @param prefix The data topics' prefix.
     */
    inline void setDataPrefix(const char *prefix)
    {
        _dataPrefix = prefix;
    }

    /**
     * Returns the data topics' prefix.
     */
    inline const char *getDataPrefix() const
    {
        return _dataPrefix;
    }

    /**
     * Returns instance of the device assigned to the HAMqtt class.
     * It's the same object (pointer) that was passed to the HAMqtt constructor.
     */
    inline HADevice const *getDevice() const
    {
        return &_device;
    }

    /**
     * Registers a new callback method that will be called when the device receives an MQTT message.
     * Please note that the callback is also fired by internal MQTT messages used by the library.
     * You should always verify the topic of the received message.
     *
     * @param callback Callback method.
     */
    inline void onMessage(HAMQTT_MESSAGE_CALLBACK(callback))
    {
        _messageCallback = callback;
    }

    /**
     * Registers a new callback method that will be called each time a connection to the MQTT broker is acquired.
     * The callback is also fired after reconnecting to the broker.
     * You can use this method to register topics' subscriptions.
     *
     * @param callback Callback method.
     */
    inline void onConnected(HAMQTT_CALLBACK(callback))
    {
        _connectedCallback = callback;
    }

    /**
     * Sets parameters of the MQTT connection using the IP address and port.
     * The library will try to connect to the broker in first loop cycle.
     * Please note that the library automatically reconnects to the broker if connection is lost.
     *
     * @param serverIp IP address of the MQTT broker.
     * @param serverPort Port of the MQTT broker.
     * @param username Username for authentication. It can be nullptr if the anonymous connection needs to be performed.
     * @param password Password for authentication. It can be nullptr if the anonymous connection needs to be performed.
     */
    bool begin(
        const IPAddress serverIp,
        const uint16_t serverPort = HAMQTT_DEFAULT_PORT,
        const char *username = nullptr,
        const char *password = nullptr,
        const char *clientID = nullptr);

    /**
     * Sets parameters of the MQTT connection using the IP address and the default port (1883).
     * The library will try to connect to the broker in first loop cycle.
     * Please note that the library automatically reconnects to the broker if connection is lost.
     *
     * @param serverIp IP address of the MQTT broker.
     * @param username Username for authentication. It can be nullptr if the anonymous connection needs to be performed.
     * @param password Password for authentication. It can be nullptr if the anonymous connection needs to be performed.
     */
    bool begin(
        const IPAddress serverIp,
        const char *username,
        const char *password,
        const char *clientID);

    /**
     * Sets parameters of the MQTT connection using the hostname and port.
     * The library will try to connect to the broker in first loop cycle.
     * Please note that the library automatically reconnects to the broker if connection is lost.
     *
     * @param serverHostname Hostname of the MQTT broker.
     * @param serverPort Port of the MQTT broker.
     * @param username Username for authentication. It can be nullptr if the anonymous connection needs to be performed.
     * @param password Password for authentication. It can be nullptr if the anonymous connection needs to be performed.
     */
    bool begin(
        const char *serverHostname,
        const uint16_t serverPort = HAMQTT_DEFAULT_PORT,
        const char *username = nullptr,
        const char *password = nullptr,
        const char *clientID = nullptr);

    /**
     * Sets parameters of the MQTT connection using the hostname and the default port (1883).
     * The library will try to connect to the broker in first loop cycle.
     * Please note that the library automatically reconnects to the broker if connection is lost.
     *
     * @param serverHostname Hostname of the MQTT broker.
     * @param username Username for authentication. It can be nullptr if the anonymous connection needs to be performed.
     * @param password Password for authentication. It can be nullptr if the anonymous connection needs to be performed.
     */
    bool begin(
        const char *serverHostname,
        const char *username,
        const char *password,
        const char *clientID);

    /**
     * Closes the MQTT connection.
     */
    bool disconnect();
    void disableHA();
    /**
     * This method should be called periodically inside the main loop of the firmware.
     * It's safe to call this method in some interval (like 5ms).
     */
    void loop();

    /**
     * Returns true if connection to the MQTT broker is established.
     */
    bool isConnected() const;

    /**
     * Adds a new device's type to the MQTT.
     * Each time the connection with MQTT broker is acquired, the HAMqtt class
     * calls "onMqttConnected" method in all devices' types instances.
     *
     * @note The HAMqtt class doesn't take ownership of the given pointer.
     * @param deviceType Instance of the device's type (HASwitch, HABinarySensor, etc.).
     */
    void addDeviceType(HABaseDeviceType *deviceType);

    /**
     * Publishes the MQTT message with given topic and payload.
     * Message won't be published if the connection with the MQTT broker is not established.
     * In this case method returns false.
     *
     * @param topic The topic to publish.
     * @param payload The payload to publish (it may be empty const char).
     * @param retained Specifies whether message should be retained.
     */
    bool publish(const char *topic, const char *payload, bool retained = false);

    /**
     * Begins publishing of a message with the given properties.
     * When this method returns true the payload can be written using HAMqtt::writePayload method.
     *
     * @param topic Topic of the published message.
     * @param payloadLength Length of the payload (bytes) that's going to be published.
     * @param retained Specifies whether the published message should be retained.
     */
    bool beginPublish(const char *topic, uint16_t payloadLength, bool retained = false);

    /**
     * Writes given string to the TCP stream.
     * Please note that before writing any data the HAMqtt::beginPublish method
     * needs to be called.
     *
     * @param data The string to publish.
     * @param length Length of the data (bytes).
     */
    void writePayload(const char *data, const uint16_t length);

    /**
     * Writes given data to the TCP stream.
     * Please note that before writing any data the HAMqtt::beginPublish method
     * needs to be called.
     *
     * @param data The data to publish.
     * @param length Length of the data (bytes).
     */
    void writePayload(const uint8_t *data, const uint16_t length);

    /**
     * Writes given progmem data to the TCP stream.
     * Please note that before writing any data the HAMqtt::beginPublish method
     * needs to be called.
     *
     * @param data Progmem data to publish.
     */
    void writePayload(const __FlashStringHelper *data);

    /**
     * Finishes publishing of a message.
     * After calling this method the message will be processed by the broker.
     */
    bool endPublish();

    /**
     * Subscribes to the given topic.
     * Whenever a new message is received the onMqttMessage callback in all
     * devices types is called.
     *
     * Please note that you need to subscribe topic each time the connection
     * with the broker is acquired.
     *
     * @param topic Topic to subscribe.
     */
    bool subscribe(const char *topic);

    /**
     * Enables the last will message that will be produced when the device disconnects from the broker.
     * If you want to change availability of the device in Home Assistant panel
     * please use enableLastWill() method from the HADevice class instead.
     *
     * @param lastWillTopic The topic to publish.
     * @param lastWillMessage The message (payload) to publish.
     * @param lastWillRetain Specifies whether the published message should be retained.
     */
    inline void setLastWill(
        const char *lastWillTopic,
        const char *lastWillMessage,
        bool lastWillRetain)
    {
        _lastWillTopic = lastWillTopic;
        _lastWillMessage = lastWillMessage;
        _lastWillRetain = lastWillRetain;
    }

    /**
     * Processes MQTT message received from the broker (subscription).
     *
     * @note Do not use this method on your own. It's only for the internal purpose.
     * @param topic Topic of the message.
     * @param payload Content of the message.
     * @param length Length of the message.
     */
    void processMessage(const char *topic, const uint8_t *payload, uint16_t length);

#ifdef ARDUINOHA_TEST
    inline uint8_t getDevicesTypesNb() const
    {
        return _devicesTypesNb;
    }

    inline HABaseDeviceType **getDevicesTypes() const
    {
        return _devicesTypes;
    }
#endif

private:
    /// Interval between MQTT reconnects (milliseconds).
    static const uint16_t ReconnectInterval = 5000;

    /// Living instance of the HAMqtt class. It can be nullptr.
    static HAMqtt *_instance;

    /**
     * Attempts to connect to the MQTT broker.
     * The method uses properties passed to the "begin" method.
     */
    void connectToServer();
    bool noHA = false;
    /**
     * This method is called each time the connection with MQTT broker is acquired.
     */
    void onConnectedLogic();

#ifdef ARDUINOHA_TEST
    PubSubClientMock *_mqtt;
#else
    /// Instance of the PubSubClient class. It's initialized in the constructor.
    PubSubClient *_mqtt;
#endif

    /// Instance of the HADevice passed to the constructor.
    const HADevice &_device;

    /// The callback method that will be called when an MQTT message is received.
    HAMQTT_MESSAGE_CALLBACK(_messageCallback);

    /// The callback method that will be called when the MQTT connection is acquired.
    HAMQTT_CALLBACK(_connectedCallback);

    /// Specifies whether the HAMqtt::begin method was ever called.
    bool _initialized;

    /// Teh discovery prefix that's used for the configuration messages.
    const char *_discoveryPrefix;

    /// The data prefix that's used for publishing data messages.
    const char *_dataPrefix;

    /// The username used for the authentication. It's set in the HAMqtt::begin method.
    const char *_username;

    /// The username used for the authentication. It's set in the HAMqtt::begin method.
    const char *_clientID;

    /// The password used for the authentication. It's set in the HAMqtt::begin method.
    const char *_password;

    /// Time of the last connection attemps (milliseconds since boot).
    uint32_t _lastConnectionAttemptAt;

    /// The amount of registered devices types.
    uint8_t _devicesTypesNb;

    /// The maximum amount of devices types that can be registered.
    uint8_t _maxDevicesTypesNb;

    /// Pointers of all registered devices types (array of pointers).
    HABaseDeviceType **_devicesTypes;

    /// The last will topic set by HAMqtt::setLastWill
    const char *_lastWillTopic;

    /// The last will message set by HAMqtt::setLastWill
    const char *_lastWillMessage;

    /// The last will retain set by HAMqtt::setLastWill
    bool _lastWillRetain;
};

#endif