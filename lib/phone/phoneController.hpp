#pragma once

#include "BLEDefines.h"
#include "BLEDevice.h"
#include <WiFi.h>

/// @brief Regelt de connecties tussen de app en fietscomputer
/// en regelt de states van de connectie
class phoneController{
    public:
        phoneController();
        void init();

        void changeDeviceConnectedBool(bool newVal);
        void handleConnectionStateWrite(uint8_t newState);
        void handleSyncRequest();

        ConnectionState getConnectionState();
    private:
        ConnectionState _connectionState = IDLE;
        FileState _fileState = NO_FILESTATE;
        BikeCompState _bikeCompState = HOME;
        AccountInfoState _accState = NONE;

        BLEServer *Server;

        BLEService *deviceControllService;
        BLEService *fileTransferService;
        BLEService *deviceInfoService;
        BLEService *accountInfoService;

        bool deviceConnected = false;
        bool wifiActive = false;

        void startBle();
        void stopBle();
        void startWifi();
        void stopWifi();
        
        // Event handlers voor write callbacks
        
};