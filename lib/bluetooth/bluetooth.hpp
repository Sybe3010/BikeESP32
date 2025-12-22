#pragma once

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>

static BLEUUID targetServiceUUID("180D");

class Bluetooth{
    private: 
        BLEScan *_BLEScan;
        BLEClient *_Client;

        BLEScanResults _scanResults;
    public:
        BLEAdvertisedDevice *targetDevice;

        
        Bluetooth();
        void init();
        void startScan();
        void stopScan();

        void connectToDevice(BLEAdvertisedDevice* advertisedDevice);
        void connectToDevice(std::string deviceAddress);

        void disconnectFromDevice(BLEClient* pClient);
        void disconnectFromDevice(std::string deviceAddress);

        BLEScanResults getScanResults();

        void printDeviceInfo(BLEAdvertisedDevice advertisedDevice);
        void printClientInfo(BLEClient* pClient);
};