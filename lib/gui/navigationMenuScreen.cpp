#include "navigationMenuScreen.hpp"

lv_obj_t *navigationMenuScreen;
lv_obj_t *mapButton;
lv_obj_t *gpxListButton;
lv_obj_t *recieveButton;

// event handelers
void mapButtonEvent(lv_event_t *e) {
    lv_screen_load(mapScreen);
}
void gpxListButtonEvent(lv_event_t *e){
    lv_screen_load(gpxListScreen);
}

void navigationMenuScreenEvent(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_GESTURE){
        lv_dir_t direction =  lv_indev_get_gesture_dir(lv_indev_active());
        if(direction = LV_DIR_LEFT){
            lv_screen_load(homeScreen);
        }
    }
}

void makeNavigationMenuScreen() {
    navigationMenuScreen = lv_obj_create(NULL);
    lv_obj_clear_flag(navigationMenuScreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(navigationMenuScreen, navigationMenuScreenEvent, LV_EVENT_ALL, NULL);

    // buttons
    mapButton = lv_button_create(navigationMenuScreen);
    lv_obj_set_size(mapButton, 320, 66);
    lv_obj_set_pos(mapButton, 0, 88);
    lv_obj_t *mapLabel = lv_label_create(mapButton);
    lv_label_set_text(mapLabel, "Map");
    lv_obj_center(mapLabel);

    gpxListButton = lv_button_create(navigationMenuScreen);
    lv_obj_set_size(gpxListButton, 320, 66);
    lv_obj_set_pos(gpxListButton, 0, 162);
    lv_obj_t *gpxListLabel = lv_label_create(gpxListButton);
    lv_label_set_text(gpxListLabel, "GPX List");
    lv_obj_center(gpxListLabel);

    recieveButton = lv_button_create(navigationMenuScreen);
    lv_obj_set_size(recieveButton, 320, 66);
    lv_obj_set_pos(recieveButton, 0, 235);
    lv_obj_t *recieveLabel = lv_label_create(recieveButton);
    lv_label_set_text(recieveLabel, "Receive");
    lv_obj_center(recieveLabel);

    lv_obj_add_event_cb(mapButton, mapButtonEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(gpxListButton, gpxListButtonEvent, LV_EVENT_CLICKED, NULL);
}