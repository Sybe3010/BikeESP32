#include "bluetooth.hpp"

BLEScan* bleScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks{
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

Bluetooth::Bluetooth(bool mode){
    bluetoothMode = mode;
}

void Bluetooth::init(){
    if(bluetoothMode){

    } else{
        BLEDevice::init("BikeEsp");
        bleScan = BLEDevice::getScan();
        bleScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
        bleScan->setActiveScan(true);
        bleScan->setInterval(100);
        bleScan->setWindow(99);
    }
    
}
 
void Bluetooth::scanDevices(){
    BLEScanResults scanResults = bleScan->start(30, false);
    for(int i = 0; i < scanResults.getCount(); i++){
        FoundedDevice device = {0};
        device.deviceMacAddress = scanResults.getDevice(i).getAddress().toString();
        device.id = i;
        device.name = scanResults.getDevice(i).getName();
        device.serviceId = scanResults.getDevice(i).getServiceDataUUID().toString();
        devices.push_back(device);
    }
}