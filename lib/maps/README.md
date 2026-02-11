# Maps Module

Renders map tiles on TFT display with pan, zoom, and GPS tracking. Displays recorded GPX tracks and waypoints with real-time position indicator.

## Features

- **Tile-Based Rendering**: Efficient rendering using pre-generated map tiles
- **Multi-level Zoom**: Zoom in/out from overview to street detail
- **GPS Tracking**: Center map on current position with optional heading indicator
- **Track Overlay**: Display recorded GPX tracks and waypoints
- **Pan and Scroll**: Manual map panning with smooth animation
- **Sprite Buffering**: Double-buffering for flicker-free updates
- **Memory Efficient**: Uses PSRAM for large tile buffers

## Tile System

### Tile Structure

Map tiles follow the standard Web Mercator projection (OpenStreetMap format):

```
/sdcard/MAP/
├── <zoom>/           # Zoom level 0-18
│   ├── <x>/          # Tile X coordinate
│   │   └── <y>.png   # Tile Y coordinate, 256x256 PNG
```

### Tile Coordinates

- **Zoom Level**: 0 (world) to 18 (street level)
  - Level 0: 1 tile covers entire world
  - Level 10: ~1km per tile
  - Level 18: ~3m per tile
  
- **X/Y Coordinates**: Grid position within zoom level
  - Max value: 2^zoom - 1
  - Level 10: 0-1023 for both X and Y

### Example Path

```
/sdcard/MAP/10/512/512.png   # Tile at zoom 10, grid position (512, 512)
```

## Sprite System

### Memory Layout

```cpp
// Full map buffer (not displayed)
TFT_eSprite mapTempSprite = TFT_eSprite(&tft);  // 768x768 pixels

// Visible map area (displayed on screen)  
TFT_eSprite mapSprite = TFT_eSprite(&tft);      // Display-sized sprites
```

### Tile Grid

The module loads a 3×3 grid of tiles (768×768):

```
[Tile(x,y)]   [Tile(x+1,y)]   [Tile(x+2,y)]
[Tile(x,y+1)] [Tile(x+1,y+1)] [Tile(x+2,y+1)]
[Tile(x,y+2)] [Tile(x+1,y+2)] [Tile(x+2,y+2)]
```

## API Reference

### Main Methods

| Method | Description |
|--------|-------------|
| `Maps()` | Constructor - initialize map system |
| `initMap(uint16_t width, uint16_t height)` | Set display dimensions |
| `generateMap(uint8_t zoom)` | Load and render tiles for zoom level |
| `displayMap()` | Show map on screen |
| `updateMap()` | Refresh map display |
| `centerOnGps(float lat, float lon)` | Center on coordinates |
| `getMapTile(float lon, float lat, uint8_t zoom, int8_t offsetX, int8_t offsetY)` | Get tile info |

### Public Properties

```cpp
uint8_t zoomLevel;              // Current zoom level (0-18)
int8_t tileX, tileY;           // Current tile grid position
int16_t offsetX, offsetY;       // Pixel offset within tiles
bool redrawMap;                 // Flag to trigger redraw
bool followGps;                 // Flag to auto-center on GPS
bool turnOnGpsHeading;          // Rotate map with GPS heading
uint16_t mapScrWidth;           // Display width in pixels
uint16_t mapScrHeight;          // Display height in pixels
std::vector<wayPoint> _waypoints; // Overlay waypoints
```

## Coordinate Conversion

### GPS to Screen Position

```cpp
// Get screen position for GPS coordinate
Maps::ScreenCoord screenPos = maps.coord2ScreenPos(
    gps.gpsData.longitude,  // GPS longitude
    gps.gpsData.latitude,   // GPS latitude  
    maps.zoomLevel,         // Current zoom
    256                     // Tile size (const)
);

// Screen position (x, y) in pixels on display
uint16_t screenX = screenPos.posX;
uint16_t screenY = screenPos.posY;
```

### Tile Calculation

```cpp
// Find tile containing GPS point at zoom level
uint32_t tileX = lon2tilex(longitude, zoomLevel);
uint32_t tileY = lat2tiley(latitude, zoomLevel);

// Find pixel position within tile
uint16_t posX = lon2posx(longitude, zoomLevel, 256);
uint16_t posY = lat2posy(latitude, zoomLevel, 256);
```

## Usage Examples

### Initialize Maps

```cpp
#include "maps.hpp"

extern Maps maps;

// In setup()
void setup() {
    maps.initMap(768, 768);  // Initialize with screen size
    maps.centerOnGps(gps.gpsData.latitude, gps.gpsData.longitude);
    maps.generateMap(13);    // Generate map at zoom level 13
}
```

### Center on GPS Position

```cpp
// Auto-follow mode
maps.followGps = true;
maps.centerOnGps(gps.gpsData.latitude, gps.gpsData.longitude);

// Manual pan
maps.offsetX += 50;  // Pan right by 50 pixels
maps.offsetY -= 30;  // Pan up by 30 pixels
maps.redrawMap = true;
```

### Change Zoom Level

```cpp
// Zoom in (fewer tiles, higher detail)
maps.zoomLevel = 14;
maps.generateMap(14);
maps.redrawMap = true;

// Zoom out (more tiles, wider view)
maps.zoomLevel = 12;
maps.generateMap(12);
maps.redrawMap = true;
```

### Add Waypoints

```cpp
// Load GPX and add waypoints to map
GpxParser gpxFile("/sdcard/TRK/route.gpx");
gpxFile.getWaypoints();

// Clear existing waypoints
maps._waypoints.clear();

// (Waypoints would be copied from GPX parser)
maps.redrawMap = true;
```

### Display with Track

```cpp
// In activity recording
void updateActivityMap() {
    // Center on current position
    maps.centerOnGps(gps.gpsData.latitude, gps.gpsData.longitude);
    
    // Update map display
    maps.displayMap();
    
    // Overlay current position with marker
    // (draw using TFT sprite methods)
}
```

## Zoom Level Guidelines

| Zoom | Coverage | Use Case |
|------|----------|----------|
| 8-10 | Regional (30-100km) | Route overview |
| 11-13 | Local (5-30km) | Ride tracking |
| 14-16 | Street (100m-1km) | Turn-by-turn navigation |
| 17-18 | Detailed (<100m) | Fine positioning |

## Performance Characteristics

- **Tile Load Time**: 50-200ms per tile (depends on SD speed)
- **Render Time**: 100-300ms (3×3 grid of 256px tiles)
- **Memory Usage**: ~2MB for 3×3 tile buffer (768×768 16-bit)
- **Update Rate**: 1-2 updates per second recommended

## Error Handling

### Missing Tile

When a tile file is not found at requested zoom/position:

```cpp
// Shows "NOMAP" placeholder from /spiffs/NOMAP.png
maps.isMapFound = false;
maps.showNoMap(mapSprite);
```

### Tile Bounds

```cpp
// Check if coordinate is within loaded tile bounds
tileBounds bounds = maps.totalBounds;

if (maps.isCoordInBounds(latitude, longitude, bounds)) {
    // Coordinate is visible on map
} else {
    // Need to pan/scroll
}
```

## Map Tile Sources

### Recommended Tile Providers

1. **OpenStreetMap**: https://tile.openstreetmap.org/
   - Free, open data
   - Multiple styles available
   
2. **Stamen Toner**: stamenbg-tiles.herokuapp.com
   - High contrast for outdoor use
   
3. **SRTM Hillshade**: 
   - Elevation visualization
   - Terrain detail

### Downloading Tiles

Many tools can generate tile sets:

```bash
# Using GDAL/OSGeo tools or dedicated tools like:
# - TileStache
# - Mapnik
# - QGIS (with appropriate plugins)

# Result structure must match:
/sdcard/MAP/<zoom>/<x>/<y>.png
```

## Touch/Gesture Support

The Maps module integrates with GUI touch handling:

```cpp
// Pan with finger swipe
void handle_pan_gesture(int delta_x, int delta_y) {
    maps.panMap(delta_x / 10, delta_y / 10);  // Convert to tile offset
    maps.redrawMap = true;
}

// Pinch-zoom detection handled by GUI module
void handle_zoom_gesture(bool zoom_in) {
    if (zoom_in && maps.zoomLevel < 18) {
        maps.zoomLevel++;
    } else if (!zoom_in && maps.zoomLevel > 0) {
        maps.zoomLevel--;
    }
    maps.generateMap(maps.zoomLevel);
    maps.redrawMap = true;
}
```

## Storage Requirements

### Typical Tile Counts

| Zoom | Area (km²) | Tile Count | Size (MB) |
|------|-----------|-----------|-----------|
| 10 | ~200 | ~10 | 2.5 |
| 12 | ~50 | ~40 | 10 |
| 14 | ~12 | ~160 | 40 |
| 16 | ~3 | ~640 | 160 |

Store only frequently-used regions to save SD space.

## Integration with Navigation

```cpp
// Display current route from GPX
void showNavigationMap() {
    // Load GPX track
    GpxParser route("/sdcard/TRK/nav_route.gpx");
    route.loadTrack();
    route.getWaypoints();
    
    // Copy waypoints to map overlay
    maps._waypoints = route.waypoints;
    
    // Center on start
    maps.centerOnGps(route.waypoints[0].latitude, 
                     route.waypoints[0].longitude);
    
    // Display
    maps.turnOnGpsHeading = true;  // Rotate view with heading
    maps.followGps = true;         // Follow current position
    maps.generateMap(14);          // Zoom for navigation
    maps.displayMap();
}
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Tiles not loading | Verify `/sdcard/MAP/` directory structure; check zoom level |
| Wrong coordinates | Verify tile coordinate system (Web Mercator); check GPS data |
| Map slow to update | Reduce tile grid size; use lower zoom level |
| Memory overflow | Reduce sprite size; implement tile streaming |
| Position marker offset | Verify screen-to-tile coordinate conversion |

## See Also

- [GPS Module](../gps/README.md) - Coordinates source
- [GPX Module](../gpx/README.md) - Track/waypoint data
- [TFT Module](../tft/README.md) - Display rendering
- [GUI Module](../gui/README.md) - Map screen integration
- Web Mercator: https://en.wikipedia.org/wiki/Web_Mercator_projection
- Slippy Map Tilenames: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
