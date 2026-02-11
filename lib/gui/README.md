# GUI Module

Provides all user interface screens and components using LVGL (Light and Versatile Graphics Library). Manages home screen, maps, activity pages, navigation, and Bluetooth sensor displays.

## Features

- **LVGL 9.2.2**: Latest lightweight GUI framework for embedded systems
- **Multiple Screens**: Home, maps, activity tracking, GPX details, Bluetooth, navigation
- **Touch Input**: Capacitive touchscreen support with gesture recognition
- **Status Bar**: Real-time GPS, battery, and connection status
- **Responsive Design**: Adapts to different display resolutions
- **Screen Transitions**: Smooth animations between screens
- **Real-time Updates**: Timer-based UI refresh without blocking

## Screen Overview

| Screen | Module | Purpose |
|--------|--------|---------|
| Home Screen | `homeScreen.cpp` | Main dashboard with activity tiles |
| Map Screen | `mapScreen.cpp` | GPS track visualization and navigation |
| GPX List Screen | `gpxListScreen.cpp` | Browse saved activities |
| GPX Detail Screen | `gpxDetailScreen.cpp` | View track statistics and route |
| Bluetooth Screen | `bluetoothScreen.cpp` | Pair and manage sensors |
| Activity Page | `activityPage.cpp` | Real-time activity data display |
| Activity Home | `activityHomePage.cpp` | Activity selection and startup |
| Navigation Menu | `navigationMenuScreen.cpp` | Global navigation and settings |
| Status Bar | `statusBar.cpp` | Header with GPS/battery/time info |

## Architecture

### LVGL Object Hierarchy

```
Display (lv_display_t)
├── homeScreen
├── mapScreen
├── gpxListScreen
├── gpxDetailsScreen
├── bleScreen
├── activityHomeScreen
├── activityPage
├── navigationMenuScreen
└── statusBar (overlay)
```

### Global Screen Objects

Defined in `globalGuiDef.h`:

```cpp
extern lv_display_t *display;        // Main display handle

// Screen objects
extern lv_obj_t *homeScreen;
extern lv_obj_t *mapScreen;
extern lv_obj_t *navigationMenuScreen;
extern lv_obj_t *gpxListScreen;
extern lv_obj_t *gpxDetailsScreen;
extern lv_obj_t *bleScreen;
extern lv_obj_t *activityHomeScreen;
extern lv_obj_t *activityPage;

// Status bar
extern lv_obj_t *statusBar;

// Timers
extern lv_timer_t *gpsTimer;  // GPS update timer
```

## API Reference

### Screen Creation Functions

Each screen module exports a creation and update function:

```cpp
// Home Screen
void makeHomeScreen();          // Create home screen
void updateHomeScreen();        // Refresh after data change

// Map Screen  
void makeMapScreen();           // Initialize map display
void updateMapScreen();         // Redraw map with new data

// Activity Pages
void createActivityPage();      // Create activity UI
void makeActivityHomePage();    // Activity selection page
void updateActivityPage();      // Update running metrics

// GPX Screens
void makeGpxListScreen();       // List saved activities
void updateGpxListScreen();     // Refresh file list
void makeGpxDetailsScreen();    // Show track details

// Bluetooth
void makeBleScreen();           // Sensor pairing interface
void updateBleScreen();         // Show connection status

// Navigation & Status
void makeNavigationMenuScreen(); // Main menu
void makeStatusBar();            // Header bar
void updateStatusBar();          // Show GPS/battery status
```

### Common LVGL Patterns

```cpp
// Create a button
lv_obj_t *btn = lv_button_create(parent);
lv_obj_set_size(btn, 200, 50);
lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

// Create a label
lv_obj_t *label = lv_label_create(parent);
lv_label_set_text(label, "Hello World");

// Update label text dynamically
lv_label_set_text_fmt(label, "Speed: %.1f km/h", speed);

// Create a container with flex layout
lv_obj_t *container = lv_obj_create(parent);
lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
lv_obj_set_size(container, 320, 200);
```

## Event Handling

### Standard Button Press

```cpp
static void button_event_handler(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_CLICKED) {
        // Button was pressed
        Serial.println("Button pressed");
        
        // Load new screen
        lv_screen_load(mapScreen);
    }
}

// Attach handler to button
lv_obj_add_event_cb(button, button_event_handler, LV_EVENT_CLICKED, NULL);
```

### Touch Input

Touch is handled by TFT module and callback:

```cpp
void IRAM_ATTR touchRead(lv_indev_t *indev_driver, lv_indev_data_t *data) {
    // Touch data read and provided to LVGL
    // Processed by event handlers above
}
```

## Widget Components

### Tiles Layout (Home Screen)

```cpp
// Create tile view
lv_obj_t *tileview = lv_tileview_create(screen);
lv_obj_set_size(tileview, 320, 480);

// Add individual tiles
lv_obj_t *tile_1 = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
lv_obj_t *tile_2 = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT);

// Add widget to tile
lv_obj_t *label = lv_label_create(tile_1);
lv_label_set_text(label, "Activity 1");
```

### Progress Indicators

```cpp
// Simple bar chart for elevation
lv_obj_t *chart = lv_chart_create(parent);
lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
lv_obj_set_size(chart, 300, 150);

lv_chart_series_t *series = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), 
                                               LV_CHART_AXIS_PRIMARY_Y);
```

## Styling and Themes

### Default Styling

Styles are configured through LVGL config and theme:

```cpp
// Font sizes
#define LV_FONT_DEF         lv_font_montserrat_14
#define LV_FONT_LARGE       lv_font_montserrat_24
#define LV_FONT_SMALL       lv_font_montserrat_10

// Colors (modify in LVGL config)
#define LV_COLOR_BG         lv_color_hex(0x000000)  // Black
#define LV_COLOR_TEXT       lv_color_hex(0xFFFFFF)  // White
```

### Custom Styles

```cpp
static lv_style_t style_btn;
lv_style_init(&style_btn);
lv_style_set_bg_color(&style_btn, lv_color_hex(0x0066CC));
lv_style_set_border_color(&style_btn, lv_color_hex(0xFFFFFF));
lv_style_set_border_width(&style_btn, 2);

lv_obj_add_style(button, &style_btn, 0);
```

## Performance Optimization

- **Partial Redraws**: Only updated screen regions refreshed
- **Timer Management**: Use `lv_timer_t` for periodic updates (not blocking)
- **Memory Pool**: Pre-allocate screen objects at startup
- **Motion Animations**: Smooth transitions without jank

### Efficient Updates

```cpp
// Update only specific values (don't redraw entire screen)
lv_label_set_text_fmt(status_label, "Speed: %.1f", speed);

// Schedule update instead of immediate call
lv_timer_t *update_timer = lv_timer_create(update_callback, 500, NULL);
```

## Display Dimensions

```cpp
extern uint16_t TFT_WIDTH;   // Display width in pixels
extern uint16_t TFT_HEIGHT;  // Display height in pixels

// Common configurations
// 320x480 (portrait)
// 768x768 (square)
```

## Integration with Other Modules

### GPS Display
- Updates location in real time
- Shows satellite count and precision
- Displays current speed and altitude

### Activity Tracking
- Shows live metrics (HR, cadence, power)
- Updates distance and time
- Real-time lap data

### Map Display
- Renders GPS track overlay
- Shows waypoints from GPX files
- Manages zoom and pan gestures

### Storage Access
- Lists GPX files from SD card
- Shows file metadata (date, distance)
- Triggers file operations

## LVGL Configuration

See `lib/lvgl/lv_conf.h` for customization:

```cpp
#define LV_USE_TILEVIEW         1    // Enable tileview widget
#define LV_USE_CHART            1    // Enable charts
#define LV_USE_TEXTAREA         1    // Enable text input
#define LV_TICK_CUSTOM          1    // Custom tick provider
#define LV_COLOR_DEPTH         16    // 16-bit RGB565 color
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Screen blank | Check TFT initialization; verify LVGL tick task running |
| Buttons unresponsive | Verify touch callback registered; check GPIO pins |
| Memory overflow | Reduce object count; use object pooling |
| Slow updates | Check lv_timer_handler() in main loop |
| Text cut off | Adjust label width/height; use text wrapping |
| Colors wrong | Verify TFT byte order (RGB vs BGR); check color depth |

## See Also

- [TFT Module](../tft/README.md) - Display driver and LVGL initialization
- [Activity Module](../activity/README.md) - Activity data source
- [Maps Module](../maps/README.md) - Map visualization
- [Storage Module](../storage/README.md) - File operations
- [Bluetooth Module](../bluetooth/README.md) - Sensor data
- LVGL Documentation: https://docs.lvgl.io/
