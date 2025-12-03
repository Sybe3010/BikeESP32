#pragma once

#include "globalGuiDef.h"

#include "maps.hpp"

extern lv_obj_t *mapCanvas;
extern lv_obj_t *zoomInBtn;
extern lv_obj_t *zoomOutBtn;
extern lv_obj_t *backButtonMapsScr;
extern lv_obj_t * headingMapButton;


extern lv_timer_t *mapUpdateTimer;

void zoomOutMapScr(lv_event_t *e);
void zoomInMapScr(lv_event_t *e);
void backButtonMapScr(lv_event_t *e);
void headingMapButtonScr(lv_event_t *e);
void updateMap(lv_event_t *e);

void makeMapScreen();
static void updateScreen(lv_timer_t *timer);
static void updateMapCanvas();