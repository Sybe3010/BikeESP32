#include "tft.hpp"

TFT_eSPI tft = TFT_eSPI();

uint16_t TFT_WIDTH = 0;
uint16_t TFT_HEIGHT = 0;

lv_display_t *display;

void initTFT()
{
  tft.init();

  tft.setRotation(2);

  TFT_HEIGHT = tft.height();
  TFT_WIDTH = tft.width();

  tft.initDMA();
  tft.fillScreen(TFT_BLACK);

  tft.calibrateTouch(nullptr, TFT_WHITE, TFT_BLACK, std::max(tft.width(), tft.height()) >> 3);
}

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

/**
 * @brief LVGL touch read
 *
 */
void IRAM_ATTR touchRead(lv_indev_t *indev_driver, lv_indev_data_t *data)
{
  lgfx::touch_point_t tp;
    data->state = LV_INDEV_STATE_RELEASED;

    if (tft.getTouch(&tp, TOUCH_MAX_POINTS)) 
    {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = tp.x;
        data->point.y = tp.y;
    }
}

void lv_tick_task(void *arg){
  (void)arg;      
  lv_tick_inc(1);
}

void initLVGL()
{
  lv_init();

  display = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
  lv_display_set_flush_cb(display, displayFlush);
  lv_display_set_flush_wait_cb(display, NULL);

  size_t DRAW_BUF_SIZE = 0;
  DRAW_BUF_SIZE = TFT_WIDTH * TFT_HEIGHT * sizeof(lv_color_t);

  lv_color_t * drawBuf1 = (lv_color_t *)heap_caps_aligned_alloc(16, DRAW_BUF_SIZE, MALLOC_CAP_SPIRAM);
  lv_color_t * drawBuf2 = (lv_color_t *)heap_caps_aligned_alloc(16, DRAW_BUF_SIZE, MALLOC_CAP_SPIRAM);
  lv_display_set_buffers(display, drawBuf1, drawBuf2, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev_drv = lv_indev_create();
  lv_indev_set_type(indev_drv, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_drv, touchRead);

  const esp_timer_create_args_t periodic_timer_args = { .callback = &lv_tick_task, .name = "lv_tick" };
  esp_timer_handle_t periodic_timer;
  esp_timer_create(&periodic_timer_args, &periodic_timer);
  esp_timer_start_periodic(periodic_timer, 1000); // 1000 µs = 1 ms
}