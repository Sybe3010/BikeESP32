#pragma once

#include <vector>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEServer.h>

#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>

static BLEUUID HR_SERVICE_UUID("180D");
static BLEUUID HR_CHAR_UUID("2A37");

static BLEUUID CSC_SERVICE_UUID("1816");
static BLEUUID CSC_MEASUREMENT_UUID("2A5B");

static BLEUUID POWER_SERVICE_UUID("1818");
static BLEUUID POWER_MEASUREMENT_UUID("2A63");

struct ConnectedDevice{
    std::string name;
    std::string macAddress;
    BLEClient* client;
    std::string type;
};

class BluetoothSearch{
    private:
        bool bluetoothMode; /**<Gives what mode BLE is in: false: client, true: server */
    public:
        struct FoundedDevice{
            std::string name;
            std::string deviceMacAddress;
            BLEUUID serviceId;
        };

        BluetoothSearch();
        BluetoothSearch(bool mode);

        void init();
        void scanAvailableDevices();

        static bool hasAServiceUUID(BLEAdvertisedDevice &device, BLEUUID uuid);

        static std::vector<FoundedDevice> devices;
  
};

class BleSensor{
    private:
        BLEAdvertisedDevice* _advertisedDevice = nullptr;
        BLEAdvertisedDevice* sensor;
        std::string deviceMacAddress;
        BLEClient* sensorClient = nullptr;
        bool connected = false;
    public:
        BleSensor();
        BleSensor(std::string macAddress);
        BleSensor(BLEAdvertisedDevice* device);

        bool isConnected();
        BLEClient* getClient();

        void heartRateNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
        void cadanceNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
        void powerNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

        bool connectSensor();

        int heartRateBpm;
        int powerWatts;
        int cadenceRpm;
};