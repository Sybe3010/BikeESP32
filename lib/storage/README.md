# Storage Module

Manages file I/O operations for both SD card and internal SPIFFS (SPI Flash File System). Handles activity files, map tiles, and configuration storage.

## Features

- **Dual Storage**: SD card (external) and SPIFFS (internal)
- **File Operations**: Create, read, write, delete, and list files
- **Directory Management**: Create and navigate directory structures
- **SD Card Info**: Query capacity, free space, and card details
- **Stream Interface**: File access via standard C++ Stream API
- **Error Handling**: Comprehensive error checking and status reporting

## Storage Hierarchy

### SD Card `/sdcard/`
```
/sdcard/
├── MAP/              # Map tiles
│   ├── 10/
│   │   ├── 512/
│   │   │   └── 512.png
│   │   └── ...
│   └── ...
├── TRK/              # GPX track files (completed activities)
│   ├── ACT_20250211_143022.gpx
│   └── ...
├── ACT/              # Activity data and metadata
│   └── ...
└── TouchCal          # Touch calibration file
```

### SPIFFS `/spiffs/`
```
/spiffs/
├── NOMAP.png         # Fallback image when tiles missing
├── config.txt        # Device configuration
└── ...
```

## API Reference

### SDCardInfo Structure

```cpp
struct SDCardInfo {
    std::string name;         // Card name identifier
    std::string capacity;     // Total capacity (formatted string)
    int sector_size;          // Bytes per sector
    int read_block_len;       // Read block size
    std::string card_type;    // Type (SDHC, SDXC, etc.)
    std::string total_space;  // Total space (e.g., "32GB")
    std::string free_space;   // Available space (formatted)
    std::string used_space;   // Used space (formatted)
};
```

### FileStream Class

```cpp
class FileStream : public Stream {
    public:
        FileStream(FILE* file);       // Constructor with FILE*
        
        // Stream interface
        int available() override;     // Bytes available to read
        int read() override;          // Read one byte
        int peek() override;          // Peek next byte
        void flush() override;        // Flush write buffer
        size_t write(uint8_t c);     // Write one byte
};
```

### Storage Class Methods

| Method | Purpose | Returns |
|--------|---------|---------|
| `initSD()` | Initialize SD card | `bool` success |
| `initSPIFFS()` | Initialize internal storage | `bool` success |
| `exists(path)` | Check if file/directory exists | `bool` |
| `mkdir(path)` | Create directory | `bool` success |
| `remove(path)` | Delete file or directory | `bool` success |
| `listDir(path)` | List directory contents | `std::vector<std::string>` |
| `getSDCardInfo()` | Get SD card information | `SDCardInfo` struct |
| `getFile(path)` | Get file pointer | `FILE*` |
| `getFileSize(path)` | Get file size in bytes | `size_t` |

## Usage Examples

### Initialize Storage

```cpp
#include "storage.hpp"

extern Storage storage;

// In setup()
void setup() {
    // Initialize SD card
    if (!storage.initSD()) {
        Serial.println("SD card initialization failed");
        return;
    }
    
    // Initialize internal SPIFFS
    if (!storage.initSPIFFS()) {
        Serial.println("SPIFFS initialization failed");
        return;
    }
    
    // Create necessary directories
    if (!storage.exists("/sdcard/TRK")) {
        storage.mkdir("/sdcard/TRK");
    }
    
    if (!storage.exists("/sdcard/ACT")) {
        storage.mkdir("/sdcard/ACT");
    }
}
```

### Create and Write File

```cpp
// Create GPX file
const char* filepath = "/sdcard/TRK/ACT_20250211_143022.gpx";

FILE* file = fopen(filepath, "w");
if (file == NULL) {
    Serial.println("Failed to create file");
    return;
}

// Write GPX header
fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
fprintf(file, "<gpx version=\"1.1\">\n");

// Write data
fprintf(file, "<trkpt lat=\"51.252376\" lon=\"4.438024\">\n");
fprintf(file, "<ele>10.5</ele>\n");
fprintf(file, "</trkpt>\n");

// Close file
fclose(file);
```

### Read File Contents

```cpp
// Read entire file
const char* filepath = "/sdcard/TRK/activity.gpx";

FILE* file = fopen(filepath, "r");
if (file == NULL) {
    Serial.println("File not found");
    return;
}

// Read line by line
char buffer[256];
while (fgets(buffer, sizeof(buffer), file) != NULL) {
    Serial.print(buffer);  // Process line
}

fclose(file);
```

### List Directory Contents

```cpp
// List all GPX files
std::vector<std::string> files = storage.listDir("/sdcard/TRK");

for (const auto& filename : files) {
    Serial.printf("Found file: %s\n", filename.c_str());
    
    // Get file size
    std::string filepath = "/sdcard/TRK/" + filename;
    size_t size = storage.getFileSize(filepath.c_str());
    Serial.printf("  Size: %d bytes\n", size);
}
```

### Check SD Card Information

```cpp
// Get SD card details
SDCardInfo cardInfo = storage.getSDCardInfo();

Serial.printf("Card: %s\n", cardInfo.name.c_str());
Serial.printf("Capacity: %s\n", cardInfo.capacity.c_str());
Serial.printf("Card Type: %s\n", cardInfo.card_type.c_str());
Serial.printf("Total Space: %s\n", cardInfo.total_space.c_str());
Serial.printf("Free Space: %s\n", cardInfo.free_space.c_str());
Serial.printf("Used Space: %s\n", cardInfo.used_space.c_str());
```

### Stream-Based File I/O

```cpp
// Create FileStream for compatible code
FILE* file = fopen("/sdcard/data.txt", "r");
if (file != NULL) {
    FileStream stream(file);
    
    // Use as Stream
    if (stream.available() > 0) {
        int byte = stream.read();
    }
    
    fclose(file);
}
```

## File Paths and Naming

### Absolute Paths

All paths are absolute from filesystem root:

```cpp
"/sdcard/MAP/10/512/512.png"      // SD card absolute path
"/spiffs/config.txt"               // SPIFFS absolute path
```

### Activity Files

Activities use timestamp format:

```
ACT_YYYYMMDD_HHMMSS.gpx

Example: ACT_20250211_143022.gpx
  Date:  2025-02-11
  Time:  14:30:22
```

### Map Tile Paths

Standard tile naming:

```
/sdcard/MAP/<zoom>/<x>/<y>.png

Example: /sdcard/MAP/12/2048/1024.png
  Zoom:  12
  X:     2048
  Y:     1024
```

## SD Card Specifications

### Required Format

- **File System**: FAT32 or exFAT
- **Partition**: Single primary partition
- **Speed**: Class 10 or higher recommended
- **Capacity**: 4GB to 2TB

### Typical Speeds

| Operation | Speed |
|-----------|-------|
| Sequential read | ~25 MB/s |
| Sequential write | ~15 MB/s |
| Random read | ~5 MB/s |
| Random write | ~2 MB/s |

### Performance Tips

- Preallocate files to reduce fragmentation
- Write in blocks (avoid single-byte writes)
- Close files promptly to flush buffers

## SPIFFS (Internal Storage)

### Specifications

- **Capacity**: ~4MB usable (depends on partition)
- **Page Size**: 4096 bytes
- **Block Size**: 65536 bytes
- **Read Speed**: ~10 MB/s
- **Write Speed**: ~5 MB/s

### Use Cases

- Configuration files
- Calibration data
- Fallback images (NOMAP.png)
- Small lookup tables

## Error Handling

### File Operations

```cpp
// Always check return values
const char* filename = "/sdcard/TRK/activity.gpx";

if (!storage.exists(filename)) {
    Serial.printf("File not found: %s\n", filename);
    return;
}

FILE* file = fopen(filename, "r");
if (file == NULL) {
    Serial.printf("Cannot open file: %s\n", filename);
    perror("fopen");
    return;
}

// Process file...

if (fclose(file) != 0) {
    Serial.println("Error closing file");
}
```

### Directory Operations

```cpp
// Create directory with error checking
const char* dirpath = "/sdcard/TRK";

if (storage.exists(dirpath)) {
    Serial.println("Directory already exists");
} else if (storage.mkdir(dirpath)) {
    Serial.println("Directory created successfully");
} else {
    Serial.println("Failed to create directory");
}
```

## Data Logging Example

### GPS Track Logging

```cpp
// Open activity file for writing
std::string filename = "/sdcard/TRK/activity.gpx";
FILE* trackFile = fopen(filename.c_str(), "w");

// Write GPX header once
fprintf(trackFile, "<?xml version=\"1.0\"?>\n<gpx><trkseg>\n");

// Log GPS points in loop
void logGPSPoint(FILE* file, const GPS::GPSData& data) {
    fprintf(file, "<trkpt lat=\"%.6f\" lon=\"%.6f\">\n",
        data.latitude, data.longitude);
    fprintf(file, "<ele>%.1f</ele>\n", data.altitude);
    fprintf(file, "</trkpt>\n");
    fflush(file);  // Flush buffer immediately
}

// Close file when done
fprintf(trackFile, "</trkseg></gpx>\n");
fclose(trackFile);
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| SD card not detected | Check power supply; verify card inserted properly |
| SPIFFS read errors | Try to reformat SPIFFS (loses data) |
| File write timeout | Verify SD card not full; check write speed |
| Directory not found | Use `mkdir()` to create; check path spelling |
| Permission denied | Verify file not read-only; check directory permissions |
| Corrupted files | Use SD card checker tools; reformat if needed |

## Integration with Modules

### Activity Recording

```cpp
// Activity module creates and writes GPX files
// Uses Storage::getFile() for efficient I/O
FILE* gpxFile = storage.getFile(activityPath);
```

### Map Tiles

```cpp
// Maps module reads tile PNG files
// Checks storage.exists() before accessing tile
if (storage.exists(tilePath)) {
    // Load tile image
}
```

### GPX Parsing

```cpp
// GPX module lists and reads track files
std::vector<std::string> files = storage.listDir("/sdcard/TRK");
```

## See Also

- [Activity Module](../activity/README.md) - Creates activity files
- [Maps Module](../maps/README.md) - Reads map tiles
- [GPX Module](../gpx/README.md) - Reads GPX files
- ESP32 SPIFFS Documentation: https://docs.espressif.com/projects/esp-idf/
- SD Card Specification: https://www.sdcard.org/
