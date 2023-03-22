#ifndef AHA_HALOCK_H
#define AHA_HALOCK_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_LOCK

#define HALOCK_CALLBACK(name) void (*name)(LockCommand command, HALock* sender)

/**
 * HALock allows to implement a custom lock (for example: door lock)
 * that can be controlled from the Home Assistant panel.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/lock.mqtt/
 */
class HALock : public HABaseDeviceType
{
public:
    /// Available states of the lock that can be reported to the HA panel.
    enum LockState {
        StateUnknown = 0,
        StateLocked,
        StateUnlocked
    };

    /// Commands that will be produced by the HA panel.
    enum LockCommand {
        CommandLock = 1,
        CommandUnlock,
        CommandOpen
    };

    /**
     * @param uniqueId The unique ID of the lock. It needs to be unique in a scope of your device.
     */
    HALock(const char* uniqueId);

    /**
     * Changes state of the lock and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the lock.
     * @param force Forces to update state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const LockState state, const bool force = false);

    /**
     * Sets current state of the lock without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the lock.
     */
    inline void setCurrentState(const LockState state)
        { _currentState = state; }

    /**
     * Returns last known state of the lock.
     * If setState method wasn't called the initial value will be returned.
     */
    inline LockState getCurrentState() const
        { return _currentState; }

    /**
     * Sets icon of the lock.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the lock's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the lock state.
     * In this mode the lock state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Registers callback that will be called each time the lock/unlock/open command from the HA is received.
     * Please note that it's not possible to register multiple callbacks for the same lock.
     *
     * @param callback
     */
    inline void onCommand(HALOCK_CALLBACK(callback))
        { _commandCallback = callback; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    /**
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(const LockState state);

    /**
     * Parses the given command and executes the lock's callback with proper enum's property.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleCommand(const uint8_t* cmd, const uint16_t length);

    /// The icon of the lock. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the lock (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// The current state of the lock. By default it's `HALock::StateUnknown`.
    LockState _currentState;

    /// The callback that will be called when lock/unlock/open command is received from the HA.
    HALOCK_CALLBACK(_commandCallback);
};

#endif
#endif
