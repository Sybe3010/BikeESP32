#include "mapScreen.hpp"

Maps maps = Maps();

lv_obj_t * mapScreen;
lv_obj_t * mapCanvas;
lv_obj_t * zoomInBtn;
lv_obj_t * zoomOutBtn;
lv_obj_t * backButtonMapsScr;
lv_obj_t * headingMapButton;

lv_timer_t *mapUpdateTimer;

int zoom = 15;

void zoomOutMapScr(lv_event_t *e){
    if(zoom > 1){
        zoom--;
    }
    updateMapCanvas();
}
void zoomInMapScr(lv_event_t *e){
    if(zoom < 17){
        zoom++;
    }
    updateMapCanvas();
}

void backButtonMapScr(lv_event_t *e){
    lv_screen_load(homeScreen);
}

void headingMapButtonScr(lv_event_t *e){
    maps.turnOnGpsHeading = !maps.turnOnGpsHeading;
    updateMapCanvas();
}

void updateMap(lv_event_t *e){
    updateMapCanvas();
}


void makeMapScreen() {
    // Initialize maps system
    maps.initMap(320, 455);  // Match canvas size
    maps.createMapScrSprites();

    mapScreen = lv_obj_create(NULL);
    lv_obj_set_size(mapScreen, 320, 455);  

    backButtonMapsScr = lv_button_create(mapScreen);
    lv_obj_set_size(backButtonMapsScr, 50, 50);
    lv_obj_set_pos(backButtonMapsScr, 10, 10);
    lv_obj_add_event_cb(backButtonMapsScr,backButtonMapScr, LV_EVENT_CLICKED, NULL);
    lv_obj_t* backLabel = lv_label_create(backButtonMapsScr);
    lv_label_set_text(backLabel, "<");
    lv_obj_set_style_text_font(backLabel, &lv_font_montserrat_28, 0);

    zoomInBtn = lv_button_create(mapScreen);
    lv_obj_set_size(zoomInBtn, 50, 50);
    lv_obj_set_pos(zoomInBtn, 265, 340);
    lv_obj_t* zoomInLabel = lv_label_create(zoomInBtn);
    lv_label_set_text(zoomInLabel, "+");
    lv_obj_add_event_cb(zoomInBtn, zoomInMapScr, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_text_font(zoomInLabel, &lv_font_montserrat_28, 0);

    zoomOutBtn = lv_button_create(mapScreen);
    lv_obj_set_size(zoomOutBtn, 50, 50);
    lv_obj_set_pos(zoomOutBtn, 265, 400);
    lv_obj_t* zoomOutLabel = lv_label_create(zoomOutBtn);
    lv_label_set_text(zoomOutLabel, "-");
    lv_obj_add_event_cb(zoomOutBtn, zoomOutMapScr, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_text_font(zoomOutLabel, &lv_font_montserrat_28, 0);

    headingMapButton = lv_button_create(mapScreen);
    lv_obj_set_size(headingMapButton, 50, 50);
    lv_obj_set_pos(headingMapButton, 10, 400);
    lv_obj_t* headingLabel = lv_label_create(headingMapButton);
    lv_label_set_text(headingLabel, "H");
    lv_obj_set_style_text_font(headingLabel, &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(headingMapButton, headingMapButtonScr, LV_EVENT_CLICKED, NULL);

    mapCanvas = lv_canvas_create(mapScreen);
    lv_obj_set_pos(mapCanvas, 0, 25);
    lv_obj_set_size(mapCanvas, 320, 455);
    lv_obj_add_event_cb(mapCanvas, updateMap, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_send_event(mapCanvas, LV_EVENT_VALUE_CHANGED, NULL);

    mapUpdateTimer = lv_timer_create(updateScreen, 1000, NULL);
    lv_timer_ready(mapUpdateTimer);
}

static void updateScreen(lv_timer_t *timer) {
    if(lv_screen_active() != mapScreen) {
        return; // niets doen als het scherm niet zichtbaar is
    }

    lv_obj_send_event(mapCanvas, LV_EVENT_VALUE_CHANGED, NULL);
}

static void updateMapCanvas() {

    maps.generateMap(zoom);

    if(maps.redrawMap){
        maps.displayMap();
        // Byte swap toepassen op de RGB565 buffer
        lv_draw_sw_rgb565_swap(maps.mapBuffer, 320 * 455);

        // Buffer instellen op canvas
        lv_canvas_set_buffer(mapCanvas, maps.mapBuffer, 320, 455, LV_COLOR_FORMAT_RGB565);
    }

    lv_obj_move_foreground(zoomInBtn);
    lv_obj_move_foreground(zoomOutBtn);
    lv_obj_move_foreground(backButtonMapsScr);
    lv_obj_move_foreground(headingMapButton);
}