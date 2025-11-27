#include "statusBar.hpp"

lv_obj_t * statusBar;

lv_timer_t *gpsTimer;

lv_obj_t *latLabel;
lv_obj_t *lonLabel;
lv_obj_t *sattLabel;
lv_obj_t *sdCardIcon;

GPS gps = GPS();

void latLabelChange(const char* latText){
    lv_label_set_text(latLabel, latText);
}   
void lonLabelChange(const char* lonText){
    lv_label_set_text(lonLabel, lonText);
}
void sattLabelChange(const char* sattText){
    lv_label_set_text(sattLabel, sattText);
}

void makeStatusBar(){
    statusBar = lv_obj_create(lv_screen_active());
    lv_obj_set_size(statusBar, 320, 24);
    lv_obj_set_style_bg_color(statusBar, lv_color_hex(0x333333), 0);
    lv_obj_set_pos(statusBar, 0, 0);

    latLabel = lv_label_create(statusBar);
    lv_label_set_text(latLabel, "Lat: --");
    lv_obj_set_pos(latLabel, 5, 0);

    lonLabel = lv_label_create(statusBar);
    lv_label_set_text(lonLabel, "Lon: --");
    lv_obj_set_pos(lonLabel, 120, 0); 

    sattLabel = lv_label_create(statusBar);
    lv_label_set_text(sattLabel, "Satt: --");
    lv_obj_set_pos(sattLabel, 240, 0); 

    sdCardIcon = lv_label_create(statusBar);
    lv_label_set_text(sdCardIcon, LV_SYMBOL_SD_CARD);
    lv_obj_set_pos(sdCardIcon, 290, 0);


    lv_obj_set_style_pad_all(statusBar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(latLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(lonLabel, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(sattLabel, 0, LV_PART_MAIN);
    
    gpsTimer = lv_timer_create(updateGPSTask, 1000, NULL);
    lv_timer_ready(gpsTimer);
    // Voeg hier extra elementen toe aan de statusbalk indien nodig
}

void updateGPSTask(lv_timer_t * timer){
    // Hier zou je de GPS-gegevens moeten ophalen en de labels bijwerken
    // Voorbeeld:
    gps.gpsData = gps.getAllData();
    if(gps.gpsData.satellites < 3){
        latLabelChange("Lat: --");
        lonLabelChange("Lon: --");
    } else {
        latLabelChange(String("Lat: " + String(gps.gpsData.latitude, 6)).c_str());
        lonLabelChange(String("Lon: " + String(gps.gpsData.longitude, 6)).c_str());
    }
    sattLabelChange(String("Satt: " + String(gps.gpsData.satellites)).c_str());
}

void sdLabelChange(bool sdCardAvailable){
    if(sdCardAvailable){
        lv_label_set_text(sdCardIcon, LV_SYMBOL_SD_CARD);
    } else {
        lv_label_set_text(sdCardIcon, LV_SYMBOL_WARNING);
    }
}