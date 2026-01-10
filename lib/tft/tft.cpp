#include "tft.hpp"

TFT_eSPI tft = TFT_eSPI(); // maak een nieuw TFT_eSPI object aan

uint16_t TFT_WIDTH = 0;
uint16_t TFT_HEIGHT = 0;

lv_display_t *display; // maak een lvgl display object aan

/// @brief Initialiseer het TFT scherm
/// @note Stelt de rotatie in, vult het scherm met zwart en kalibreert het touchscreen
void initTFT()
{
  tft.init();

  tft.setRotation(2);

  TFT_HEIGHT = tft.height();
  TFT_WIDTH = tft.width();

  tft.initDMA(); // Zet direct memory access aan voor snellere scherm updates
  tft.fillScreen(TFT_BLACK);

  tft.calibrateTouch(nullptr, TFT_WHITE, TFT_BLACK, std::max(tft.width(), tft.height()) >> 3);
  // Kalibreer het touchscreen: nadien nog de kalibratiewaarden opslaan op de SD kaart
}

/// @brief LVGL display flush
/// @note Een callback functie die door LVGL wordt aangeroepen om een deel van het scherm up te daten.
/// Word in RAM geplaatst voor snellere toegang.
void IRAM_ATTR displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{ 
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.setSwapBytes(true);
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t*)px_map);
  tft.setSwapBytes(false);

  lv_display_flush_ready(disp);
}

/// @brief LVGL touchscreen read
/// @note Een callback functie die door LVGL wordt aangeroepen om de status van het touchscreen te lezen.
/// Word in RAM geplaatst voor snellere toegang.
void IRAM_ATTR touchRead(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
  lgfx::touch_point_t tp;
    data->state = LV_INDEV_STATE_RELEASED; // Maakt een datapunt aan dat LVGL gebruikt om de touch status bij te houden

  if (tft.getTouch(&tp, TOUCH_MAX_POINTS)) 
  {
      data->state = LV_INDEV_STATE_PRESSED;
      data->point.x = tp.x;
      data->point.y = tp.y;
  }
}

/// @brief LVGL tick taak
/// @note Een periodieke taak die LVGL's interne tick teller bijwerkt.
void lv_tick_task(void *arg){ 
  (void)arg;      
  lv_tick_inc(1);
}


/// @brief Initialiseer LVGL
/// @note Stelt de buffers en callbacks in voor LVGL

void initLVGL()
{
  // Initialiseer LVGL
  lv_init();

  // Maak een display aan voor LVGL
  display = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
  lv_display_set_flush_cb(display, displayFlush);
  lv_display_set_flush_wait_cb(display, NULL);

  size_t DRAW_BUF_SIZE = 0;
  DRAW_BUF_SIZE = TFT_WIDTH * TFT_HEIGHT * sizeof(lv_color_t);

  // Scherm teken buffers aanmaken in PSRAM
  lv_color_t * drawBuf1 = (lv_color_t *)heap_caps_aligned_alloc(16, DRAW_BUF_SIZE, MALLOC_CAP_SPIRAM);
  lv_color_t * drawBuf2 = (lv_color_t *)heap_caps_aligned_alloc(16, DRAW_BUF_SIZE, MALLOC_CAP_SPIRAM);
  lv_display_set_buffers(display, drawBuf1, drawBuf2, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);


  // Touchscreen input device aanmaken
  lv_indev_t *indev_drv = lv_indev_create();
  lv_indev_set_type(indev_drv, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_drv, touchRead);

  const esp_timer_create_args_t periodic_timer_args = { .callback = &lv_tick_task, .name = "lv_tick" }; // Timer aanmaken voor LVGL tick taak
  esp_timer_handle_t periodic_timer; 
  esp_timer_create(&periodic_timer_args, &periodic_timer);
  esp_timer_start_periodic(periodic_timer, 1000); // 1000 µs = 1 ms
}