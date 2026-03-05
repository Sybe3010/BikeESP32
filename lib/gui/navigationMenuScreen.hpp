#pragma once

#include "globalGuiDef.h"

extern lv_obj_t *mapButton;
extern lv_obj_t *gpxListButton;
extern lv_obj_t *recieveButton;

void mapButtonEvent(lv_event_t *e);
void gpxListButtonEvent(lv_event_t *e);

void makeNavigationMenuScreen();    
void navigationMenuScreenEvent(lv_event_t *e); 