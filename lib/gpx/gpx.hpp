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
        GpxParser() {}
        GpxParser(const char* filePath);
        ~GpxParser();

        static std::map<std::string, std::vector<std::string>> getTagElementList(const char* tag, const char* element, const std::string& folderPath);

        bool loadTrack();
        bool getWaypoints();
        bool getDefaultWaypoints();

        float trackLenght;
        float totalAscent;
        float totalDescent;
        
        std::vector<wayPoint> trackData;
        
        std::vector<wayPoint> defaultWaypoints;

        std::vector<elevationData> elevationProfile;
        std::vector<ClimbSegment> climbs;

    private:
        void getElevationProfile();
        void detectClimbs();
        void getTrackLenght();
        float getDistance(float lon1, float lat1, float lon2, float lat2, float h1, float h2);
        
        //Voor climbdetails leg climbs en elvationProfile over elkaar.
        
        std::string _filePath;
        
};