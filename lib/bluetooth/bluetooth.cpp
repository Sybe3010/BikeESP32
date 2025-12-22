#include "bluetooth.hpp"


/// Callbacks ///
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    private:
        Bluetooth* _bluetooth;
    public:
        MyAdvertisedDeviceCallbacks(Bluetooth* bluetooth) : _bluetooth(bluetooth) {}

        void onResult(BLEAdvertisedDevice advertisedDevice) override {
            Serial.print("Found device: ");
            Serial.println(advertisedDevice.toString().c_str());
            if(advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(targetServiceUUID)) { // Replace with your target device name
                //_bluetooth->targetDevice = new BLEAdvertisedDevice(advertisedDevice);
                Serial.println("Target device found!");
            }
        }
};

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
    _Client = BLEDevice::createClient();
    _Client->setClientCallbacks(new MyClientCallback(this));
    _Client->connect(advertisedDevice);


    // Heart rate notify
    if(auto hr = _Client->getService(targetServiceUUID)) {
        Serial.println("Service found!");
        auto characteristic = hr->getCharacteristic(BLEUUID((uint16_t)0x2A37)); // Replace with your target characteristic UUID
        if(characteristic && characteristic->canNotify()){
            characteristic->registerForNotify([](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                Serial.print("Notification received: ");
                for(size_t i = 0; i < length; i++) {
                    Serial.print(pData[i]);
                    Serial.print(" ");
                }
                Serial.println();
            });
        }
    } else {
        Serial.println("Service not found!");
    }
}

BLEScanResults Bluetooth::getScanResults(){
    return _scanResults;
}