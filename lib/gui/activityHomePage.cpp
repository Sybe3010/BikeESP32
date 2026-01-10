#include "activityHomePage.hpp"

lv_obj_t *activityHomeScreen;

std::string typeActivity;

lv_obj_t *activityHomeTitel;
lv_obj_t *activityHomeMainBox;
lv_obj_t *activityHomeRoute;
lv_obj_t *activityHomeSensors;
lv_obj_t *activityHomeOpties;


void makeActivityHomePage(){
    activityHomeScreen = lv_obj_create(NULL);

    /// Activity titel
    activityHomeTitel = lv_label_create(activityHomeScreen);
    lv_obj_set_pos(activityHomeTitel, 34, 32);
    lv_obj_set_style_text_font(activityHomeTitel, &lv_font_montserrat_28, 0);
    
    /// Activity main box
    activityHomeMainBox = lv_button_create(activityHomeScreen);
    lv_obj_set_pos(activityHomeMainBox, 32, 88);
    lv_obj_set_size(activityHomeMainBox, 256, 135);
    // Startpijl label
    lv_obj_t* startLabelActHome = lv_label_create(activityHomeMainBox);
    // Statuslabels in activitymainbox
    lv_obj_t* gpsStatusLabelActHome = lv_label_create(activityHomeMainBox);
    lv_obj_t* routeStatusLabelActHome = lv_label_create(activityHomeMainBox);
    lv_obj_t* hrStatusLabelActHome = lv_label_create(activityHomeMainBox);
    lv_obj_t* speedStatusLabelActHome = lv_label_create(activityHomeMainBox);
    lv_obj_t* cadanceStatusLabelActHome = lv_label_create(activityHomeMainBox);

    lv_obj_t *activityStartLabel = lv_label_create(activityHomeMainBox);
    lv_obj_set_pos(activityStartLabel, 14, 11);
    lv_obj_set_style_text_font(activityStartLabel, &lv_font_montserrat_26, 0);

    /// Routes button
    activityHomeRoute = lv_button_create(activityHomeScreen);
    lv_obj_set_size(activityHomeRoute, 256, 67);
    lv_obj_set_pos(activityHomeRoute, 32, 246);
    lv_obj_t *activityRouteLabel = lv_label_create(activityHomeRoute);
    lv_obj_set_pos(activityRouteLabel, 14, 11);
    lv_obj_set_style_text_font(activityRouteLabel, &lv_font_montserrat_26, 0);
    lv_label_set_text(activityRouteLabel, "Route");
 
    /// Sensors button
    activityHomeSensors = lv_button_create(activityHomeScreen);
    lv_obj_set_size(activityHomeSensors, 256, 67);
    lv_obj_set_pos(activityHomeSensors, 32, 325);
    lv_obj_t *activitySensorsLabel = lv_label_create(activityHomeSensors);
    lv_obj_set_pos(activitySensorsLabel, 14, 11);
    lv_obj_set_style_text_font(activitySensorsLabel, &lv_font_montserrat_26, 0);
    lv_label_set_text(activitySensorsLabel, "Sensors");

    /// Options button
    activityHomeOpties = lv_button_create(activityHomeScreen);
    lv_obj_set_size(activityHomeOpties, 256, 67);
    lv_obj_set_pos(activityHomeOpties, 32, 405);
    lv_obj_t *activityOptionsLabel = lv_label_create(activityHomeOpties);
    lv_obj_set_pos(activityOptionsLabel, 14, 11);
    lv_obj_set_style_text_font(activityOptionsLabel, &lv_font_montserrat_26, 0);
    lv_label_set_text(activityOptionsLabel, "Options");

    lv_obj_add_event_cb(activityHomeScreen, updateActivityHomePage, LV_EVENT_VALUE_CHANGED, NULL);

    lv_timer_t* testBLEtimer = lv_timer_create(updateBLeTest, 1000, NULL);
    lv_timer_ready(gpsTimer);

}

void updateBLeTest(lv_timer_t * t){
    lv_label_set_text(activityHomeTitel, String(bleSensors.hrValue).c_str());
}

void updateActivityHomePage(lv_event_t* e){
    lv_label_set_text(activityHomeTitel, typeActivity.c_str());
}