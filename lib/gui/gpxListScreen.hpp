#pragma once

#include "globalGuiDef.h"
#include "gpx.hpp"

extern GpxParser gpxList;

void gpxListEvent(lv_event_t* e);

void makeGpxListScreen();
void updateGpxListScreen();