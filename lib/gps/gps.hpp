#pragma once

#include <Arduino.h>
#include <TinyGPSPlus.h>

#define GPS_SERIAL_RX_PIN 9
#define GPS_SERIAL_TX_PIN 14

class GPS
{
    private:
        TinyGPSPlus gps;
        bool simulateGPS = false;
    public:
        struct GPSData
        {
            double latitude;
            double longitude;
            double speed; // in km/h
            double heading; // in degrees
            double altitude; // in meters

            float precisionH; // HDOP: horizontal precision
            uint8_t satellites;
        } gpsData;

        GPS();
        void begin(unsigned long baud);
        GPSData getAllData();
        GPSData SimulateByGPXTrack();
};
