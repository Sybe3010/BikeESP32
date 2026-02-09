#pragma once

#include <Arduino.h>
#include "globalGuiDef.h"
#include "bluetooth.hpp"
#include "activity.hpp"
#include <chrono>

extern Bluetooth bleSensors;

extern std::string typeActivity;


std::string generateGpsFileName();
void ActivityStartEvent(lv_event_t *e);

void makeActivityHomePage();
void updateActivityHomePage(lv_event_t* e);

void actHomeSetRoute(lv_event_t* e);

void updateBLeTest(lv_timer_t * t);