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
            Serial.println("Disconnected from device!");
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

    // Heart rate notify
    if(auto hr = newClient._client->getService(HRServiceUUID)) {
        Serial.println("Service found!");
        newClient.hr = true;
        auto characteristic = hr->getCharacteristic(BLEUUID((uint16_t)0x2A37)); // Replace with your target characteristic UUID
        if(characteristic && characteristic->canNotify()){
            characteristic->registerForNotify([this](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                hrValue = pData[1];
            });
        }
    }

    // Speed Cadance Notify
    if(auto sc = newClient._client->getService(CSServiceUUID)) {
        Serial.println("Service found!");
        auto typeSensor = sc->getCharacteristic(BLEUUID((uint16_t)0x2A5C));
        if(typeSensor && typeSensor->canRead()){
            std::string typeSensorString = typeSensor->readValue();

            if(typeSensorString[0] == 1){
                newClient.speed = true;
            } else {
                newClient.speed = false;
            }

            if(typeSensorString[1] == 1){
                newClient.cadance = true;
            } else {
                newClient.cadance = false;
            }

            if(typeSensorString[2] == 1){
                newClient.speed = true;
                newClient.cadance = false;
            } else {
                newClient.speed = false;
                newClient.cadance = false;
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

    clients.push_back(newClient);
}

BLEScanResults Bluetooth::getScanResults(){
    return _scanResults;
}