#include "mapScreen.hpp"

Maps maps = Maps();

lv_obj_t * mapScreen;
lv_obj_t * mapCanvas;
lv_obj_t * zoomInBtn;
lv_obj_t * zoomOutBtn;
lv_obj_t * backButtonMapsScr;

lv_timer_t *mapUpdateTimer;

int zoom = 15;

void zoomOutMapScr(lv_event_t *e){
    if(zoom > 1){
        zoom--;
    }
}
void zoomInMapScr(lv_event_t *e){
    if(zoom < 17){
        zoom++;
    }
}

void backButtonMapScr(lv_event_t *e){
    lv_screen_load(homeScreen);
}

void makeMapScreen() {
    // Initialize maps system
    maps.initMap(320, 455);  // Match canvas size
    maps.createMapScrSprites();

    mapScreen = lv_obj_create(NULL);
    lv_obj_set_size(mapScreen, 320, 455);  

    backButtonMapsScr = lv_btn_create(mapScreen);
    lv_obj_set_size(backButtonMapsScr, 50, 30);
    lv_obj_set_pos(backButtonMapsScr, 10, 5);
    lv_obj_add_event_cb(backButtonMapsScr,backButtonMapScr, LV_EVENT_CLICKED, NULL);

    zoomInBtn = lv_btn_create(mapScreen);
    lv_obj_set_size(zoomInBtn, 50, 30);
    lv_obj_set_pos(zoomInBtn, 260, 5);
    lv_obj_t* zoomInLabel = lv_label_create(zoomInBtn);
    lv_label_set_text(zoomInLabel, "+");
    lv_obj_add_event_cb(zoomInBtn, zoomInMapScr, LV_EVENT_CLICKED, NULL);

    zoomOutBtn = lv_btn_create(mapScreen);
    lv_obj_set_size(zoomOutBtn, 50, 30);
    lv_obj_set_pos(zoomOutBtn, 200, 5);
    lv_obj_t* zoomOutLabel = lv_label_create(zoomOutBtn);
    lv_label_set_text(zoomOutLabel, "-");
    lv_obj_add_event_cb(zoomOutBtn, zoomOutMapScr, LV_EVENT_CLICKED, NULL);

    mapCanvas = lv_canvas_create(mapScreen);
    lv_obj_set_pos(mapCanvas, 0, 25);
    lv_obj_set_size(mapCanvas, 320, 455);

    mapUpdateTimer = lv_timer_create(updateMapCanvas, 1000, NULL);
    lv_timer_ready(mapUpdateTimer);
}

static void updateMapCanvas(lv_timer_t *timer) {

    if(lv_screen_active() != mapScreen) {
        return; // niets doen als het scherm niet zichtbaar is
    }

    maps.generateMap(zoom);

    if(maps.redrawMap){
        maps.displayMap();
        // Byte swap toepassen op de RGB565 buffer
        lv_draw_sw_rgb565_swap(maps.mapBuffer, 320 * 455);

        // Buffer instellen op canvas
        lv_canvas_set_buffer(mapCanvas, maps.mapBuffer, 320, 455, LV_COLOR_FORMAT_RGB565);
    }
}