# TFT Module

Manages the TFT LCD display initialization, graphics rendering, and LVGL integration. Handles touch input calibration and provides the low-level display driver interface.

## Features

- **LovyanGFX Driver**: High-performance graphics library with multiple display support
- **LVGL 9.2.2 Integration**: Full GUI framework support
- **Touch Support**: Capacitive touch with multi-point detection and calibration
- **High Performance**: Memory-mapped rendering with DMA acceleration
- **Sprite Graphics**: Offscreen rendering for flicker-free animation
- **Display Persistence**: Touch calibration saved to SD card

## Hardware Configuration

### Display Specifications

```cpp
extern uint16_t TFT_WIDTH;   // Display width (e.g., 768)
extern uint16_t TFT_HEIGHT;  // Display height (e.g., 768)

extern TFT_eSPI tft;         // Global TFT object
```

### Common Display Resolutions

| Display | Width | Height | DPI | Common Board |
|---------|-------|--------|-----|--------------|
| 3.5" | 320 | 480 | 165 | ESP32 Starter Kit |
| 4.0" | 480 | 320 | 155 | ESP32-S3 |
| 7.0" | 768 | 768 | 145 | Custom BikeESP32 |

### Touch Configuration

```cpp
#define TOUCH_MAX_POINTS 5    // Maximum simultaneous touch points
static const char* calibrationFile = "/sdcard/TouchCal";  // Calibration storage
```

## API Reference

### Initialization Functions

| Function | Purpose |
|----------|---------|
| `initTFT()` | Initialize display driver and graphics engine |
| `initLVGL()` | Initialize LVGL framework on top of TFT |
| `displayFlush()` | LVGL callback for pixel buffer flushing |
| `touchRead()` | LVGL callback for touch input processing |
| `lv_tick_task()` | Timer tick handler for LVGL timing |

### Display Operations

```cpp
// Clear entire screen
tft.fillScreen(TFT_BLACK);

// Draw rectangle
tft.drawRect(x, y, width, height, TFT_WHITE);
tft.fillRect(x, y, width, height, TFT_BLUE);

// Draw circle
tft.drawCircle(centerX, centerY, radius, TFT_GREEN);

// Draw text
tft.setTextColor(TFT_WHITE, TFT_BLACK);
tft.setTextSize(2);
tft.drawString("Hello", x, y);

// Draw line
tft.drawLine(x1, y1, x2, y2, TFT_RED);
```

## Usage Examples

### Initialize Display in Setup

```cpp
#include "tft.hpp"

// In Arduino setup():
void setup() {
    Serial.begin(9600);
    
    // Initialize TFT display
    initTFT();
    
    // Display splash screen (optional)
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("BikeESP32", 10, 10);
    
    // Initialize LVGL on top of TFT
    initLVGL();
}
```

### Clear and Redraw Display

```cpp
// Clear to black background
tft.fillScreen(TFT_BLACK);

// Draw status information
tft.setTextColor(TFT_WHITE);
tft.drawString("GPS: Ready", 10, 20);
tft.drawString("Speed: 25.5 km/h", 10, 40);

// Flush to display
tft.flush();  // May be automatic with LVGL
```

### Create Sprite for Offscreen Rendering

```cpp
// Create sprite (offscreen buffer)
TFT_eSprite sprite = TFT_eSprite(&tft);
sprite.createSprite(100, 100);  // 100x100 pixel buffer

// Draw to sprite (not visible yet)
sprite.fillRect(0, 0, 100, 100, TFT_BLUE);
sprite.drawCircle(50, 50, 25, TFT_WHITE);

// Push sprite to display
sprite.pushSprite(50, 50);      // x, y position on screen

// Clean up
sprite.deleteSprite();
```

### Color Definitions

```cpp
// Standard 16-bit RGB565 colors
TFT_BLACK       // 0x0000
TFT_NAVY        // 0x000F
TFT_DARKGREEN   // 0x03E0
TFT_DARKCYAN    // 0x03EF
TFT_MAROON      // 0x7800
TFT_PURPLE      // 0x780F
TFT_OLIVE       // 0x7BE0
TFT_LIGHTGREY   // 0xC618
TFT_DARKGREY    // 0x7BEF
TFT_BLUE        // 0x001F
TFT_GREEN       // 0x07E0
TFT_CYAN        // 0x07FF
TFT_RED         // 0xF800
TFT_MAGENTA     // 0xF81F
TFT_YELLOW      // 0xFFE0
TFT_WHITE       // 0xFFFF

// Custom colors using hex
uint16_t customColor = tft.color565(R, G, B);  // 8-bit R,G,B → 16-bit color
uint16_t hexColor = tft.color565(0xFF, 0xAA, 0x00);  // Orange
```

## LVGL Integration

### Display Flush Callback

```cpp
void IRAM_ATTR displayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    // Called by LVGL when pixels need updating
    // area: region to update (x1, y1, x2, y2)
    // px_map: pixel data buffer
    
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    
    // Push pixels to display
    tft.pushImage(area->x1, area->y1, w, h, (uint16_t*)px_map);
    
    // Tell LVGL done
    lv_disp_flush_ready(disp);
}
```

### Touch Input Callback

```cpp
void IRAM_ATTR touchRead(lv_indev_t *indev_driver, lv_indev_data_t *data) {
    // Called by LVGL to read touch input
    // data: touch coordinates and state
    
    uint8_t num_touches;
    lv_point_t touch_point;
    
    // Read touch hardware
    // ...
    
    if (touch_detected) {
        data->point.x = touch_point.x;
        data->point.y = touch_point.y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}
```

### Timer Tick Provision

```cpp
void lv_tick_task(void *arg) {
    // Called by hardware timer at regular intervals (e.g., every 1ms)
    lv_tick_inc(1);  // Increment LVGL tick counter
}
```

## Touch Calibration

### Auto-Calibration Process

```cpp
// Calibration is performed on first touch
// User touches screen at calibration points
// Results stored in /sdcard/TouchCal file

// To force recalibration:
storage.remove("/sdcard/TouchCal");
// Restart device
```

### Calibration File Format

```
// /sdcard/TouchCal (binary file)
// Contains touch-to-screen mapping coefficients
// Automatically loaded on startup
```

### Manual Calibration

```cpp
// If calibration file missing/corrupt:
// 1. Device will prompt for touch calibration
// 2. Follow on-screen instructions
// 3. Touch indicated points in sequence
// 4. Calibration saved automatically
```

## Graphics Rendering

### Pixel Formats

```cpp
// Color depth (set in LVGL config)
#define LV_COLOR_DEPTH 16   // 16-bit RGB565 (5-6-5 bits)

// Each pixel: RRRRRGGGGGGBBBBB
// Red:   5 bits (0-31)
// Green: 6 bits (0-63)
// Blue:  5 bits (0-31)
```

### Sprite Rendering Pipeline

```
Application     ← Draw commands
     ↓
LovyanGFX       ← Graphics rendering
     ↓
TFT Buffer      ← Screen memory
     ↓
Display Panel   ← LCD output
     ↓
User            ← Visual feedback
```

## Performance Characteristics

### Display Update

- **Full Screen Refresh**: ~100-200ms (768x768)
- **Partial Update**: ~50-100ms (typical area)
- **Frame Rate**: ~5-10 FPS (full screen)
- **Touch Latency**: ~50ms (input to response)

### Memory Usage

```cpp
// TFT frame buffer (16-bit color)
768 × 768 × 2 bytes = 1.18 MB (PSRAM)

// LVGL drawing buffer
512 × 512 × 2 bytes = 0.5 MB (PSRAM)

Total: ~1.7 MB for graphics
```

## Configuration

### LVGL Configuration

See `lib/lvgl/lv_conf.h`:

```cpp
#define LV_COLOR_DEPTH          16      // RGB565
#define LV_HOR_RES_MAX          768     // Display width
#define LV_VER_RES_MAX          768     // Display height
#define LV_DISP_DEF_REFR_PERIOD 10      // Refresh period (ms)
#define LV_INDEV_DEF_READ_PERIOD 10     // Touch read period (ms)
```

### Display Driver Selection

In config header or platform config:

```cpp
// Select driver: ST7796, ILI9341, GC9A01, etc.
#define LGFX_USE_V1             // Use LovyanGFX V1
#define BOARD_CUSTOM            // Custom board definition
```

## Integration with LVGL

### Initialization Sequence

```
initTFT()       ← Initialize display hardware
    ↓
initLVGL()      ← Create LVGL display object
    ↓
lv_display_drv ← Register callbacks
    ↓
lv_indev_drv   ← Register touch device
    ↓
Ready for Use
```

### Main Loop Integration

```cpp
void loop() {
    // LVGL main handler (process events and timers)
    lv_timer_handler();
    
    // Small delay to prevent CPU oversaturation
    delay(5);
}
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Display blank | Check power supply and data lines; verify GPIO config |
| Wrong colors | Check color format (RGB vs BGR); verify color depth |
| Touch not working | Recalibrate `/sdcard/TouchCal` or regenerate |
| Display flickering | Reduce update frequency; check PSRAM stability |
| Memory overflow | Reduce sprite sizes; optimize LVGL buffers |
| Slow rendering | Profile with task timer; consider lower resolution |

### Serial Debug Output

```cpp
// Enable serial logging for diagnostics
Serial.begin(9600);

// In initTFT():
Serial.printf("TFT: %d x %d\n", TFT_WIDTH, TFT_HEIGHT);
Serial.printf("Color depth: %d bits\n", LV_COLOR_DEPTH);
Serial.printf("Touch calibration: %s\n", 
    storage.exists("/sdcard/TouchCal") ? "OK" : "MISSING");
```

## Display Module Selection

BikeESP32 supports various TFT displays through LovyanGFX:

### Supported Controllers

| Controller | Common Sizes | Brightness | Cost |
|-----------|----------|-----------|------|
| ST7796 | 3.5", 4.0" | Very High | $ |
| ILI9341 | 2.4", 2.8", 3.2" | High | $ |
| GC9A01 | 1.28" Round | Medium | $ |
| ILI9806 | 4.0", 5.0" | High | $$ |
| ILI9488 | 3.5", 4.0" | Very High | $$ |

### Custom Board Setup

Create `display.hpp` with hardware pinout:

```cpp
#define TFT_CS      15
#define TFT_DC      2
#define TFT_MOSI    13
#define TFT_CLK     14
#define TFT_MISO    12
#define TFT_RST     4
#define TOUCH_SDA   21
#define TOUCH_SCL   22
```

## See Also

- [GUI Module](../gui/README.md) - LVGL screens on TFT
- [Maps Module](../maps/README.md) - Map rendering
- LovyanGFX: https://github.com/lovyan03/LovyanGFX
- LVGL Display Driver: https://docs.lvgl.io/master/porting/display.html
- TFT Display Selection: https://www.ebay.com/sch/i.html?_nkw=esp32+tft+display
