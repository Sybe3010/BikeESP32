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

        void connectToSensor(BLEAdvertisedDevice &device);
        static std::vector<FoundedDevice> devices;
  
};

class BleSensor{
    private:
        BLEAdvertisedDevice* sensor;
        BLEClient* sensorClient;
        bool connected = false;
    public:
        BleSensor();
        BleSensor(BLEAdvertisedDevice* device);

        void heartRateNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
        void cadanceNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
        void powerNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);

        bool connectSensor();

        int heartRateBpm;
        int powerWatts;
        int cadenceRpm;
};