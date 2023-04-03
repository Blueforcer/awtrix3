#include <BeaconScanner.h>
#include <NimBLEDevice.h>
#include <Ticker.h>
#include "MQTTManager.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <DisplayManager.h>

float triggerDistance = 0.6;
String room = "Büro";
std::vector<String> allowedIds;

Ticker scanTicker;
// The getter for the instantiated singleton instance
BeaconScanner_ &BeaconScanner_::getInstance()
{
    static BeaconScanner_ instance;
    return instance;
}

// Initialize the global shared instance
BeaconScanner_ &BeaconScanner = BeaconScanner.getInstance();

static const int scanTime = 5; // In Sekunden
NimBLEScan *pBLEScan;

uint32_t fnv1a_32(const String &input)
{
    const uint32_t FNV_PRIME = 0x01000193;
    const uint32_t FNV_OFFSET_BASIS = 0x811C9DC5;

    uint32_t hash = FNV_OFFSET_BASIS;

    for (unsigned int i = 0; i < input.length(); ++i)
    {
        hash ^= static_cast<uint8_t>(input[i]);
        hash *= FNV_PRIME;
    }

    return hash;
}

String getManufacturerName(uint16_t manufacturerID)
{
    switch (manufacturerID)
    {
    case 0x004C: // Apple, Inc.
        return "Apple";
    case 0x0118: // Google, Inc.
        return "Google";
    case 0x000F: // Microsoft Corporation
        return "Microsoft";
    case 0x0030: // Ericsson Technology Licensing
        return "Ericsson";
    case 0x0039: // Intel Corp.
        return "Intel";
    case 0x0059: // Samsung Electronics Co., Ltd.
        return "Samsung";
    case 0x000D: // Nokia Corporation
        return "Nokia";
    case 0x0100: // Broadcom Corporation
        return "Broadcom";
    case 0x0034: // Texas Instruments Inc.
        return "Texas Instruments";
    case 0x0075: // Sony Ericsson Mobile Communications AB
        return "Sony Ericsson";
    case 0x0089: // Panasonic Corporation
        return "Panasonic";
    default:
        return "Unknown";
    }
}

bool isIdAllowed(const char *id)
{
    if (allowedIds.size() == 0)
        return true;
    for (const String &allowedId : allowedIds)
    {
        if (strcmp(id, allowedId.c_str()) == 0)
        {
            return true;
        }
    }
    Serial.println("Not allowed");
    return false;
}

double calculateDistance(int rssi, int8_t txPower)
{
    if (rssi == 0)
    {
        return -1.0;
    }

    double pathLoss = txPower - rssi;
    // Path Loss Exponent (n) ist eine Umgebungsspezifische Konstante und liegt normalerweise zwischen 2 und 4.
    // 2 entspricht Freiraum, während 4 dichtere Umgebungen repräsentiert.
    double pathLossExponent = 3.0;

    return pow(10, pathLoss / (10 * pathLossExponent));
}

String toHexString(const std::string &input)
{
    String hexString = "";
    for (unsigned char c : input)
    {
        char hexChar[3];
        snprintf(hexChar, sizeof(hexChar), "%02X", c);
        hexString += hexChar;
    }
    return hexString;
}

bool isIBeacon(NimBLEAdvertisedDevice *advertisedDevice, uint16_t *major, uint16_t *minor, int8_t *txPower, String *uuid, String *manufacturerName)
{
    std::string payload = advertisedDevice->getManufacturerData();

    if (payload.size() == 25 && static_cast<uint8_t>(payload[0]) == 0x4C && static_cast<uint8_t>(payload[1]) == 0x00 && static_cast<uint8_t>(payload[2]) == 0x02 && static_cast<uint8_t>(payload[3]) == 0x15)
    {
        uint16_t manufacturerID = (static_cast<uint8_t>(payload[0]) << 8) | static_cast<uint8_t>(payload[1]);
        *uuid = toHexString(payload.substr(4, 16));
        *major = (static_cast<uint8_t>(payload[20]) << 8) | static_cast<uint8_t>(payload[21]);
        *minor = (static_cast<uint8_t>(payload[22]) << 8) | static_cast<uint8_t>(payload[23]);
        *txPower = static_cast<int8_t>(payload[24]);
        *manufacturerName = getManufacturerName(manufacturerID);
        return true;
    }
    return false;
}

bool isEddystoneUID(NimBLEAdvertisedDevice *advertisedDevice, String *namespaceID, String *instanceID)
{
    std::string serviceData = advertisedDevice->getServiceData();
    if (serviceData.size() >= 20 && static_cast<uint8_t>(serviceData[0]) == 0xAA && static_cast<uint8_t>(serviceData[1]) == 0xFE && static_cast<uint8_t>(serviceData[2]) == 0x00)
    {
        *namespaceID = toHexString(serviceData.substr(4, 10));
        *instanceID = toHexString(serviceData.substr(14, 6));
        return true;
    }

    return false;
}

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{
    void onResult(NimBLEAdvertisedDevice *advertisedDevice)
    {
        StaticJsonDocument<512> doc;
        uint16_t major;
        uint16_t minor;
        int8_t txPower;
        String uuid;
        String manufacturer;

        uint32_t hashedUUID = fnv1a_32(uuid);
        char compressedUUID[9];
        snprintf(compressedUUID, sizeof(compressedUUID), "%08X", hashedUUID);

        int rssi = advertisedDevice->getRSSI();
        doc["id"] = compressedUUID;
        doc["rssi"] = rssi;
        String topic;
        bool sendToMQTT = false;
        topic = String("rooms/") + room;
        if (isIBeacon(advertisedDevice, &major, &minor, &txPower, &uuid, &manufacturer))
        {
            double BeaconDistance = calculateDistance(rssi, txPower);

            doc["txPower"] = txPower;
            doc["distance"] = BeaconDistance;

            if (advertisedDevice->haveName())
            {
                String nameBLE = String(advertisedDevice->getName().c_str());
                doc["name"] = nameBLE;
            }
            Serial.print("iBeacon UUID: ");
            Serial.print(compressedUUID);
            Serial.print(" Distance: ");
            Serial.print(BeaconDistance);
            Serial.print(" Manufacturer: ");
            Serial.print(manufacturer);
            Serial.print(" TxPower: ");
            Serial.println(txPower);

            if (BeaconDistance < 0.2)
            {
                char jsonString[100];
                memset(jsonString, 0, sizeof(jsonString));
                std::snprintf(jsonString, sizeof(jsonString), "{\"text\":\"%s\",\"duration\":\"%d\",\"color\":%s\"}", compressedUUID, 4,"#00ff00");
                DisplayManager.generateNotification(jsonString);
                return;
            }

            if ((BeaconDistance <= triggerDistance) && isIdAllowed(compressedUUID))
            {
                char JSONmessageBuffer[512];
                serializeJson(doc, JSONmessageBuffer);
                MQTTManager.publish(topic.c_str(), JSONmessageBuffer);
            }
        }
    }
};

void BeaconScanner_::startScan()
{
    Serial.println(F("---------------"));
    Serial.println(F("Scan BLE"));
    Serial.println(F("---------------"));
    pBLEScan->start(scanTime, [](NimBLEScanResults results)
                    { scanTicker.once(scanTime, []()
                                      { BeaconScanner.startScan(); }); });
}

void printAllowedIds()
{
    Serial.println("Allowed IDs:");
    for (const String &allowedId : allowedIds)
    {
        Serial.println(allowedId);
    }
}

bool loadBeaconSettings()
{
    Serial.println("loadSettings");
    if (LittleFS.exists("/beacons.json"))
    {
        File file = LittleFS.open("/beacons.json", "r");
        DynamicJsonDocument doc(128);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            Serial.println(F("Failed to read beacon settings"));
            return false;
        }

        if (doc.containsKey("room"))
        {
            room = doc["room"].as<String>();
            Serial.println(room);
        }
        else
        {
            return false;
        }

        if (doc.containsKey("trigger_distance"))
        {
            triggerDistance = doc["trigger_distance"].as<float>();
            Serial.println(triggerDistance);
        }
        else
        {
            return false;
        }

        if (doc.containsKey("allowed_ids"))
        {
            JsonArray allowedIdsJsonArray = doc["allowed_ids"];
            for (const char *id : allowedIdsJsonArray)
            {
                allowedIds.push_back(String(id));
            }
            printAllowedIds();
        }

        return true;
        file.close();
    }
    else
    {
        return false;
    }
 }

void BeaconScanner_::setup()
{
    if (!loadBeaconSettings())
        return;
    NimBLEDevice::init("");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
    pBLEScan->setActiveScan(true);
    pBLEScan->setMaxResults(0);
    startScan();
}