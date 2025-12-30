#pragma once

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEAdvertisedDevice.h>

static BLEUUID targetServiceUUID("180D"); // ServiceUUID van een hartslagmeter

class Bluetooth{
    private: 
        BLEScan *_BLEScan;  // pointer naar BLEScan
        BLEClient *_Client; // pointer voor de client

        BLEScanResults _scanResults; // resultaten van de BLE Scan
    public:
        BLEAdvertisedDevice *targetDevice; // pointer voor het device dat gebruikt wordt door de client

        
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