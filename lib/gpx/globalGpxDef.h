#pragma once

#include <pgmspace.h>
#include <stdint.h>

static const char* wptFile PROGMEM = "/sdcard/WPT/waypoint.gpx"; /**< Path to the waypoint GPX file on the SD card. */
static const char* trkFolder PROGMEM = "/sdcard/TRK";            /**< Path to the track folder on the SD card. */

/**
 * @brief Waypoint Structure
 *
 * @details Stores information related to a GPS waypoint.
 */
struct wayPoint
{
    float     lat;     /**< Latitude of the waypoint. */
    float     lon;     /**< Longitude of the waypoint. */
    float     ele;     /**< Elevation of the waypoint. */
    char*     time;    /**< Timestamp of the waypoint (ISO 8601). */
    char*     name;    /**< Name of the waypoint. */
    char*     desc;    /**< Description of the waypoint. */
    char*     src;     /**< Source of the waypoint data. */
    char*     sym;     /**< Symbol associated with the waypoint. */
    char*     type;    /**< Type/category of the waypoint. */
    uint8_t   sat;     /**< Number of satellites used for this fix. */
    float     hdop;    /**< Horizontal dilution of precision. */
    float     vdop;    /**< Vertical dilution of precision. */
    float     pdop;    /**< Position dilution of precision. */
};

/// @brief Structure to hold elevation data points
/// @note Each point contains distance, elevation, and grade information.
struct elevationData{
    float distance;
    float elevation;
    float grade;
};


/// @brief Climbsegment structure
/// @note Structure representing a climb
struct ClimbSegment{
    size_t startIndex;
    size_t endIndex;
    float totalDistance;
    float totalElevationGain;
    float avgGrade;
};

/**
 * @Brief GPX header file format
 *
 * @details Static string containing the standard GPX 1.0 file header, to be used when creating new GPX files.
 */
static const char* gpxHeader PROGMEM = { "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                      "<gpx\n"
                      " version=\"1.0\"\n"
                      " creator=\"IceNav\"\n"
                      " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                      " xmlns=\"http://www.topografix.com/GPX/1/0\"\n"
                      " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd\">\n"
                      "</gpx>" };