#include <Arduino.h>

#include "esp_err.h"

#include "storage.hpp"

#include "tft.hpp"
#include "homeScreen.hpp"
#include "mapScreen.hpp"
#include "statusBar.hpp"
#include "navigationMenuScreen.hpp"
#include "gpxListScreen.hpp"
#include "gps.hpp"
#include "maps.hpp"
#include "gpxDetailScreen.hpp"

#include "activity.hpp"

extern GPS gps;
extern Maps maps;
extern Storage storage;


void setup() {
  bool sdCardAvailable = false;

  gps.begin(9600);

  storage.initSD();
  storage.initSPIFFS();

  initTFT();

  if(!storage.exists("/sdcard/TRK")){
    if(!storage.mkdir("/sdcard/TRK")){
      tft.fillScreen(TFT_RED);
    }
  }
  
  initLVGL();

  makeHomeScreen();
  makeMapScreen();
  makeNavigationMenuScreen();
  makeGpxListScreen();
  makeGpxDetailsScreen();

  lv_screen_load(homeScreen);
  makeStatusBar();

  updateGpxListScreen();
}

void loop() {
  // put your main code here, to run repeatedly:
  lv_timer_handler();
  delay(5);
}


  // Maak een ActivityPoint aan en vul met gegevens
  // Activity::ActivityPoint ap = {
  //   .lon = 4.438024f,
  //   .lat = 51.252376f,
  //   .speed = 15.5f,
  //   .ele = 10.0f,
  //   .cadance = 85,
  //   .hartrate = 120,
  //   .power = 200
  // };

  // testAct.startActivity();
  // testAct.addActivityPoint(ap);
