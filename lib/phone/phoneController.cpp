#include "phoneController.hpp"


phoneController::phoneController(){

}

class MyServerCallbacks: public BLEServerCallbacks {
    private:
        phoneController* _phoneController;
    public:
        MyServerCallbacks(phoneController* bluetooth) : _phoneController(bluetooth) {}

        void onConnect(BLEServer* pServer) override {
            if (_phoneController) {
                _phoneController->changeDeviceConnectedBool(true);
            }
        }

        void onDisconnect(BLEServer* pServer) override {
            if (_phoneController) {
                _phoneController->changeDeviceConnectedBool(false);
            }
        }
};

class ConnectionStateCallbacks: public BLECharacteristicCallbacks {
    private:
        phoneController* _phoneController;
    public:
        ConnectionStateCallbacks(phoneController* bluetooth) : _phoneController(bluetooth) {}

        void onWrite(BLECharacteristic* pCharacteristic) override {
            if (_phoneController) {
                uint8_t* data = pCharacteristic->getData();
                if (data && pCharacteristic->getValue().length() > 0) {
                    _phoneController->handleConnectionStateWrite(data[0]);
                }
            }
        }
};
void phoneController::changeDeviceConnectedBool(bool newVal) {
    deviceConnected = newVal;
    if (deviceConnected) {
        _connectionState = BLE_CONNECTED;

        // Sync connectionState naar BLE characteristic
        uint8_t stateValue = static_cast<uint8_t>(_connectionState);
        connectionStateChar.setValue(&stateValue, 1);
        connectionStateChar.notify();
    } else {
        _connectionState = IDLE;
        // Stop WiFi als disconnect gebeurd
        if (wifiActive) {
            log_e("Device disconnected, stopping wifi...");
            stopWifi();
        }
    }
}

void phoneController::handleConnectionStateWrite(uint8_t newState) {
    ConnectionState requestedState = static_cast<ConnectionState>(newState);
    _connectionState = requestedState;
    
    log_e("Connection state write received: %d", newState);
    
    // Direct state change handling via callback (niet in update loop!)
    if (deviceConnected && _connectionState == IDLE) {
        _connectionState = BLE_CONNECTED;
    } else if (!deviceConnected && _connectionState == BLE_CONNECTED) {
        _connectionState = IDLE;
    }

    if(_connectionState == 1 && wifiActive){
        stopWifi();
    }

    if(_connectionState == SYNC_REQUESTED && !wifiActive) {
        startWifi();
    }
}


void phoneController::init(){
    Server = BLEDevice::createServer();
    Server->setCallbacks(new MyServerCallbacks(this));

    // make device control serice
    deviceControllService = Server->createService(deviceControllServiceUUID);

    // add device control charasteristics
    deviceControllService->addCharacteristic(&connectionStateChar);
    deviceControllService->addCharacteristic(&bikeCompStateChar);
    
    // Add write callbacks for event-driven behavior
    connectionStateChar.setCallbacks(new ConnectionStateCallbacks(this));

    // add device control descriptors
    connectionStateDisc.setValue("Gives the connectionstate of the BikeEsp bikecomputer");
    bikeCompStateDisc.setValue("gives the menu the bike computer is in.");
    connectionStateChar.addDescriptor(&connectionStateDisc);
    bikeCompStateChar.addDescriptor(&bikeCompStateDisc);


    //make device fileTransfer service
    fileTransferService = Server->createService(fileTransferServiceUUID);
    
    //add fileTransfer charasteristics
    fileTransferService->addCharacteristic(&fileStateChar);
    fileTransferService->addCharacteristic(&fileNameChar);
    fileTransferService->addCharacteristic(&transferControlChar);
    fileTransferService->addCharacteristic(&transferProgressChar);

    //add fileTransfer discriptors
    fileStateDisc.setValue("gives the file status");
    fileNameDisc.setValue("filename");
    transferControlDisc.setValue("gives the transfer status");
    transferProgressDisc.setValue("gives the transfer progress status");
    fileStateChar.addDescriptor(&fileStateDisc);
    fileNameChar.addDescriptor(&fileNameDisc);
    transferControlChar.addDescriptor(&transferControlDisc);
    transferProgressChar.addDescriptor(&transferProgressDisc);


    // make device info status
    deviceInfoService = Server->createService(deviceInfoServiceUUID);
    deviceInfoService->addCharacteristic(&sdCardChar);
    sdCardDisc.setValue("is sd-card available");
    sdCardChar.addDescriptor(&sdCardDisc);

    // make account info service
    accountInfoService = Server->createService(accountInfoServiceUUID);
    
    // add account info characteristics
    accountInfoService->addCharacteristic(&dataRequestTypeChar);
    accountInfoService->addCharacteristic(&totalKmsChar);
    accountInfoService->addCharacteristic(&totalRidesChar);
    accountInfoService->addCharacteristic(&lastRideChar);
    
    // add account info descriptors
    dataRequestTypeDisc.setValue("type of data request");
    totalKmsDisc.setValue("total kilometers ridden");
    totalRidesDisc.setValue("total number of rides");
    lastRideDisc.setValue("last ride date and time");
    dataRequestTypeChar.addDescriptor(&dataRequestTypeDisc);
    totalKmsChar.addDescriptor(&totalKmsDisc);
    totalRidesChar.addDescriptor(&totalRidesDisc);
    lastRideChar.addDescriptor(&lastRideDisc);

    // start all services
    deviceControllService->start();
    fileTransferService->start();
    deviceInfoService->start();
    accountInfoService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(deviceControllServiceUUID);
    pAdvertising->addServiceUUID(fileTransferServiceUUID);
    pAdvertising->addServiceUUID(deviceInfoServiceUUID);
    pAdvertising->addServiceUUID(accountInfoServiceUUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();
}

ConnectionState phoneController::getConnectionState() {
    return _connectionState;
}

void phoneController::startWifi() {        
    // Sync connectionState naar BLE characteristic
    _connectionState = WIFI_STARTING;

    uint8_t stateValue = static_cast<uint8_t>(_connectionState);
    connectionStateChar.setValue(&stateValue, 1);
    connectionStateChar.notify();


    // Zet WiFi in Access Point mode (AP mode)
    WiFi.mode(WIFI_AP);
    
    // Configureer het WiFi netwerk
    const char* ssid = "BikeESP32";        // Netwerknaam
    const char* password = "BikeESP32Pass"; // WiFi wachtwoord
    
    // Start Access Point
    WiFi.softAP(ssid, password);
    
    // Print IP adres
    IPAddress IP = WiFi.softAPIP();
    Serial.print("WiFi AP gestart!\nIP adres: ");
    Serial.println(IP);
    
    wifiActive = true;
    _connectionState = WIFI_READY;
    
    // Notificeer de app van de status change
    stateValue = static_cast<uint8_t>(_connectionState);
    connectionStateChar.setValue(&stateValue, 1);
    connectionStateChar.notify();
}

void phoneController::stopWifi() {
    if (!wifiActive) {
        return; // WiFi is niet actief
    }
    
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    
    wifiActive = false;
    _connectionState = BLE_CONNECTED;
}
