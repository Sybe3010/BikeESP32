#include <Arduino.h>

#include "esp_err.h"

#include "storage.hpp"
#include "bluetooth.hpp"

#include "tft.hpp"
#include "homeScreen.hpp"
#include "mapScreen.hpp"
#include "statusBar.hpp"
#include "navigationMenuScreen.hpp"
#include "gpxListScreen.hpp"  
#include "gps.hpp"
#include "maps.hpp"
#include "gpxDetailScreen.hpp"
#include "bluetoothScreen.hpp"
#include "activityHomePage.hpp"

#include "activity.hpp"

extern GPS gps; // Gebruik het GPS object dat al is aangemaakt
extern Maps maps; // Gebruik het Maps object dat al is aangemaakt
extern Storage storage; // Gebruik het Storage object dat al is aangemaakt
extern Bluetooth bleSensors;  // Gebruik het Bluetooth object dat al is aangemaakt


void setup() {
  Serial.begin(9600); // start de seriele poort voor debugging
  gps.begin(9600); // start de seriele poort voor de GPS module

  storage.initSD(); // initialiseer de SD kaart
  storage.initSPIFFS(); // initialiseer SPIFFS

  if(!storage.exists("/sdcard/TRK")){ // Controleer of de map TRK bestaat anders maak deze aan
    if(!storage.mkdir("/sdcard/TRK")){
      tft.fillScreen(TFT_RED); // Fout bij het aanmaken van de map, vul het scherm met rood
    }
  }
  

  initTFT(); // initialiseer het TFT scherm

  bleSensors.init(); // initialiseer Bluetooth

  initLVGL(); // initialiseer LVGL


  /// GUI schermen aanmaken /// (nadien nog in initLVGL zetten.)
  makeHomeScreen(); // maak het homescreen
  makeMapScreen(); // maak het kaartscherm
  makeNavigationMenuScreen(); // maak het navigatiemenu scherm
  makeGpxListScreen(); // maak het GPX lijst scherm
  makeGpxDetailsScreen(); // maak het GPX detail scherm
  makeBleScreen(); // maak het Bluetooth scherm
  

  lv_screen_load(homeScreen); // laad het homescreen
  makeStatusBar(); // maak de statusbalk
  makeActivityHomePage();

  updateGpxListScreen(); // update de GPX lijst scherm

  //bleSensors.startScan();
  //bleSensors.connectToDevice(bleSensors.targetDevice);
}

void loop() {
  // put your main code here, to run repeatedly:
  lv_timer_handler(); // LVGL taak handler
  delay(1); // Kleine delay (1ms ipv 5ms) voor beter timing met LVGL tick
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
