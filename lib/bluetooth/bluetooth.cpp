#include "bluetooth.hpp"

BLEScan* bleScan;
BLEAdvertisedDevice* heartRateDevice = nullptr;
BLERemoteCharacteristic* heartRateCharacteristic = nullptr;

std::vector<BluetoothSearch::FoundedDevice> BluetoothSearch::devices;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks{
    private:
        BluetoothSearch* _owner;
    public:
        MyAdvertisedDeviceCallbacks(BluetoothSearch* owner) : _owner(owner){}
        void onResult(BLEAdvertisedDevice advertisedDevice) override {
            // if(_owner->hasAServiceUUID(advertisedDevice, HR_SERVICE_UUID)
            //     || _owner->hasAServiceUUID(advertisedDevice, CSC_SERVICE_UUID)
            //     || _owner->hasAServiceUUID(advertisedDevice, POWER_SERVICE_UUID)){
                
            //     BluetoothSearch::FoundedDevice device;

            //     device.deviceMacAddress = advertisedDevice.getAddress().toString();
            //     device.serviceId = advertisedDevice.getServiceUUID();
            //     device.name = advertisedDevice.getName();

            //     _owner->devices.push_back(device);
            // }
            
            BluetoothSearch::FoundedDevice device;
            device.deviceMacAddress = advertisedDevice.getAddress().toString();
            device.serviceId = advertisedDevice.getServiceUUID();
            device.name = advertisedDevice.getName();

            _owner->devices.push_back(device);
        }
};

BluetoothSearch::BluetoothSearch() {
    bluetoothMode = false;
}

BluetoothSearch::BluetoothSearch(bool mode){
    bluetoothMode = mode;
}

bool BluetoothSearch::hasAServiceUUID(BLEAdvertisedDevice &device, BLEUUID uuid){
    return device.haveServiceUUID() && device.isAdvertisingService(uuid);
}

void BluetoothSearch::init(){
    BLEDevice::init("BikeEsp_Bike_Computer");
    bleScan = BLEDevice::getScan();
    bleScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(this));
    bleScan->setActiveScan(true); 

}
 
void BluetoothSearch::scanAvailableDevices(){
    Serial.println("[BLE] Starting scan...");
    devices.clear();
    BLEScanResults scanResults = bleScan->start(5, false);
    Serial.printf("[BLE] Scan finished. Found %d devices\n", BluetoothSearch::devices.size());
}



BleSensor::BleSensor(BLEAdvertisedDevice* device){
    sensor = device;
}

bool BleSensor::connectSensor(){
    sensorClient = BLEDevice::createClient();

    if(!sensorClient->connect(sensor)){
        return false;
    }

    if(auto hr = sensorClient->getService(HR_SERVICE_UUID)){
        auto ch = hr->getCharacteristic(HR_CHAR_UUID);
        if (ch && ch->canNotify()) ch->registerForNotify([this](BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify){
            heartRateNotify(pBLERemoteCharacteristic, pData, length, isNotify);
        });
    }

    if (auto csc = sensorClient->getService(CSC_SERVICE_UUID)) {
        auto ch = csc->getCharacteristic(CSC_MEASUREMENT_UUID);
        if (ch && ch->canNotify()) ch->registerForNotify([this](BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify){
            cadanceNotify(pBLERemoteCharacteristic, pData, length, isNotify);
        });
    }


    if (auto pwr = sensorClient->getService(POWER_SERVICE_UUID)) {
        auto ch = pwr->getCharacteristic(POWER_MEASUREMENT_UUID);
        if (ch && ch->canNotify()) ch->registerForNotify([this](BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify){
            powerNotify(pBLERemoteCharacteristic, pData, length, isNotify);
        });
    }

    connected = true;

    return true;
}

void BleSensor::heartRateNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify){
    if (length > 1) {
        heartRateBpm = pData[1];
        Serial.printf("HR: %d BPM\n", heartRateBpm);
    }
}
void BleSensor::cadanceNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
    /*
    * Flags:
    * bit0 = wheel revolution data present
    * bit1 = crank revolution data present
    */
    uint8_t flags = pData[0];
    int index = 1;


    if (flags & 0x02) { // crank data
        uint16_t crankRevs = pData[index] | (pData[index + 1] << 8);
        uint16_t crankTime = pData[index + 2] | (pData[index + 3] << 8);
        index += 4;


        static uint16_t lastCrankRevs = 0;
        static uint16_t lastCrankTime = 0;


        uint16_t deltaRevs = crankRevs - lastCrankRevs;
        uint16_t deltaTime = crankTime - lastCrankTime;


        if (deltaTime > 0) {
        cadenceRpm = (deltaRevs * 60 * 1024) / deltaTime;
        Serial.printf("Cadans: %d RPM\n", cadenceRpm);
    }


    lastCrankRevs = crankRevs;
    lastCrankTime = crankTime;
    }
}


void BleSensor::powerNotify(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
    if (length >= 4) {
        powerWatts = pData[2] | (pData[3] << 8);
        Serial.printf("Vermogen: %d W\n", powerWatts);
    }
}