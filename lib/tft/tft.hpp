#pragma once

#include <Arduino.h>

#include "display.hpp"
#include <lvgl.h>
#include <LGFX_TFT_eSPI.hpp>
#include "storage.hpp"

#define TOUCH_MAX_POINTS 5

extern TFT_eSPI tft;

static const char* calibrationFile PROGMEM = "/sdcard/TouchCal"; /**< Touch calibration file path */

extern uint16_t TFT_WIDTH;
extern uint16_t TFT_HEIGHT;

void initTFT();

void IRAM_ATTR displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void IRAM_ATTR touchRead(lv_indev_t *indev_driver, lv_indev_data_t *data);
void lv_tick_task(void *arg);
void initLVGL();