# Bluetooth Module

Manages Bluetooth Low Energy (BLE) sensor connections for heart rate monitors, cadence/speed sensors, and power meters. Provides real-time streaming of fitness data.

## Features

- **Automatic Sensor Detection**: Scans for standard BLE sensors
- **Multiple Simultaneous Connections**: Support for multiple BLE devices
- **Standard GATT Profiles**: Heart Rate, Cycling Speed/Cadence, Power Meter
- **Real-time Data Streaming**: Continuous notification handling
- **Connection Management**: Auto-reconnect and device pairing
- **Customizable Wheel Circumference**: Adjust cadence/speed calculations

## Supported Sensors

| Sensor Type | GATT UUID | Data Provided |
|-------------|-----------|---------------|
| Heart Rate Monitor | 180D | Heart rate (BPM) |
| Cycling Speed/Cadence | 1816 | Speed (km/h), Cadence (RPM) |
| Power Meter | 1818 | Power (watts) |

### Compatible Devices

- Garmin Edge (HRM, Speed/Cadence, Power)
- Wahoo sensors
- Sigma ROX
- 4iiii Precision
- Any ANT+ compatible sensor with BLE bridge

## API Reference

### BleClients Structure

```cpp
struct BleClients {
    BLEClient* _client;
    bool hr;              // Heart rate service available
    bool cadance;         // Cadence service available
    bool speed;           // Speed service available
    BLEAdvertisedDevice* targetDevice;
    
    uint32_t prevWheelRevs;      // Previous wheel revolutions
    uint16_t prevWheelTime;      // Previous wheel time
    uint16_t prevCrankRevs;      // Previous crank revolutions  
    uint16_t prevCrankTime;      // Previous crank time
    
    double lastSpeedKmh;         // Last calculated speed
    double lastCadanceRpm;       // Last calculated cadence
    double wheelCircumferenceMeters;  // Wheel size (default 2.105m)
};
```

### Main Methods

| Method | Description |
|--------|-------------|
| `Bluetooth()` | Constructor - initializes BLE subsystem |
| `init()` | Initialize BLE device and scanner |
| `startScan()` | Begin scanning for BLE devices (30 sec default) |
| `stopScan()` | Stop active scan |
| `connectToDevice(BLEAdvertisedDevice* device)` | Connect using BLEAdvertisedDevice pointer |
| `connectToDevice(std::string address)` | Connect using MAC address |
| `disconnectFromDevice(BLEClient* client)` | Disconnect by client pointer |
| `disconnectFromDevice(std::string address)` | Disconnect by device address |
| `getScanResults()` | Get scan results after scanning |

### Public Properties

```cpp
std::vector<BleClients> clients;  // List of connected clients
uint8_t hrValue;                   // Latest heart rate (BPM)
uint16_t cadanceValue;             // Latest cadence (RPM)
float speedValue;                  // Latest speed (km/h)
```

## Usage Examples

### Scan and Connect

```cpp
extern Bluetooth bleSensors;

// Start scanning for devices
bleSensors.startScan();
delay(30000);  // Wait 30 seconds for scan

// Get scan results
BLEScanResults results = bleSensors.getScanResults();
Serial.printf("Found %d devices\n", results.getCount());

// Connect to first device found
if (results.getCount() > 0) {
    BLEAdvertisedDevice device = results.getDevice(0);
    bleSensors.connectToDevice(&device);
}
```

### Read Sensor Data

```cpp
// Heart rate
Serial.printf("HR: %d BPM\n", bleSensors.hrValue);

// Cadence
Serial.printf("Cadence: %d RPM\n", bleSensors.cadanceValue);

// Speed
Serial.printf("Speed: %.2f km/h\n", bleSensors.speedValue);
```

### Custom Wheel Circumference

```cpp
// Set wheel circumference for accurate speed calculation
// Default: 2.105m (700x25c road bike tire)
// For 26" MTB: ~2.0m
// For 29" MTB: ~2.3m

if (bleSensors.clients.size() > 0) {
    bleSensors.clients[0].wheelCircumferenceMeters = 2.0;
}
```

## Sensor Data Streams

### GATT Notifications

The module automatically subscribes to:

- **HRM Service (180D)**:
  - Heart Rate Measurement (2A37)
  
- **CSC Service (1816)**:
  - CSC Measurement (2A5B) - Wheel revolutions and time
  - CSC Feature (2A5C) - Cumulative crank revolutions

- **Power Service (1818)**:
  - Power Measurement (2A63)

## Connection State Machine

```
SCANNING → DEVICE_FOUND → CONNECTING → CONNECTED
                                    ↓
                            IDLE (Data Streaming)
                                    ↓
                            DISCONNECT → DISCONNECTED
```

## Performance Notes

- **Scan Duration**: Default 30 seconds
- **Notification Frequency**: 1-4Hz depending on sensor
- **Memory**: ~100 bytes per connected device
- **Power**: BLE scan consumes ~10-15mA, idle ~1mA

## Data Calculation

### Speed from Wheel Revolutions

```
wheel_revolutions
circumference = 2.105 meters (default)
time_delta = current_time - prev_time

speed = (wheel_revolutions * circumference) / time_delta
```

### Cadence from Crank Revolutions

```
crank_revolutions
time_delta = current_time - prev_time

cadence = (60 * crank_revolutions) / time_delta  // RPM
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| No devices found | Ensure sensor is powered and in pairing mode |
| Connection fails | Verify sensor supports BLE (not ANT+ Connect) |
| No data received | Check if notifications are enabled on sensor |
| Speed incorrect | Verify wheel circumference matches tire size |
| Cadence jumps | Check sensor battery - low voltage causes errors |

## Integration Points

### In main.cpp

```cpp
#include "bluetooth.hpp"

extern Bluetooth bleSensors;

// In setup():
bleSensors.init();  // Initialize BLE

// In loop():
// Access current sensor values
Serial.printf("HR: %d, Cadence: %d RPM, Speed: %.1f km/h\n",
    bleSensors.hrValue,
    bleSensors.cadanceValue,
    bleSensors.speedValue);
```

### With Activity Recording

```cpp
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

## See Also

- [Activity Module](../activity/README.md) - Activity recording integration
- [GPS Module](../gps/README.md) - Location data
- Bluetooth SIG GATT Profiles: https://www.bluetooth.com/specifications/gatt/