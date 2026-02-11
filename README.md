# BikeESP32

An open-source GPS-enabled bike computer built on the ESP32 microcontroller. BikeESP32 records cycling activities with real-time GPS tracking, displays maps and route information on a TFT screen, and integrates with Bluetooth sensors for heart rate, cadence, and power measurements.

## Features

- **GPS Tracking**: Real-time positioning and activity recording with elevation data (see [GPS documentation](lib/gps/README.md))
- **Bluetooth Sensor Integration**: Connect wireless heart rate, cadence, and power sensors (see [Bluetooth documentation](lib/bluetooth/README.md))
- **TFT Display**: Interactive touchscreen interface with multiple screens (see [GUI documentation](lib/gui/README.md))
- **Map Visualization**: On-device map rendering with tile-based system and zoom control (see [Maps documentation](lib/maps/README.md))
- **Activity Recording**: Automatic logging of cycling activities in GPX format (see [Activity documentation](lib/activity/README.md))
- **LVGL GUI Framework**: Responsive and customizable user interface
- **Storage**: SD card support for map tiles and activity storage, plus internal SPIFFS (see [Storage documentation](lib/storage/README.md))

## Hardware Requirements

- **ESP32** development board with PSRAM
- **TFT Display**: Compatible with LovyanGFX library
- **GPS Module**: Serial UART compatible (default: RX on GPIO 9, TX on GPIO 14)
- **Optional Sensors**: Bluetooth HR/Cadence/Power sensors
- **Storage**: Micro SD card for map tiles and activity data

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) IDE or CLI
- Arduino IDE with PlatformIO extension
- USB cable for programming the ESP32

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Sybe3010/BikeESP32.git
   cd BikeESP32
   ```

2. **Open with PlatformIO**:
   - Open the project folder in VS Code with PlatformIO extension installed
   - Or use PlatformIO CLI: `pio open`

3. **Build and Upload**:
   ```bash
   pio run -t upload
   ```
   The build will automatically handle:
   - Downloading required libraries (LovyanGFX, LVGL, TinyGPSPlus, tinyxml2)
   - Configuring LVGL with custom settings
   - Setting ESP32-specific compiler flags

### First Run

1. Insert a micro SD card with pre-generated map tiles (directory structure: `/MAP/<zoom>/<x>/<y>.png`)
2. Power on the device
3. The device will initialize SD card, SPIFFS, and create necessary directories (`/sdcard/TRK` and `/sdcard/ACT`)
4. The home screen will display once all systems are ready
5. Wait for GPS lock (shown in status bar)
6. Pair and connect Bluetooth sensors from the Bluetooth screen
7. Create and record a new activity

## Project Structure

```
BikeESP32/
├── src/
│   └── main.cpp              # Main initialization and loop
├── lib/
│   ├── activity/             # Activity recording and GPX export
│   ├── bluetooth/            # BLE sensor connections
│   ├── gps/                  # GPS parsing and data management
│   ├── gpx/                  # GPX file handling
│   ├── gui/                  # LVGL screens and UI components
│   ├── maps/                 # Map rendering and tile management
│   ├── storage/              # SD card and SPIFFS operations
│   ├── tft/                  # Display initialization
│   └── lvgl/                 # LVGL configuration
├── include/                  # Header includes
└── platformio.ini            # PlatformIO configuration
```

## Key Libraries

| Library | Purpose | Version |
|---------|---------|---------|
| [LovyanGFX](https://github.com/lovyan03/LovyanGFX) | Display graphics engine | 1.2.7 |
| [LVGL](https://lvgl.io/) | GUI framework | 9.2.2 |
| [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) | GPS data parsing | 1.1.0 |
| [tinyxml2](https://github.com/leethomason/tinyxml2) | GPX file generation | Latest |

## Module Documentation

Each module has dedicated documentation. Refer to these for detailed implementation, API usage, and customization:

| Module | Purpose | Documentation |
|--------|---------|-----------------|
| **Activity** | Activity recording and GPX export | [README](lib/activity/README.md) |
| **Bluetooth** | BLE sensor connections and data streaming | [README](lib/bluetooth/README.md) |
| **GPS** | GPS parsing and real-time positioning | [README](lib/gps/README.md) |
| **GPX** | GPX file format handling | [README](lib/gpx/README.md) |
| **GUI** | LVGL screens and user interface | [README](lib/gui/README.md) |
| **Maps** | Map rendering and tile management | [README](lib/maps/README.md) |
| **Storage** | SD card and SPIFFS file operations | [README](lib/storage/README.md) |
| **TFT** | Display initialization and graphics | [README](lib/tft/README.md) |

## Hardware Notes

- **PSRAM Required**: The device uses external PSRAM for map rendering and GUI buffers
- **USB CDC**: Serial communication enabled for debugging and updates
- **Touchscreen Support**: Some TFT displays support capacitive touch (configured in TFT initialization)

## Contributing

We welcome contributions! Please:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature`
3. Commit changes: `git commit -am 'Add feature'`
4. Push to branch: `git push origin feature/your-feature`
5. Submit a pull request

Please ensure code follows the existing style and includes comments for complex logic.

## Maintainer

**Sybe Van den Bergh**
- GitHub: [@sybevdb](https://github.com/sybevdb)

## License

This project is open source. See LICENSE file for details.

## Support

- **GitHub Issues**: Report bugs and request features [here](https://github.com/Sybe3010/BikeESP32/issues)
- **Module Documentation**: See [Module Documentation](#module-documentation) section above for detailed guides on each component
- **Serial Debugging**: Connect via USB and monitor serial output at 9600 baud
- **Contributing**: See [Contributing](#contributing) section below

## Acknowledgments

- [LovyanGFX](https://github.com/lovyan03/LovyanGFX) - Display graphics library by Lovyan03
- [LVGL](https://lvgl.io/) - Open source embedded GUI library
- [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) - GPS parsing by Mikal Hart
- [PlatformIO](https://platformio.org/) - Professional development platform

## Specifications

- **Microcontroller**: ESP32 (Xtensa dual-core 32-bit)
- **Display**: 320×480 TFT LCD
- **Storage**: 4MB SPIFFS + Micro SD card support
- **Connectivity**: Bluetooth 5.0 LE, GPS (UART)
- **Power**: USB-C (when available)

---

**Happy cycling!** 🚴
