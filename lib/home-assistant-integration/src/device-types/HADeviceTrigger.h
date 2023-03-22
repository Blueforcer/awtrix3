#ifndef AHA_HADEVICETRIGGER_H
#define AHA_HADEVICETRIGGER_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_DEVICE_TRIGGER

/**
 * HADeviceTrigger allows to a custom trigger that can be used in the Home Assistant automation.
 * For example, it can be a wall switch that produces `press` and `long_press` actions.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/device_trigger.mqtt/
 */
class HADeviceTrigger : public HABaseDeviceType
{
public:
    /// Built-in types of the trigger.
    enum TriggerType {
        ButtonShortPressType = 1,
        ButtonShortReleaseType,
        ButtonLongPressType,
        ButtonLongReleaseType,
        ButtonDoublePressType,
        ButtonTriplePressType,
        ButtonQuadruplePressType,
        ButtonQuintuplePressType
    };

    /// Built-in subtypes of the trigger.
    enum TriggerSubtype {
        TurnOnSubtype = 1,
        TurnOffSubtype,
        Button1Subtype,
        Button2Subtype,
        Button3Subtype,
        Button4Subtype,
        Button5Subtype,
        Button6Subtype
    };

    /**
     * Creates the device trigger with a custom type and subtype.
     * For example, it can be `click` as the type and `btn0` as the subtype.
     * Please note that combination of the type and subtype needs to be unique in a scope of your device.
     *
     * @param type String representation of the type.
     * @param subtype String representation of the subtype.
     */
    HADeviceTrigger(const char* type, const char* subtype);

    /**
     * Creates the device trigger with a built-in type and a custom subtype.
     * For example, it can be `HADeviceTrigger::ButtonShortPressType` as the type and `btn0` as the subtype.
     * Please note that combination of the type and subtype needs to be unique in a scope of your device.
     *
     * @param type Built-in type of the trigger.
     * @param subtype String representation of the subtype.
     */
    HADeviceTrigger(TriggerType type, const char* subtype);

    /**
     * Creates the device trigger with a custom type and a built-in subtype.
     * For example, it can be `click` as the type and `HADeviceTrigger::Button1Subtype` as the subtype.
     * Please note that combination of the type and subtype needs to be unique in a scope of your device.
     *
     * @param type String representation of the subtype.
     * @param subtype Built-in subtype of the trigger.
     */
    HADeviceTrigger(const char* type, TriggerSubtype subtype);

    /**
     * Creates the device trigger with a built-in type and built-in subtype.
     * For example, it can be `HADeviceTrigger::ButtonShortPressType` as the type and `HADeviceTrigger::Button1Subtype` as the subtype.
     * Please note that combination of the type and subtype needs to be unique in a scope of your device.
     *
     * @param type Built-in type of the trigger.
     * @param subtype Built-in subtype of the trigger.
     */
    HADeviceTrigger(TriggerType type, TriggerSubtype subtype);

    /**
     * Frees memory allocated by the class.
     */
    ~HADeviceTrigger();

    /**
     * Publishes MQTT message with the trigger event.
     * The published message is not retained.
     *
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool trigger();

    /**
     * Returns the type of the trigger.
     * If the built-in type is used the returned value points to the flash memory.
     * Use `HADeviceTrigger::isProgmemType` to verify if the returned value is the progmem pointer.
     *
     * @returns Pointer to the type.
     */
    inline const char* getType() const
        { return _type; }

    /**
     * Returns `true` if the built-in type was assigned to the trigger.
     */
    inline bool isProgmemType() const
        { return _isProgmemType; }

    /**
     * Returns the subtype of the trigger.
     * If the built-in subtype is used the returned value points to the flash memory.
     * Use `HADeviceTrigger::isProgmemSubtype` to verify if the returned value is the progmem pointer.
     *
     * @returns Pointer to the subtype.
     */
    inline const char* getSubtype() const
        { return _subtype; }

    /**
     * Returns `true` if the built-in subtype was assigned to the trigger.
     */
    inline bool isProgmemSubtype() const
        { return _isProgmemSubtype; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    /**
     * Calculates desired size of the unique ID based on the type and subtype that were passed to the constructor.
     */
    uint16_t calculateIdSize() const;

    /**
     * Builds the unique ID of the device's type based on the type and subtype that were passed to the constructor.
     */
    void buildUniqueId();

    /**
     * Returns pointer to the flash memory that represents the given type.
     * 
     * @param subtype Built-in type enum's value.
     * @returns Pointer to the flash memory if the given type is supported.
     *          For the unsupported type the nullptr is returned.
     */
    const char* determineProgmemType(TriggerType type) const;

    /**
     * Returns pointer to the flash memory that represents the given subtype.
     * 
     * @param subtype Built-in subtype enum's value.
     * @returns Pointer to the flash memory if the given subtype is supported.
     *          For the unsupported subtype the nullptr is returned.
     */
    const char* determineProgmemSubtype(TriggerSubtype subtype) const;

private:
    /// Pointer to the trigger's type. It can be pointer to the flash memory.
    const char* _type;

    /// Pointer to the trigger's subtype. It can be pointer to the flash memory.
    const char* _subtype;
    
    /// Specifies whether the type points to the flash memory.
    bool _isProgmemType;
    
    /// Specifies whether the subtype points to the flash memory.
    bool _isProgmemSubtype;
};

#endif
#endif
