#pragma once

#include <lvgl.h>

extern lv_display_t *display;

// Home Screen object and tiles
extern lv_obj_t *homeScreen;
extern lv_obj_t *homeScreenTiles;

// Navigation Menu objects
extern lv_obj_t *navigationMenuScreen;
extern lv_obj_t *statusBar;
extern lv_obj_t *mapScreen;

//gpx objects
extern lv_obj_t *gpxListScreen;
extern lv_obj_t *gpxDetailsScreen;

// ble Objects
extern lv_obj_t *bleScreen;

// timer voor gps updates
extern lv_timer_t *gpsTimer;