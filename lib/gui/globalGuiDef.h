#pragma once

#include <lvgl.h>

extern lv_display_t *display;

// Home Screen object and tiles
extern lv_obj_t *homeScreen;
extern lv_obj_t *homeScreenTiles;

// Navigation Menu object
extern lv_obj_t *navigationMenuScreen;
extern lv_obj_t *statusBar;
extern lv_obj_t *mapScreen;

extern lv_obj_t *gpxListScreen;

extern lv_obj_t *gpxDetailsScreen;

extern lv_obj_t *bleScreen;


extern lv_timer_t *gpsTimer;