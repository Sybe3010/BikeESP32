#pragma once

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>

static BLEUUID HRServiceUUID("180D"); // ServiceUUID van een hartslagmeter
static BLEUUID CSServiceUUID("1816"); // ServiceUUID van een snelheid/cadans sensor
static BLEUUID PWRServiceUUID("1818"); // ServiceUUID van een hartslagmeter

class Bluetooth{
    private: 
        BLEScan *_BLEScan;  // pointer naar BLEScan
        BLEClient *_Client; // pointer voor de client

        BLEScanResults _scanResults; // resultaten van de BLE Scan

        struct BleClients{
            BLEClient* _client;
            bool hr;
            bool cadance;
            bool speed;
            BLEAdvertisedDevice *targetDevice;

            uint32_t prevWheelRevs = 0;
            uint16_t prevWheelTime = 0;
            uint16_t prevCrankRevs = 0;
            uint16_t prevCrankTime = 0;

            double lastSpeedKmh = 0.0;
            double lastCadanceRpm = 0.0;

            double wheelCircumferenceMeters = 2.105;
        };
    public:
        BLEAdvertisedDevice *targetDevice; // pointer voor het device dat gebruikt wordt door de client
        std::vector<BleClients> clients;
        uint8_t hrValue;
        uint16_t cadanceValue;
        float speedValue;
        uint8_t* hrPtr = &hrValue;

        Bluetooth();
        void init(); // initialiseerd BLE
        void startScan(); // start de scan
        void stopScan(); // stop de scan, enkel indien nodig

        void connectToDevice(BLEAdvertisedDevice* advertisedDevice); // connecteer doormiddel van BLEAdvertisedDevice
        void connectToDevice(std::string deviceAddress); // connecteer doormiddel van MACAdress, nog niet geimplementeerd in cpp

        void disconnectFromDevice(BLEClient* pClient); // disconnect doormiddel van BLEAdvertisedDevice, nog niet geimplementeerd in cpp
        void disconnectFromDevice(std::string deviceAddress); // disconnect doormiddel van MACAdress, nog niet geimplementeerd in cpp

        BLEScanResults getScanResults(); // verkrijg de scanResultaten

        void printDeviceInfo(BLEAdvertisedDevice advertisedDevice); // print de device info, nog niet in cpp
        void printClientInfo(BLEClient* pClient); // print de clientinfo, nog niet in cpp
};