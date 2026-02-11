# GPS Module

Parses GPS data from serial UART and provides real-time location, speed, heading, and altitude information. Uses TinyGPSPlus library for NMEA sentence parsing.

## Features

- **Real-time GPS Data**: Continuous position, speed, and altitude updates
- **Precision Metrics**: HDOP (horizontal precision) and satellite count
- **System Clock Sync**: Automatic device time synchronization with GPS
- **Simulation Mode**: Test without GPS hardware (optional)
- **Low Memory Footprint**: Efficient parsing without full position history

## Hardware Configuration

### Default Pin Assignment

```cpp
#define GPS_SERIAL_RX_PIN 9    // ESP32 GPIO9 (UART1 RX)
#define GPS_SERIAL_TX_PIN 14   // ESP32 GPIO14 (UART1 TX)
```

### Serial Protocol

- **Baud Rate**: 9600 (configurable)
- **Data Format**: 8N1 (8 bits, no parity, 1 stop bit)
- **Protocol**: NMEA 0183 standard sentences
- **Common Sentences**: RMC, GGA, GSA, GSV

## API Reference

### GPSData Structure

```cpp
struct GPSData {
    double latitude;      // Latitude in decimal degrees (-90 to 90)
    double longitude;     // Longitude in decimal degrees (-180 to 180)
    double speed;         // Speed over ground in km/h
    double heading;       // Course over ground in degrees (0-360)
    double altitude;      // Altitude above sea level in meters
    
    float precisionH;     // HDOP: horizontal dilution of precision
    uint8_t satellites;   // Number of satellites in view
};
```

### Main Methods

| Method | Description |
|--------|-------------|
| `GPS()` | Constructor - initializes serial and parsing |
| `begin(unsigned long baud)` | Initialize UART with specified baud rate |
| `setGpsTime()` | Sync system clock with GPS time |
| `getAllData()` | Retrieve current GPS data structure |

## Usage Examples

### Basic GPS Initialization

```cpp
#include "gps.hpp"

extern GPS gps;

// In setup():
void setup() {
    gps.begin(9600);  // Initialize with 9600 baud
}

// In loop():
void loop() {
    GPS::GPSData data = gps.getAllData();
    
    Serial.printf("Position: %.6f, %.6f\n", data.latitude, data.longitude);
    Serial.printf("Speed: %.2f km/h\n", data.speed);
    Serial.printf("Altitude: %.1f m\n", data.altitude);
    Serial.printf("Satellites: %d\n", data.satellites);
}
```

### Check GPS Signal Quality

```cpp
GPS::GPSData data = gps.getAllData();

// Check if we have a valid fix
if (data.satellites >= 4) {
    // Good fix - proceed with navigation
    Serial.println("GPS Ready");
} else if (data.satellites >= 3) {
    // Marginal fix - usable for basic positioning
    Serial.println("GPS Weak");
} else {
    // No fix - wait for satellites
    Serial.println("No GPS Signal");
}

// Check horizontal precision
if (data.precisionH < 2.5) {
    // Excellent accuracy (<2.5m)
} else if (data.precisionH < 5.0) {
    // Good accuracy (<5m)
} else {
    // Poor accuracy (>5m) - wait for more satellites
}
```

### Display Coordinates in Different Formats

```cpp
GPS::GPSData data = gps.getAllData();

// Decimal degrees (default)
Serial.printf("DMS: %.6f°, %.6f°\n", 
    data.latitude, data.longitude);

// Degrees, Minutes, Seconds
float lat = data.latitude;
float lon = data.longitude;

int lat_deg = (int)lat;
int lon_deg = (int)lon;
float lat_min = (lat - lat_deg) * 60;
float lon_min = (lon - lon_deg) * 60;

Serial.printf("DMS: %d°%.2f', %d°%.2f'\n",
    lat_deg, lat_min, lon_deg, lon_min);
```

## UART Configuration

### Hardware UART (Recommended)

ESP32 has multiple UART peripherals:

- **UART0**: GPIO1 (TX), GPIO3 (RX) - Reserved for USB serial
- **UART1**: GPIO10 (TX), GPIO9 (RX) - Default for GPS
- **UART2**: GPIO17 (TX), GPIO16 (RX) - Alternative

### Custom Pins

To use different pins, modify the `#define` statements:

```cpp
#define GPS_SERIAL_RX_PIN 16   // Alternative RX pin
#define GPS_SERIAL_TX_PIN 17   // Alternative TX pin
```

## NMEA Sentence Support

TinyGPSPlus automatically parses these standard sentences:

| Sentence | Content |
|----------|---------|
| RMC | Recommended Minimum Navigation Info (Position, speed, date) |
| GGA | Time, Position, Fix, Satellite count (used for altitude) |
| GSA | Satellites used, fix type, DOP values (HDOP) |
| GSV | Individual satellite signal strength |
| GLL | Geographic position (latitude/longitude) |
| VTG | Track and speed over ground |

## Performance Characteristics

- **Update Rate**: Default 1Hz (new NMEA sentence every 1 second)
- **Cold Start**: ~30-45 seconds to first fix (open sky)
- **Warm Start**: ~5-10 seconds (after recent use)
- **Hot Start**: <1 second (powered continuously)
- **Accuracy**: ±2.5m typical (depends on HDOP)
- **Serial Buffer**: Continuous parsing - no data loss

## GPS Simulation Mode

For testing without GPS hardware:

1. Uncomment in `platformio.ini`:
   ```ini
   build_flags = 
       -D GPS_SIMULATOR
   ```

2. Simulated data will be generated internally
3. Set breakpoints to verify calculations without hardware

## Data Validation

Always check data validity before use:

```cpp
GPS::GPSData data = gps.getAllData();

// Validate satellite count
if (data.satellites < 3) {
    Serial.println("Insufficient satellites for fix");
    return;
}

// Validate HDOP (lower is better)
if (data.precisionH > 10.0) {
    Serial.println("Poor precision - HDOP too high");
    return;
}

// Validate coordinates (0,0 usually means no fix)
if (data.latitude == 0 && data.longitude == 0) {
    Serial.println("No position fix yet");
    return;
}

// Safe to use data
```

## Integration with Activity Recording

```cpp
// In activity recording loop:
Activity::ActivityPoint point = {
    .lon = gps.gpsData.longitude,
    .lat = gps.gpsData.latitude,
    .speed = gps.gpsData.speed,
    .ele = gps.gpsData.altitude,
    .cadance = bleSensors.cadanceValue,
    .hartrate = bleSensors.hrValue,
    .power = 0
};
currentActivity.addActivityPoint(point);
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| No signal acquisition | Ensure antenna is attached; wait 30+ seconds in open sky |
| Coordinate fluctuates | HDOP > 5; wait for more satellites (satellite count ≥ 6) |
| Altitude wrong | Common in urban canyons; requires clear sky view |
| Speed jumps | Likely GPS noise; smooth data with moving average |
| Time not syncing | Verify `setGpsTime()` called; check system RTC |
| Partial NMEA data | Serial buffer full; verify baud rate correct |

## Wiring Example

```
GPS Module ──→ ESP32

GND (Black)  ──→ GND
5V (Red)     ──→ 5V
TX (Yellow)  ──→ GPIO9 (RX1)
RX (Green)   ──→ GPIO14 (TX1)
```

**Note**: Some breadboards require series resistor on RX line if using 5V GPS.

## See Also

- [Activity Module](../activity/README.md) - Uses GPS data for tracking
- [Maps Module](../maps/README.md) - Uses coordinates for map display
- TinyGPSPlus: https://github.com/mikalhart/TinyGPSPlus
- NMEA 0183 Standard: https://www.nmea.org/