#pragma once

#include "bluetooth.hpp"
#include "globalGuiDef.h"

extern BluetoothSearch bleSensors;
extern BLEScan* bleScan;    

extern std::vector<ConnectedDevice> connectedDevices;

void makeBleScreen();
void updateBleScreen();

void bleDeviceEvent(lv_event_t *e);