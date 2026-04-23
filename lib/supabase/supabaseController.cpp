#include "supabaseController.hpp"

SupabaseController::SupabaseController() {
}



void SupabaseController::initSupabase() {
    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
        if (event == ARDUINO_EVENT_WIFI_STA_DISCONNECTED) {
            lastDisconnectReason = info.wifi_sta_disconnected.reason;
        }
    });

    log_i("Connecting to WiFi...");
    lastDisconnectReason = WIFI_REASON_UNSPECIFIED;

    WiFi.begin(ssid, wifiPass);

    connectStart = millis();
    wifiConnecting = true;
}

void SupabaseController::updateWifi() {
    if(WiFi.status() != WL_CONNECTED){
        isConnectedToWifi = false;
    }
    if (!wifiConnecting) return;

    // Verbonden
    if (WiFi.status() == WL_CONNECTED) {
        log_i("Connected to WiFi");
        wifiConnecting = false;
        isConnectedToWifi = true;

        supabase.begin(supabase_url, supabase_key);
        supabase.login_email(email, password);
        return;
    }

    // Timeout na 8 seconden
    if (millis() - connectStart > 100000) {
        wifiConnecting = false;
        isConnectedToWifi = false;

        if (lastDisconnectReason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT ||
            lastDisconnectReason == WIFI_REASON_AUTH_FAIL) {
            log_e("Wrong WiFi password!");
        } else if (lastDisconnectReason == WIFI_REASON_NO_AP_FOUND) {
            log_e("Network not found!");
        } else {
            log_e("WiFi connection failed, reason: %d", lastDisconnectReason);
        }
    }
}

void SupabaseController::uploadGpxFile(String filePath) {
    FILE* file = storage.open(filePath.c_str(), "r");
    if (!file) {
        Serial.println("Error: Could not open file");
        return;
    }
    
    uint32_t fileSize = storage.size(filePath.c_str());
    Serial.printf("Uploading file: %s (size: %u bytes)\n", filePath.c_str(), fileSize);
    
    if (fileSize == 0) {
        Serial.println("Error: File is empty");
        storage.close(file);
        return;
    }
    
    FileStream fileStream(file);
    
    // Extract filename from path
    int lastSlash = filePath.lastIndexOf('/');
    String fileName = (lastSlash != -1) ? filePath.substring(lastSlash + 1) : filePath;
    
    Serial.printf("Uploading as: %s\n", fileName.c_str());
    
    int result = supabase.upload(bucketName, fileName, "application/gpx+xml", &fileStream, fileSize);
    
    Serial.printf("Upload result: %d\n", result);
    storage.close(file);
}

void SupabaseController::downloadGpxFile(String fileName, String destinationPath) {
    
}

