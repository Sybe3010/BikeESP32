#pragma once

#include <vector>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEServer.h>

class Bluetooth{
    private:
        bool bluetoothMode; /**<Gives what mode BLE is in: false: client, true: server */

        struct FoundedDevice{
            std::string name;
            uint16_t id;
            std::string deviceMacAddress;
            std::string serviceId;
        };
  
    public:
        Bluetooth();
        Bluetooth(bool mode);

        void init();
        void scanDevices();

        std::vector<FoundedDevice> devices;
  
};