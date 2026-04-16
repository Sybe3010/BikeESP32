#include "supabaseController.hpp"

SupabaseController::SupabaseController() {
}

void SupabaseController::initSupabase() {
    WiFi.begin(ssid, wifiPass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    supabase.begin(supabase_url, supabase_key);

    supabase.login_email(email, password);
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

