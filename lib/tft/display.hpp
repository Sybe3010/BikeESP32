/**
 * @file ILI9488_XPT2046_SPI.hpp
 * @author Jordi Gauchía (jgauchia@jgauchia.com)
 * @brief  LOVYANGFX TFT driver for ILI9488 SPI With XPT2046 Touch controller
 * @version 0.2.2
 * @date 2025-05
 */

#pragma once

#define LGFX_USE_V1

#include "LovyanGFX.hpp"

#define LARGE_SCREEN
#define TOUCH_INPUT

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ILI9488 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_XPT2046 _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = SPI2_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 79999999;
      cfg.freq_read = 15000000;
      cfg.spi_3wire = false;
      cfg.use_lock = false;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = GPIO_NUM_12;
      cfg.pin_mosi = GPIO_NUM_11;
      cfg.pin_miso = GPIO_NUM_13;
      cfg.pin_dc = GPIO_NUM_3;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = GPIO_NUM_10;
      cfg.pin_rst = GPIO_NUM_16;
      cfg.pin_busy = -1;
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.memory_width = 320;
      cfg.memory_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = false;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;
      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = GPIO_NUM_7;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0;
      cfg.x_max = 319;
      cfg.y_min = 0;
      cfg.y_max = 479;
      cfg.pin_int = GPIO_NUM_5;
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      cfg.spi_host = SPI3_HOST;
      cfg.freq = 1000000;
      cfg.pin_sclk = GPIO_NUM_17;
      cfg.pin_mosi = GPIO_NUM_18;
      cfg.pin_miso = GPIO_NUM_46;
      cfg.pin_cs = GPIO_NUM_4;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};