#include <Arduino.h>

#include "esp_err.h"

#include <SD.h>

#include "tft.hpp"
#include "homeScreen.hpp"
#include "mapScreen.hpp"
#include "statusBar.hpp"
#include "gps.hpp"
#include "maps.hpp"

extern GPS gps;
extern Maps maps;


void setup() {
  bool sdCardAvailable = false;

  gps.begin(9600);

  pinMode(GPIO_NUM_15, OUTPUT);
  digitalWrite(GPIO_NUM_15, LOW);

  SPI.begin(GPIO_NUM_12, GPIO_NUM_13, GPIO_NUM_11);

  if(!SD.begin(GPIO_NUM_15, SPI, 20000000, "/sdcard")){
    sdCardAvailable = false;
  } else {
    sdCardAvailable = true;
  }

  initTFT();
  initLVGL();

  makeHomeScreen();
  makeMapScreen();

  lv_screen_load(homeScreen);
  makeStatusBar();
  sdLabelChange(sdCardAvailable);
}

void loop() {
  // put your main code here, to run repeatedly:
  lv_timer_handler();
  delay(5);
}
