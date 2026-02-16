#pragma once

#include <Arduino.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "tinyxml2.h"
#include "storage.hpp"
#include "globalGuiDef.h"
#include "bluetooth.hpp"
#include "gps.hpp"
#include "activity.hpp"

extern Storage storage;
extern GPS gps;
extern Bluetooth bleSensors;

static const char* gpxTrkHeader PROGMEM = { "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
					  "<gpx\n"
					  " version=\"1.1\"\n"
					  " creator=\"BikeESP32 by Sybe Van den Bergh\"\n"
					  " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/11.xsd\"\n"
					  " xmlns:ns3=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\"\n"
					  " xmlns=\"http://www.topografix.com/GPX/1/1\"\n"
					  " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ns2=\"http://www.garmin.com/xmlschemas/GpxExtensions/v3\">\n"
					  "<trk>\n"
					  "<name>BikeEsp Activity</name>\n"
					  "<type>cycling</type>\n"
					  "<trkseg>\n"
					  "</trkseg>\n"
					  "</trk>\n"
					  "</gpx>" };

static const char* AgpxTrackTag PROGMEM    = "trk";   /**< GPX track tag. */
static const char* AgpxTrackSegmentTag PROGMEM = "trkseg";/**< GPX track segment tag. */
static const char* AgpxTrackPointTag PROGMEM = "trkpt";/**< GPX track point tag. */
static const char* AgpxLatElem PROGMEM     = "lat";   /**< GPX latitude attribute. */
static const char* AgpxLonElem PROGMEM     = "lon";   /**< GPX longitude attribute. */
static const char* AgpxEleElem PROGMEM     = "ele";   /**< GPX elevation element. */

static const char* AgpxExtensionTag PROGMEM = "extensions"; /**< GPX extensions tag. */
static const char* AgpxTrackPointExtensionTag PROGMEM = "ns3:TrackPointExtension"; /**< GPX track point extension tag. */
static const char* AgpxHrElem PROGMEM = "ns3:hr"; /**< GPX temperature element. */
static const char* AgpxPowerElem PROGMEM = "ns3:power";


class Activity{
    public:
        struct ActivityPoint{
            float lon;
            float lat;
            float speed;
            float ele;
            int cadance;
            int hartrate;
            int power;
        };

        struct ActivityData{
            uint32_t timer;
            float distance;
            float avgSpeed;
            int avgCadance;
            int avgHR;
            int avgPower;
        };
        bool uiNeedsUpdate = false;

        Activity();
        Activity(const char* filePath);
        ~Activity(){}

        bool startActivity();
        bool addActivityPoint(const ActivityPoint& point);

        std::vector<ActivityPoint>& getActivityPoints();

        ActivityData getActivityData();

        bool stopActivity();
    private:
        std::string _activityFilePath;
        std::vector<ActivityPoint> _activityPoints;
        ActivityData totalData;
        bool isStarted = false;

        static void ActivityTimer(void *arg);
        bool createActivityFile();
        bool writeGpxData(const ActivityPoint& ap);
        void calculateActivityData();

        std::string formatFloat(float value, int precision);
};