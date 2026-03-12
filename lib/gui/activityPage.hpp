#pragma once

#include "activityGlobal.h"
#include "globalGuiDef.h"
#include "activity.hpp"
#include "maps.hpp"
#include "bluetooth.hpp"
#include "gpx.hpp"

extern Activity* newActivity; 
extern Bluetooth bleSensors;

extern lv_obj_t *activityPageTileView;
extern lv_obj_t *activityPageMapTile;
extern lv_obj_t *activityPageDataTile; // geeft de huidige data weer
extern lv_obj_t *activityPageStatsTile; // geeft de statistieken weer: totale afstand, gemiddelde snelheid, max snelheid, totale klimming
extern lv_obj_t *activityPageClimbView; // geeft een grafiek weer van de beklimming

extern lv_obj_t *mapWidget;
extern lv_obj_t *mapDataWidget1;
extern lv_obj_t *mapDataWidget2;

extern lv_obj_t *dataWidget1;
extern lv_obj_t *dataWidget2;
extern lv_obj_t *dataWidget3;
extern lv_obj_t *dataWidget4;

extern lv_obj_t *statsWidget1;
extern lv_obj_t *statsWidget2;
extern lv_obj_t *statsWidget3;
extern lv_obj_t *statsWidget4;

extern lv_obj_t *climbViewWidget1;
extern lv_obj_t *climbViewWidget2;
extern lv_obj_t *climbGraph;

extern lv_obj_t *browseNextBtn;
extern lv_obj_t *browsePrevBtn;
extern lv_obj_t *actMenuBtn;

extern lv_obj_t *ActivityZoomInBtn;
extern lv_obj_t *ActivityZoomOutBtn;

extern lv_timer_t* activityUITimer;


extern std::string trackFileName;

// Create page function
void createActivityPage();
void createMapTile();
void createDataTile();
void createStatsTile();
void createClimbViewTile();
void createBrowseButtons();

void activityPageController(lv_event_t *e);
void activityUIUpdateTimer(lv_timer_t* timer);