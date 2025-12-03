#include "gpx.hpp"



GpxParser::GpxParser(const char* filePath) {
    _filePath = std::string(filePath);
}

GpxParser::~GpxParser() {
    trackData.clear();
}

std::map<std::string, std::vector<std::string>> GpxParser::getTagElementList(const char* tag, const char* element, const std::string& folderPath)
{
    std::map<std::string, std::vector<std::string>> elementsByFile;

    DIR* dir = opendir(folderPath.c_str());
    if (!dir)
    {
        return elementsByFile;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_REG)
        {
            std::string fileName = entry->d_name;

            if (fileName.size() >= 4 && fileName.substr(fileName.size() - 4) == ".gpx")
            {
                std::string filePath = folderPath + "/" + fileName;

                GpxParser parser(filePath.c_str());
                std::vector<std::string> elementValue;

                tinyxml2::XMLDocument doc;
                tinyxml2::XMLError result = doc.LoadFile(filePath.c_str());
                if (result == tinyxml2::XML_SUCCESS)
                {
                    tinyxml2::XMLElement* root = doc.RootElement();
                    if (root)
                    {
                        for (tinyxml2::XMLElement* Tag = root->FirstChildElement(tag); Tag!= nullptr; Tag = Tag->NextSiblingElement(tag))
                        {
                            tinyxml2::XMLElement* valueElement = Tag->FirstChildElement(element);
                            if (valueElement)
                            {
                                const char* value = valueElement->GetText();
                                if (value)
                                    elementValue.push_back(value);
                            }
                        }
                }
                else
                    ESP_LOGE("GPX", "Failed to get root element in file: %s", filePath.c_str());
                }
                else
                    ESP_LOGE("TAGGPX", "Failed to load GPX file: %s", filePath.c_str());

                elementsByFile[fileName] = elementValue;
            }
        }
    }

    closedir(dir);
    return elementsByFile;
}

bool GpxParser::loadTrack() {
    // Implementation to parse GPX track data
    tinyxml2::XMLDocument route;

    tinyxml2::XMLError result = route.LoadFile(_filePath.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        ESP_LOGE("GPX", "Failed to load GPX file: %s", _filePath.c_str());
        return false;
    }

    tinyxml2::XMLElement* root = route.RootElement();

    if (!root)
    {
        ESP_LOGE("GPX", "Failed to get root element in file: %s", _filePath.c_str());
        return false;
    }

    // Iterate through <trk> elements
    for (tinyxml2::XMLElement* trk = root->FirstChildElement(gpxTrackTag); trk != nullptr; trk = trk->NextSiblingElement(gpxTrackTag))
    {
        // Iterate through <trkseg> elements
        for (tinyxml2::XMLElement* trkseg = trk->FirstChildElement(gpxTrackSegTag); trkseg != nullptr; trkseg = trkseg->NextSiblingElement("trkseg"))
        {
            // Iterate through <trkpt> elements
            for (tinyxml2::XMLElement* trkpt = trkseg->FirstChildElement(gpxTrackPtTag); trkpt != nullptr; trkpt = trkpt->NextSiblingElement("trkpt"))
            {
                wayPoint point = {0};

                // Extract latitude and longitude
                trkpt->QueryFloatAttribute(gpxLatElem, &point.lat);
                trkpt->QueryFloatAttribute(gpxLonElem, &point.lon);

                // Extract optional elements
                tinyxml2::XMLElement* ele = trkpt->FirstChildElement("ele");
                if (ele) point.ele = static_cast<float>(ele->DoubleText());

                // tinyxml2::XMLElement* time = trkpt->FirstChildElement("time");
                // if (time) point.time = strdup(time->GetText());

                trackData.push_back(point);
            }
        }
    }
    return true;
}

bool GpxParser::getElevationData() {
    for(size_t i = 1; i < trackData.size(); ++i) {
        elevationData data = {0};
        float distance = sqrt(pow(trackData[i].lat - trackData[i-1].lat, 2) + pow(trackData[i].lon - trackData[i-1].lon, 2)) * 111320; // Approx conversion to meters
        data.distance = distance;
        data.elevation = trackData[i].ele;
        data.grade = (trackData[i].ele - trackData[i-1].ele) / distance * 100; // Grade in percentage
        elevationProfile.push_back(data);
    } 
}

bool GpxParser::getWaypoints() {
    // Implementation to parse default GPX waypoints
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLError result = doc.LoadFile(_filePath.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        ESP_LOGE("GPX", "Failed to load GPX file: %s", _filePath.c_str());
        return false;
    }

    tinyxml2::XMLElement* root = doc.RootElement();

    if (!root)
    {
        ESP_LOGE("GPX", "Failed to get root element in file: %s", _filePath.c_str());
        return false;
    }

    // Iterate through <wpt> elements
    for (tinyxml2::XMLElement* wpt = root->FirstChildElement(gpxWaypointTag); wpt != nullptr; wpt = wpt->NextSiblingElement(gpxWaypointTag))
    {
        wayPoint point = {0};

        // Extract latitude and longitude
        wpt->QueryFloatAttribute(gpxLatElem, &point.lat);
        wpt->QueryFloatAttribute(gpxLonElem, &point.lon);

        // Extract optional elements
        tinyxml2::XMLElement* ele = wpt->FirstChildElement(gpxEleElem);
        if (ele) point.ele = static_cast<float>(ele->DoubleText());

        tinyxml2::XMLElement* name = wpt->FirstChildElement(gpxNameElem);
        if (name) point.name = strdup(name->GetText());

        tinyxml2::XMLElement* desc = wpt->FirstChildElement(gpxDescElem);
        if (desc) point.desc = strdup(desc->GetText());

        tinyxml2::XMLElement* src = wpt->FirstChildElement(gpxSrcElem);
        if (src) point.src = strdup(src->GetText());

        trackData.push_back(point);
    }
    return true;
}

bool GpxParser::getDefaultWaypoints() {
    _filePath = "/sdcard/default_waypoints.gpx";
    bool result = getWaypoints();
    _filePath = "";
    return result;
}