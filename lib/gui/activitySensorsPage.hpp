#pragma once

#include "bluetooth.hpp"
#include "globalGuiDef.h"

extern Bluetooth bleSensors;
extern BLEScan* bleScan;    

extern lv_obj_t *bleActivityScreen;

void makeBleActivityScreen();
void updateBleActivityScreen();

void bleActivityDeviceEvent(lv_event_t *e);