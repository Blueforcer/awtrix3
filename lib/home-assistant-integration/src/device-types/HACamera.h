#ifndef AHA_HACAMERA_H
#define AHA_HACAMERA_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_CAMERA

/**
 * HACamera allows to display an image in the Home Assistant panel.
 * It can be used for publishing an image from the ESP32-Cam module or any other
 * module that's equipped with a camera. 
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/camera.mqtt/
 */
class HACamera : public HABaseDeviceType
{
public:
    enum ImageEncoding {
        EncodingBinary = 1,
        EncodingBase64
    };

    /**
     * @param uniqueId The unique ID of the camera. It needs to be unique in a scope of your device.
     */
    HACamera(const char* uniqueId);

    /**
     * Publishes MQTT message with the given image data as a message content.
     * It updates image displayed in the Home Assistant panel.
     *
     * @param data Image data (raw binary data or base64)
     * @param length The length of the data.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool publishImage(const uint8_t* data, const uint16_t length);

    /**
     * Sets encoding of the image content.
     * Bu default Home Assistant expects raw binary data (e.g. JPEG binary data).
     *
     * @param encoding The image's data encoding.
     */
    inline void setEncoding(const ImageEncoding encoding)
        { _encoding = encoding; }

    /**
     * Sets icon of the camera.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;

private:
    /**
     * Returns progmem string representing the encoding property.
     */
    const __FlashStringHelper* getEncodingProperty() const;

    /// The encoding of the image's data. By default it's `HACamera::EncodingBinary`.
    ImageEncoding _encoding;

    /// The icon of the camera. It can be nullptr.
    const char* _icon;
    
};

#endif
#endif
