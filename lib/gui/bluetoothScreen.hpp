#pragma once

#include "bluetooth.hpp"
#include "globalGuiDef.h"

extern BluetoothSearch bleSensors;

void makeBleScreen();
void updateBleScreen();

void bleDeviceEvent(lv_event_t *e);