#ifndef AHA_HADEVICETRACKER_H
#define AHA_HADEVICETRACKER_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_DEVICE_TRACKER

/**
 * HADeviceTracker allows to implement a custom device's tracker.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/device_tracker.mqtt/
 */
class HADeviceTracker : public HABaseDeviceType
{
public:
    /// Available source types of the tracker.
    enum SourceType {
        SourceTypeUnknown = 0,
        SourceTypeGPS,
        SourceTypeRouter,
        SourceTypeBluetooth,
        SourceTypeBluetoothLE
    };

    /// Available states that can be reported to the HA panel.
    enum TrackerState {
        StateUnknown = 0,
        StateHome,
        StateNotHome,
        StateNotAvailable
    };

    /**
     * @param uniqueId The unique ID of the tracker. It needs to be unique in a scope of your device.
     */
    HADeviceTracker(const char* uniqueId);

    /**
     * Changes the state of the tracker and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state The new state of the tracker.
     * @param force Forces to update the state without comparing it to a previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const TrackerState state, const bool force = false);

    /**
     * Sets the current state of the tracker without publishing it to Home Assistant.
     * This method may be useful if you want to change the state before connection
     * with MQTT broker is acquired.
     *
     * @param state The new state of the tracker.
     */
    inline void setCurrentState(const TrackerState state)
        { _currentState = state; }

    /**
     * Returns the last known state of the tracker.
     * If setState method wasn't called the initial value will be returned.
     */
    inline TrackerState getState() const
        { return _currentState; }

    /**
     * Sets icon of the tracker.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets the source type of the tracker.
     *
     * @param type The source type (gps, router, bluetooth, bluetooth LE).
     */
    inline void setSourceType(const SourceType type)
        { _sourceType = type; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
   /**
     * Publishes the MQTT message with the given state.
     * 
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(TrackerState state);

    /**
     * Returns progmem string representing source type of the tracker.
     */
    const __FlashStringHelper* getSourceTypeProperty() const;

    /// The icon of the tracker. It can be nullptr.
    const char* _icon;

    /// The source type of the tracker. By default it's `HADeviceTracker::SourceTypeUnknown`.
    SourceType _sourceType;

    /// The current state of the device's tracker. By default its `HADeviceTracker::StateUnknown`.
    TrackerState _currentState;
};

#endif
#endif
