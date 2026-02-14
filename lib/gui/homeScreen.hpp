#pragma once

#include "bluetoothScreen.hpp"
#include "mapScreen.hpp"

extern std::string typeActivity;

// Hoofd scherm tiles
extern lv_obj_t *homeTile;
extern lv_obj_t *settingTile; // staat boven de homeTile
extern lv_obj_t *optionsTile; // staat onder de homeTile

// Tiles voor de keuze activiteiten scherm
extern lv_obj_t *activityTiles;
extern lv_obj_t *roadActivityTile;
extern lv_obj_t *gravelActivityTile;
extern lv_obj_t *mtbActivityTile;

//knoppen voor activiteiten keuze scherm
extern lv_obj_t *roadActivityBtn;
extern lv_obj_t *gravelActivityBtn;
extern lv_obj_t *mtbActivityBtn;

// knoppen
extern lv_obj_t *navigationButton;
extern lv_obj_t *optionsButton;
extern lv_obj_t *settingsButton;

// knoppen voor settingsScreen
extern lv_obj_t *backButton;
extern lv_obj_t *accountButton;
extern lv_obj_t *bluetoothButton;
extern lv_obj_t *moreSettingsButton;

// knoppen voor optionsScreen
extern lv_obj_t *brightnessSlider;
extern lv_obj_t *mapOptionBtn;
extern lv_obj_t *activityOptionBtn;
extern lv_obj_t *touchscreenOptionBtn;

// Event handlers
void navBtnEvent(lv_event_t * e);
void optionsBtnEvent(lv_event_t * e);
void settingsBtnEvent(lv_event_t * e);

void backBtnEvent(lv_event_t * e);
void accountBtnEvent(lv_event_t * e);
void bluetoothBtnEvent(lv_event_t * e);
void moreSettingsBtnEvent(lv_event_t * e);

void mapOptionBtnEvent(lv_event_t * e);
void activityOptionBtnEvent(lv_event_t * e);
void touchscreenOptionBtnEvent(lv_event_t * e);

void changeBrightnessEvent(lv_event_t *e);

void roadActBtnEvent(lv_event_t * e);

// Functie om het home screen te maken
void makeHomeScreen();