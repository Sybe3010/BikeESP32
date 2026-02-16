#pragma once

#include "bluetooth.hpp"
#include "globalGuiDef.h"

extern Bluetooth bleSensors;
extern BLEScan* bleScan;    



void makeBleScreen();
void updateBleScreen();

void bleDeviceEvent(lv_event_t *e);