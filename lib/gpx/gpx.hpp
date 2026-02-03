#pragma once

#include <Arduino.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <dirent.h> 
#include "esp_log.h"
#include "tinyxml2.h"
#include "globalGpxDef.h"

static const char* gpxWaypointTag PROGMEM = "wpt";   /**< GPX waypoint tag. */
static const char* gpxTrackTag PROGMEM    = "trk";   /**< GPX track tag. */
static const char* gpxTrackSegTag PROGMEM = "trkseg";/**< GPX track segment tag. */
static const char* gpxTrackPtTag PROGMEM  = "trkpt"; /**< GPX track point tag. */
static const char* gpxNameElem PROGMEM    = "name";  /**< GPX name element. */
static const char* gpxLatElem PROGMEM     = "lat";   /**< GPX latitude attribute. */
static const char* gpxLonElem PROGMEM     = "lon";   /**< GPX longitude attribute. */
static const char* gpxEleElem PROGMEM     = "ele";   /**< GPX elevation element. */
static const char* gpxTimeElem PROGMEM    = "time";  /**< GPX time element. */
static const char* gpxDescElem PROGMEM    = "desc";  /**< GPX description element. */
static const char* gpxSrcElem PROGMEM     = "src";   /**< GPX source element. */

class GpxParser {
    public:
        GpxParser() {} /// Default constructor
        GpxParser(const char* filePath); /// constructor met een file pad, dit voor het inladen van een specifiek gpx bestand
        ~GpxParser();

        // functie die alle gpx bestanden in een map ophaalt en de bestandsnamen teruggeeft in een vector
        static std::map<std::string, std::vector<std::string>> getTagElementList(const char* tag, const char* element, const std::string& folderPath);

        bool loadTrack(); // Laad het gpx bestand en parse de data
        bool getWaypoints(); // Haal de waypoints op uit het gpx bestand
        bool getDefaultWaypoints(); // Haal de default waypoints op uit het gpx bestand

        float trackLenght; // Lengte van het track in meters
        float totalAscent; // Totaal stijgen in meters
        float totalDescent; // Totaal dalen in meters
        
        std::vector<wayPoint> trackData; // Bevat alle trackpoints van het gpx bestand
        
        std::vector<wayPoint> defaultWaypoints; // Bevat de default waypoints van het gpx bestand

        std::vector<elevationData> elevationProfile; // Bevat het elevatieprofiel van het track
        std::vector<ClimbSegment> climbs; // Bevat alle klimsegmenten van het track

    private:
        void getElevationProfile(); // Genereer het elevatieprofiel van het track
        void detectClimbs(); // Detecteer klimsegmenten in het track
        void getTrackLenght(); // Bereken de lengte van het track
        float getDistance(float lon1, float lat1, float lon2, float lat2, float h1, float h2); // Haversine formule om afstand te berekenen tussen twee GPS punten
        
        //Voor climbdetails leg climbs en elvationProfile over elkaar.
    
        std::string _filePath; // Pad naar het gpx bestand
        
};