#pragma once

#include <Arduino.h>
#include <TinyGPSPlus.h>

#define GPS_SERIAL_RX_PIN 9
#define GPS_SERIAL_TX_PIN 14

class GPS
{
    private:
        TinyGPSPlus gps; //  TinyGPSPlus object voor GPS data parsing
        bool simulateGPS = false; //  Schakel GPS simulatie in/uit
    public:
        struct GPSData // Structure om alle GPS data te bevatten
        {
            double latitude; // in degrees
            double longitude; // in degrees
            double speed; // in km/h
            double heading; // in degrees
            double altitude; // in meters

            float precisionH; // HDOP: horizontal precision
            uint8_t satellites;
        } gpsData;

        GPS();
        void begin(unsigned long baud); // Initialiseer de GPS module met de opgegeven baudrate
        GPSData getAllData(); // Haal alle GPS data op
};
