#include "homeScreen.hpp"

// Hoofd scherm object: beschikbaar van overal
lv_obj_t *homeScreen;
lv_obj_t *homeScreenTiles;

// objecten in hoofdscherm: niet van overal beschikbaar
lv_obj_t *homeTile;
lv_obj_t *settingTile; 
lv_obj_t *optionsTile; 
lv_obj_t *activityTiles;
lv_obj_t *roadActivityTile;
lv_obj_t *gravelActivityTile;
lv_obj_t *mtbActivityTile;
lv_obj_t *roadActivityBtn;
lv_obj_t *gravelActivityBtn;
lv_obj_t *mtbActivityBtn;
lv_obj_t *navigationButton;
lv_obj_t *optionsButton;
lv_obj_t *settingsButton;
lv_obj_t *backButton;
lv_obj_t *accountButton;
lv_obj_t *bluetoothButton;
lv_obj_t *moreSettingsButton;
lv_obj_t *brightnessOptionBtn;
lv_obj_t *mapOptionBtn;
lv_obj_t *activityOptionBtn;
lv_obj_t *touchscreenOptionBtn;

void navBtnEvent(lv_event_t * e){
    lv_screen_load(navigationMenuScreen);
}
void optionsBtnEvent(lv_event_t * e){
    lv_tileview_set_tile(homeScreenTiles, optionsTile, LV_ANIM_ON);
}
void settingsBtnEvent(lv_event_t * e){
    lv_tileview_set_tile(homeScreenTiles, settingTile, LV_ANIM_ON);
}

void backBtnEvent(lv_event_t * e){
    
}
void accountBtnEvent(lv_event_t * e){
    
}
void bluetoothBtnEvent(lv_event_t * e){
    updateBleScreen();
    lv_screen_load(bleScreen);
}
void moreSettingsBtnEvent(lv_event_t * e){

}

void brightnessOptionBtnEvent(lv_event_t * e){

}
void mapOptionBtnEvent(lv_event_t * e){

}
void activityOptionBtnEvent(lv_event_t * e){

}
void touchscreenOptionBtnEvent(lv_event_t * e){
   
}

void roadActBtnEvent(lv_event_t * e){
    typeActivity = "Road";
    lv_obj_send_event(activityHomeScreen, LV_EVENT_VALUE_CHANGED, NULL);
    lv_screen_load(activityHomeScreen);
}

void makeHomeScreen(){
    homeScreen = lv_obj_create(NULL); // Maak een nieuw scherm object aan

    // HomeScreen Tiles
    homeScreenTiles = lv_tileview_create(homeScreen); // Maak een container voor de tegels op het homescreen
    settingTile = lv_tileview_add_tile(homeScreenTiles, 0, 0, LV_DIR_BOTTOM); // Voeg de instellingentegel toe boven de homeTile
    homeTile = lv_tileview_add_tile(homeScreenTiles, 0, 1, (lv_dir_t)(LV_DIR_TOP | LV_DIR_BOTTOM)); // Voeg de homeTile toe in het midden
    optionsTile = lv_tileview_add_tile(homeScreenTiles, 0, 2, LV_DIR_TOP); // Voeg de optieentegel toe onder de homeTile
    lv_obj_set_size(homeScreenTiles, lv_disp_get_hor_res(display) , lv_disp_get_ver_res(display) - 25); //-25 om ruimte te laten voor de statusbalk
    lv_obj_set_pos(homeScreenTiles, 0, 25);

    lv_tileview_set_tile(homeScreenTiles, homeTile, LV_ANIM_ON); // Zet de homeTile als standaard zichtbare tegel

    // Activiteiten keuze scherm tiles
    activityTiles = lv_tileview_create(homeTile); // Maak een container voor de tegels op het activiteiten keuze scherm
    roadActivityTile = lv_tileview_add_tile(activityTiles, 0, 0, LV_DIR_RIGHT); 
    gravelActivityTile = lv_tileview_add_tile(activityTiles, 1, 0, (lv_dir_t)(LV_DIR_LEFT | LV_DIR_RIGHT)); 
    mtbActivityTile = lv_tileview_add_tile(activityTiles, 2, 0, LV_DIR_LEFT);
    lv_obj_set_size(activityTiles, 300 , 188); //-20 en -35 om ruimte te laten voor de marges en statusbalk
    lv_obj_set_pos(activityTiles, 10, 10);
    
    //Maak activity tiles inhoud
    roadActivityBtn = lv_button_create(roadActivityTile);
    lv_obj_set_size(roadActivityBtn, 300, 188);
    lv_obj_set_pos(roadActivityBtn, 0, 0);
    lv_obj_t* roadLabel = lv_label_create(roadActivityBtn);
    lv_label_set_text(roadLabel, "Road");
    lv_obj_set_pos(roadLabel, 10, 30);

    gravelActivityBtn = lv_button_create(gravelActivityTile);
    lv_obj_set_size(gravelActivityBtn, 300, 188);
    lv_obj_set_pos(gravelActivityBtn, 0, 0);
    lv_obj_t* gravelLabel = lv_label_create(gravelActivityBtn);
    lv_label_set_text(gravelLabel, "Gravel");
    lv_obj_set_pos(gravelLabel, 10, 30);

    mtbActivityBtn = lv_button_create(mtbActivityTile);
    lv_obj_set_size(mtbActivityBtn, 300, 188);
    lv_obj_set_pos(mtbActivityBtn, 0, 0);
    lv_obj_t* mtbLabel = lv_label_create(mtbActivityBtn);
    lv_label_set_text(mtbLabel, "Mountainbike");
    lv_obj_set_pos(mtbLabel, 10, 30);

    // Add homeScreen buttons
    navigationButton = lv_button_create(homeTile);
    lv_obj_set_size(navigationButton, 75, 75);
    lv_obj_set_pos(navigationButton, 32, 308);

    optionsButton = lv_button_create(homeTile);
    lv_obj_set_size(optionsButton, 75, 75);
    lv_obj_set_pos(optionsButton, 123, 308);
    
    settingsButton = lv_button_create(homeTile);
    lv_obj_set_size(settingsButton, 75, 75);
    lv_obj_set_pos(settingsButton, 214, 308);

    //labels voor bij de knoppen (homescreen)
    lv_obj_t* navLabel = lv_label_create(homeTile);
    lv_label_set_text(navLabel, "Navigation");
    lv_obj_set_pos(navLabel, 30, 396);
    lv_obj_t* optionsLabel = lv_label_create(homeTile);
    lv_label_set_text(optionsLabel, "Options");
    lv_obj_set_pos(optionsLabel, 130, 396);
    lv_obj_t* settingsLabel = lv_label_create(homeTile);
    lv_label_set_text(settingsLabel, "Settings");
    lv_obj_set_pos(settingsLabel, 220, 396);
    

    //Home buttons events
    lv_obj_add_event_cb(navigationButton, navBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(optionsButton, optionsBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(settingsButton, settingsBtnEvent, LV_EVENT_CLICKED, NULL);

    lv_obj_add_event_cb(roadActivityBtn, roadActBtnEvent, LV_EVENT_LONG_PRESSED, NULL);


    //Settings Tile inhoud
    lv_obj_t* settingTitle = lv_label_create(settingTile);
    lv_label_set_text(settingTitle, "Settings");
    lv_obj_set_pos(settingTitle, 32, 25);

    backButton = lv_button_create(settingTile);
    lv_obj_set_size(backButton, 96, 96);
    lv_obj_set_pos(backButton, 32, 92);

    accountButton = lv_button_create(settingTile);
    lv_obj_set_size(accountButton, 96, 96);
    lv_obj_set_pos(accountButton, 172, 92);

    bluetoothButton = lv_button_create(settingTile);
    lv_obj_set_size(bluetoothButton, 96, 96);
    lv_obj_set_pos(bluetoothButton, 172, 262);

    moreSettingsButton = lv_button_create(settingTile);
    lv_obj_set_size(moreSettingsButton, 96, 96);
    lv_obj_set_pos(moreSettingsButton, 32, 262);

    //Labels voor settings buttons
    lv_obj_t* backLabel = lv_label_create(settingTile);
    lv_label_set_text(backLabel, "WiFi");
    lv_obj_set_pos(backLabel,  40, 190);
    lv_obj_t* accountLabel = lv_label_create(settingTile);
    lv_label_set_text(accountLabel, "Account");
    lv_obj_set_pos(accountLabel,  210, 190);
    lv_obj_t* bluetoothLabel = lv_label_create(settingTile);
    lv_label_set_text(bluetoothLabel, "Bluetooth");
    lv_obj_set_pos(bluetoothLabel,  200, 360);
    lv_obj_t* moreSettingsLabel = lv_label_create(settingTile);
    lv_label_set_text(moreSettingsLabel, "More");
    lv_obj_set_pos(moreSettingsLabel,  60, 360);

    // Events voor settings buttons
    lv_obj_add_event_cb(backButton, backBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(accountButton, accountBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(bluetoothButton, bluetoothBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(moreSettingsButton, moreSettingsBtnEvent, LV_EVENT_CLICKED, NULL);

    // Options tile inhoud
    lv_obj_t* optionsTitle = lv_label_create(optionsTile);
    lv_label_set_text(optionsTitle, "Options");
    lv_obj_set_pos(optionsTitle, 32, 25);

    brightnessOptionBtn = lv_button_create(optionsTile);
    lv_obj_set_size(brightnessOptionBtn, 256, 96);
    lv_obj_set_pos(brightnessOptionBtn, 32, 70);

    mapOptionBtn = lv_button_create(optionsTile);
    lv_obj_set_size(mapOptionBtn, 256, 96);
    lv_obj_set_pos(mapOptionBtn, 32, 176);

    activityOptionBtn = lv_button_create(optionsTile);
    lv_obj_set_size(activityOptionBtn, 256, 96);
    lv_obj_set_pos(activityOptionBtn, 32, 282);

    touchscreenOptionBtn = lv_button_create(optionsTile);
    lv_obj_set_size(touchscreenOptionBtn, 256, 96);
    lv_obj_set_pos(touchscreenOptionBtn, 32, 388);

    //Labels voor options buttons
    lv_obj_t* brightnessLabel = lv_label_create(brightnessOptionBtn);
    lv_label_set_text(brightnessLabel, "Brightness settings");
    lv_obj_set_pos(brightnessLabel,  30, 20);
    lv_obj_t* mapLabel = lv_label_create(mapOptionBtn);
    lv_label_set_text(mapLabel, "Map settings");
    lv_obj_set_pos(mapLabel,  30, 20);
    lv_obj_t* activityLabel = lv_label_create(activityOptionBtn);
    lv_label_set_text(activityLabel, "Activity settings");
    lv_obj_set_pos(activityLabel,  30, 20);
    lv_obj_t* touchscreenLabel = lv_label_create(touchscreenOptionBtn);
    lv_label_set_text(touchscreenLabel, "Touchscreen calibration");
    lv_obj_set_pos(touchscreenLabel,  30, 20);

    // Events voor options buttons
    lv_obj_add_event_cb(brightnessOptionBtn, brightnessOptionBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(mapOptionBtn, mapOptionBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(activityOptionBtn, activityOptionBtnEvent, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(touchscreenOptionBtn, touchscreenOptionBtnEvent, LV_EVENT_CLICKED, NULL);
}
