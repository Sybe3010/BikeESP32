#include "bluetooth.hpp"


/// Callbacks ///

/// @brief Callback voor de BLEScan 
/// @note Wordt aangeroepen bij elk gedetecteerd apparaat, voor dan voor elk apparaat de code in onResult uit.

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    private:
        Bluetooth* _bluetooth;
    public:
        MyAdvertisedDeviceCallbacks(Bluetooth* bluetooth) : _bluetooth(bluetooth) {}

        void onResult(BLEAdvertisedDevice advertisedDevice) override {
            Serial.print("Found device: ");
            Serial.println(advertisedDevice.toString().c_str());
        }
};

/// @brief callback voor de BLE Client
/// @note De code wordt uitgevoerd als de client connect of disconnect.
class MyClientCallback : public BLEClientCallbacks {
    private:
        Bluetooth* _bluetooth;
    public:
        MyClientCallback(Bluetooth* bluetooth) : _bluetooth(bluetooth) {}

        void onConnect(BLEClient* pClient) override {
            Serial.println("Connected to device!");
        }

        void onDisconnect(BLEClient* pClient) override {
            for(int i = 0; i < _bluetooth->clients.size(); i++){
                if(pClient == _bluetooth->clients[i]._client){
                    if (pClient->isConnected())
                    {
                        pClient->disconnect();
                    }
                    
                    delete pClient;

                    _bluetooth->clients.erase(_bluetooth->clients.begin() + i);
                    _bluetooth->hrValue = 0;
                }
            }
        }
};


/// Bluetooth class methods ///
Bluetooth::Bluetooth(){
    _BLEScan = nullptr;
    _Client = nullptr;
}


/// @brief Initialize the BLE device
/// @note Sets up the BLE device with a default name.
void Bluetooth::init(){
    BLEDevice::init("BikeEsp");
}

/// @brief Start scanning for BLE devices
/// @note Configures and starts the BLE scan with specified parameters.
void Bluetooth::startScan(){
    _BLEScan = BLEDevice::getScan();
    _BLEScan->setActiveScan(true);
    _BLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(this));
    _BLEScan->setInterval(100);
    _BLEScan->setWindow(99);
    _scanResults = _BLEScan->start(5, false);
}

/// @brief Stop scanning for BLE devices
/// @note Stops the ongoing BLE scan.
void Bluetooth::stopScan(){
    _BLEScan->stop();
    Serial.println("Scan stopped!");
}

/// @brief Connect to a BLE device using its advertised device information
/// @param advertisedDevice Pointer to the advertised device to connect to
/// @note Creates a BLE client and connects to the specified advertised device.
void Bluetooth::connectToDevice(BLEAdvertisedDevice* advertisedDevice){
    BleClients newClient;
    newClient._client = BLEDevice::createClient();
    newClient._client->setClientCallbacks(new MyClientCallback(this));
    newClient._client->connect(advertisedDevice);

    
    clients.push_back(newClient);
    auto &client = clients.back();

    // Heart rate notify
    if(auto hr = client._client->getService(HRServiceUUID)) {
        Serial.println("Service found!");
        client.hr = true;
        auto characteristic = hr->getCharacteristic(BLEUUID((uint16_t)0x2A37)); // Replace with your target characteristic UUID
        if(characteristic && characteristic->canNotify()){
            characteristic->registerForNotify([this](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                hrValue = pData[1];
            });
        }
    }

    // Speed Cadance Notify
    if(auto sc = client._client->getService(CSServiceUUID)) {
        Serial.println("Service found!");
        auto typeSensor = sc->getCharacteristic(BLEUUID((uint16_t)0x2A5C));
        if(typeSensor && typeSensor->canRead()){
            std::string typeSensorString = typeSensor->readValue();
            uint16_t feature = static_cast<uint16_t>(typeSensorString[0]);
            if(feature & 0x01){ // enkel snelheid
                client.speed = true;
            } else {
                client.speed = false;
            }

            if(feature & 0x02){ // enkel cadans
                client.cadance = true;
            } else {
                client.cadance = false;
            }

            if(feature & 0x04){ // snelheid en candans
                client.speed = true;
                client.cadance = false;
            } else {
                client.speed = false;
                client.cadance = false;
            }
        }

        auto sensorValue = sc->getCharacteristic(BLEUUID((uint16_t)0x2A5B));
        if(sensorValue && sensorValue->canNotify()){
            sensorValue->registerForNotify([](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                Serial.print("Notification received: "); // bit 0: wheel revolutions bit, bit 1: crank evolutions bit
                for(size_t i = 0; i < length; i++) {
                    Serial.print(pData[i]);
                    Serial.print(" ");
                }
                Serial.println();
            });
        }
    }

}

BLEScanResults Bluetooth::getScanResults(){
    return _scanResults;
}