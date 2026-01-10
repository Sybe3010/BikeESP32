#include "bluetooth.hpp"

// deltaWithWrap: veilig verschil met wrap-around voor unsigned types
template<typename T>
static inline uint64_t deltaWithWrap(T newVal, T oldVal) {
    if (newVal >= oldVal) return static_cast<uint64_t>(newVal - oldVal);
    return static_cast<uint64_t>(static_cast<uint64_t>(std::numeric_limits<T>::max()) + 1 + newVal - oldVal);
}

static inline double deltaTimeSeconds(uint16_t newTicks, uint16_t oldTicks) {
    uint64_t dtTicks = deltaWithWrap<uint16_t>(newTicks, oldTicks);
    return static_cast<double>(dtTicks) / 1024.0;
}

double calculateCadenceRPM(uint16_t newCrankRevs, uint16_t oldCrankRevs,
                           uint16_t newCrankTime, uint16_t oldCrankTime) {
    uint64_t dRevs = deltaWithWrap<uint16_t>(newCrankRevs, oldCrankRevs);
    double dt = deltaTimeSeconds(newCrankTime, oldCrankTime);
    if (dt <= 0.0 || dRevs == 0) return 0.0;
    return (static_cast<double>(dRevs) / dt) * 60.0;
}

double calculateSpeedKmh(uint32_t newWheelRevs, uint32_t oldWheelRevs,
                         uint16_t newWheelTime, uint16_t oldWheelTime,
                         double wheelCircumferenceMeters) {
    uint64_t dRevs = deltaWithWrap<uint32_t>(newWheelRevs, oldWheelRevs);
    double dt = deltaTimeSeconds(newWheelTime, oldWheelTime);
    if (dt <= 0.0 || dRevs == 0 || wheelCircumferenceMeters <= 0.0) return 0.0;
    double metersPerSec = (static_cast<double>(dRevs) * wheelCircumferenceMeters) / dt;
    return metersPerSec * 3.6;
}


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
            sensorValue->registerForNotify([this, &client](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                uint8_t flags = pData[0];
                size_t offset = 1;

                uint32_t newWheelRevs = client.prevWheelRevs;
                uint16_t newWheelTime = client.prevWheelTime;
                uint16_t newCrankRevs = client.prevCrankRevs;
                uint16_t newCrankTime = client.prevCrankTime;

                if(flags & 0x01){
                    newWheelRevs = (uint32_t)pData[offset] | ((uint32_t)pData[offset+1] << 8) | ((uint32_t)pData[offset+2] << 16) | ((uint32_t)pData[offset+3] << 24);
                    newWheelTime = (uint16_t)pData[offset+4] | ((uint16_t)pData[offset+5] << 8); offset += 6;
                }

                if(flags & 0x02){
                    newCrankRevs = (uint16_t)pData[offset] | ((uint16_t)pData[offset+1] << 8); 
                    newCrankTime = (uint16_t)pData[offset+2] | ((uint16_t)pData[offset+3] << 8);
                }

                if(flags & 0x01){
                    double speedKmh = calculateSpeedKmh(newWheelRevs, client.prevWheelRevs, newWheelTime, client.prevWheelTime, client.wheelCircumferenceMeters);
                    client.lastSpeedKmh = speedKmh;
                    client.prevWheelRevs = newWheelRevs;
                    client.prevWheelTime = newWheelTime;
                }

                if(flags & 0x02){
                    double cadanceRpm = calculateCadenceRPM(newCrankRevs, client.prevCrankRevs, newCrankTime, client.prevCrankTime);
                    client.lastCadanceRpm = cadanceRpm;
                    client.prevCrankRevs = newCrankRevs;
                    client.prevCrankTime = newCrankTime;
                }

                if(flags & 0x01)
                { 
                    Serial.print(" Speed(km/h): "); 
                    Serial.print(client.lastSpeedKmh); 
                }
                
                if(flags & 0x02)
                { 
                    Serial.print(" Cadence(RPM): "); 
                    Serial.print(client.lastCadanceRpm); 
                }
            });
        }
    }

}

BLEScanResults Bluetooth::getScanResults(){
    return _scanResults;
}