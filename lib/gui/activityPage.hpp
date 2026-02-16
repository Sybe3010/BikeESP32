// ojbect waar alle ActivityDataPages samenkomen en worden getoond.
#pragma once

#include "activityDataPage.hpp"
#include "globalGuiDef.h"
#include "activity.hpp"

extern Activity* newActivity;

void createActivityPage();
void updateScreen(lv_event_t *e);
void updateScreenData(lv_timer_t *t);

void leftButtonEventHandler(lv_event_t* e);
void rightButtonEventHandler(lv_event_t* e);