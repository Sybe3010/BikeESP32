#include "gps.hpp"

GPS::GPS() {} // definieer constructor

void GPS::begin(unsigned long baud)
{
    // gebruik de meegegeven baud-parameter
    Serial2.begin(baud, SERIAL_8N1, GPS_SERIAL_RX_PIN, GPS_SERIAL_TX_PIN);

    simulateGPS = true; // Zet simulatie uit bij begin
}

/// @brief Verkrijg alle beschikbare gpsData.
/// @return geeft alle gps data terug in een GPSData struct.
GPS::GPSData GPS::getAllData(){
    if(simulateGPS){
        // Simuleer GPS-gegevens als er geen echte GPS-gegevens zijn  51.248304, 4.487703
        GPSData data;
        data.latitude = 51.248304;   // Simuleer breedtegraad (Sint-Jozefinstituut Schoten)
        data.longitude = 4.487703;   // Simuleer lengtegraad (Sint-Jozefinstituut Schoten)
        data.speed = 15.0;         // Simuleer snelheid in km/h
        data.heading = 30.0;       // Simuleer richting in graden
        data.altitude = 0.0;      // Simuleer hoogte in meters
        data.precisionH = 0.8;     // Simuleer HDOP-waarde
        data.satellites = 5;       // Simuleer aantal satellieten

        return data;

    } else {
        while (Serial2.available() > 0) {
            // cast naar char om signature overeen te laten komen
            gps.encode((char)Serial2.read());
        }
        GPSData data;

        data.latitude = gps.location.lat();
        data.longitude = gps.location.lng();
        data.speed = gps.speed.kmph();
        data.heading = gps.course.deg();
        data.altitude = gps.altitude.meters();
        data.precisionH = gps.hdop.hdop();
        data.satellites = gps.satellites.value();

        return data;
    }
}
