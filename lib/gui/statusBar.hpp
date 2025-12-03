#pragma once

#include "globalGuiDef.h"

#include "gps.hpp"
#include "storage.hpp"

extern lv_obj_t *latLabel;
extern lv_obj_t *lonLabel;

extern lv_obj_t *sattLabel;
extern lv_obj_t *sdCardIcon;

void latLabelChange(const char* latText);
void lonLabelChange(const char* lonText);
void sattLabelChange(const char* sattText);

void makeStatusBar();

void updateGPSTask(lv_timer_t * timer);

void sdLabelChange(bool sdCardAvailable);