#pragma once

#include "globalGuiDef.h"
#include "gpxDetailScreen.hpp"

extern String gpxFileFolder;
extern std::string trackFileName;

void gpxListEvent(lv_event_t* e);
// void gxpListScreenBackSwipeEvent(lv_event_t *e);



void makeGpxListScreen();
void viewOptions();
void updateGpxListScreen();