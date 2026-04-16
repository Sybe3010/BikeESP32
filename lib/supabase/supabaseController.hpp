#pragma once

#include <Arduino.h>
#include <ESPSupabase.h>
#include <WiFi.h>

#include "storage.hpp"

extern Storage storage;

class SupabaseController {
    public:
        SupabaseController();
        void initSupabase();

        void uploadGpxFile(String filePath);
        void deleteGpxFile(String filePath);
        void downloadGpxFile(String fileName, String destinationPath);
    private:
        Supabase supabase;
        String supabase_url = "https://dokobfqjtjmudsmfzfkj.supabase.co";
        String supabase_key = "sb_publishable_8V4pupYU6-24Un-eadtfEg_sl2f35E5";

        const char* ssid = "Labo-ICT";
        const char* wifiPass = "IICT618216";

        const String email = "sybevandenbergh@outlook.com";
        const String password = "SVdB2008";

        const String bucketName = "routeFiles";
};