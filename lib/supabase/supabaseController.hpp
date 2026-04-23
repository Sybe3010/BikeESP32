#pragma once

#include <Arduino.h>
#include <ESPSupabase.h>
#include <WiFi.h>

#include "esp_wifi.h"   // voor wifi_err_reason_t
#include "storage.hpp"

extern Storage storage;

class SupabaseController {
public:
    SupabaseController();

    // Start WiFi + Supabase login (niet-blokkerend)
    void initSupabase();

    // Wordt regelmatig aangeroepen in loop()
    void updateWifi();

    void uploadGpxFile(String filePath);
    void deleteGpxFile(String filePath);
    void downloadGpxFile(String fileName, String destinationPath);

    bool isConnectedToWifi = false;

private:
    Supabase supabase;

    String supabase_url = "https://dokobfqjtjmudsmfzfkj.supabase.co";
    String supabase_key = "sb_publishable_8V4pupYU6-24Un-eadtfEg_sl2f35E5";

    const char* ssid = "Labo-ICT";
    const char* wifiPass = "IICT618216 ";

    const String email = "sybevandenbergh@outlook.com";
    const String password = "SVdB2008";

    const String bucketName = "routeFiles";

    // WiFi connectie status
    unsigned long connectStart = 0;
    bool wifiConnecting = false;

    // Reden van disconnect (ESP32 event)
    int lastDisconnectReason = WIFI_REASON_UNSPECIFIED;
};
