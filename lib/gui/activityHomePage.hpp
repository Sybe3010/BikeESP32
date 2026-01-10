#pragma once

#include <Arduino.h>
#include "globalGuiDef.h"
#include "bluetooth.hpp"

extern Bluetooth bleSensors;

extern std::string typeActivity;

void makeActivityHomePage();
void updateActivityHomePage(lv_event_t* e);

void updateBLeTest(lv_timer_t * t);