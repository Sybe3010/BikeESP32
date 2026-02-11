# Activity Module

Records cycling activities with GPS, heart rate, cadence, and power data. Exports activities to GPX format (1.1) compatible with Strava, Garmin, and other fitness platforms.

## Features

- **Activity Recording**: Start/stop activity tracking with automatic GPS point logging
- **Real-time Data**: Captures speed, elevation, heart rate, cadence, and power metrics
- **GPX Export**: Generates GPX 1.1 files with Garmin TrackPoint extensions
- **Statistics**: Automatic calculation of distance, average speed, average cadence, HR, and power
- **File Management**: Save activities to SD card with timestamped filenames
- **Live Updates**: LVGL timer-based UI updates during activity recording

## API Reference

### ActivityPoint Structure

```cpp
struct ActivityPoint {
    float lon;        // Longitude in degrees
    float lat;        // Latitude in degrees
    float speed;      // Speed in km/h
    float ele;        // Elevation in meters
    int cadance;      // Cadence in RPM
    int hartrate;     // Heart rate in BPM
    int power;        // Power in watts
};
```

### ActivityData Structure

```cpp
struct ActivityData {
    uint16_t timer;          // Elapsed time in seconds
    float distance;          // Total distance in meters
    float avgSpeed;          // Average speed in km/h
    uint16_t avgCadance;     // Average cadence in RPM
    uint8_t avgHR;           // Average heart rate in BPM
    uint16_t avgPower;       // Average power in watts
};
```

### Main Methods

| Method | Description |
|--------|-------------|
| `Activity()` | Default constructor - creates new activity |
| `Activity(const char* filePath)` | Load existing activity from file |
| `startActivity()` | Initialize and begin recording |
| `addActivityPoint(const ActivityPoint& point)` | Add GPS point with sensor data |
| `getActivityPoints()` | Retrieve all recorded points |
| `getActivityData()` | Get aggregated activity statistics |
| `stopActivity()` | Finalize and save GPX file |

## Usage Examples

### Start and Record Activity

```cpp
extern Activity currentActivity;

// Start new activity
if (!currentActivity.startActivity()) {
    Serial.println("Failed to start activity");
    return;
}

// Recording loop
void recordingLoop() {
    Activity::ActivityPoint point = {
        .lon = gps.gpsData.longitude,
        .lat = gps.gpsData.latitude,
        .speed = gps.gpsData.speed,
        .ele = gps.gpsData.altitude,
        .cadance = bleSensors.cadanceValue,
        .hartrate = bleSensors.hrValue,
        .power = 0  // Will be set if power meter available
    };
    
    currentActivity.addActivityPoint(point);
}

// Stop and save
currentActivity.stopActivity();
```

### Access Activity Data

```cpp
Activity::ActivityData stats = currentActivity.getActivityData();

Serial.printf("Distance: %.2f m\n", stats.distance);
Serial.printf("Avg Speed: %.2f km/h\n", stats.avgSpeed);
Serial.printf("Avg HR: %d BPM\n", stats.avgHR);
Serial.printf("Time: %d seconds\n", stats.timer);
```

## File Structure

Activities are saved to `/sdcard/ACT/` with filenames:
- Format: `ACT_<YYYYMMDD>_<HHMMSS>.gpx`
- Example: `ACT_20250211_143022.gpx`

## GPX Format

Generated GPX files include:

- **Track metadata**: Activity name, type (cycling), creator attribution
- **Track points**: Latitude, longitude, elevation
- **Extensions**: Heart rate, power (Garmin namespace)
- **Version**: 1.1 (standard GPX format)

### Sample GPX Structure
```xml
<?xml version="1.0" encoding="UTF-8"?>
<gpx version="1.1" creator="BikeESP32 by Sybe Van den Bergh"
     xmlns:ns3="http://www.garmin.com/xmlschemas/TrackPointExtension/v1"
     xmlns="http://www.topografix.com/GPX/1/1">
  <trk>
    <name>BikeEsp Activity</name>
    <type>cycling</type>
    <trkseg>
      <trkpt lat="51.252376" lon="4.438024">
        <ele>10.0</ele>
        <extensions>
          <ns3:TrackPointExtension>
            <ns3:hr>120</ns3:hr>
            <ns3:power>200</ns3:power>
          </ns3:TrackPointExtension>
        </extensions>
      </trkpt>
    </trkseg>
  </trk>
</gpx>
```

## Statistics Calculation

- **Distance**: Calculated using Haversine formula between consecutive points (includes elevation)
- **Average Speed**: Total distance / elapsed time
- **Average Metrics**: Mean of HR, cadence, and power over activity duration

## Data Dependencies

- **GPS Module**: Provides location and altitude data
- **Bluetooth Module**: Provides HR, cadence, and power (optional)
- **Storage Module**: Handles file I/O to SD card
- **LVGL**: Timer-based UI updates during recording

## Integration Points

### In main.cpp

```cpp
#include "activity.hpp"

// Create global activity instance
Activity currentActivity;

// In setup():
// Activity initialization handled automatically

// In loop():
// Add points regularly from GPS data
if (gps.gpsData.latitude != 0) {
    Activity::ActivityPoint point = { /* ... */ };
    currentActivity.addActivityPoint(point);
}
```

## Performance Notes

- **Memory**: Activity stores points in std::vector (≈ 28 bytes per point)
- **Timing**: GPX write operation takes ~10-50ms depending on buffer size
- **Updates**: LVGL timer updates UI at ~10Hz to avoid blocking

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Activity won't start | Check SD card is initialized and `/sdcard/ACT` exists |
| Points not recording | Verify GPS has valid fix (check `gpsData.satellites > 3`) |
| File not saving | Ensure sufficient SD card space and write permissions |
| Incorrect distance | Verify GPS altitude data is valid (ele > 0) |

## See Also

- [GPS Module](../gps/README.md) - GPS data provider
- [Bluetooth Module](../bluetooth/README.md) - Sensor data provider
- [Storage Module](../storage/README.md) - File operations
- [GPX Module](../gpx/README.md) - File format details
