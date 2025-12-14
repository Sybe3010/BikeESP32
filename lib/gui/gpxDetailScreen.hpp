#pragma once
#include <Arduino.h>
#include "globalGuiDef.h"

#include "gpx.hpp"

extern lv_obj_t *routePreviewCanvas;
extern lv_obj_t *gpxDetailsScreenBackButton;
extern lv_obj_t *routeLenghtBox;
extern lv_obj_t *routeAscentBox;
extern lv_obj_t *routeElevationProfileCanvas;
extern lv_obj_t *routeElevationChart;
extern lv_obj_t *routeStartButton;
extern lv_obj_t *moreDetailsButton;

extern lv_obj_t *routeLenghtLabel;
extern lv_obj_t *routeAscentLabel;

extern lv_chart_series_t * elevationSerie;



extern GpxParser details;

void makeGpxDetailsScreen();
void fillGpxDetailsScreen();
void drawElevationChartEvent(lv_event_t *e);
void add_faded_area(lv_event_t * e);
void gpxDetailsBackButtonEvent(lv_event_t *e);

