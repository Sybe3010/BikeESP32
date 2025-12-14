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
    //tinyxml2::XMLError result = route.LoadFile("/sdcard/TRK/2705882668.gpx");
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
    getTrackLenght();
    getElevationProfile();
    detectClimbs();
    return true;
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

void GpxParser::getTrackLenght(){
    float distance = 0; 
    for(int i = 0; i < trackData.size() - 1; i++){
        //Calculate distance bewteen coordinates
        distance = distance + getDistance(trackData[i].lon, trackData[i].lat, trackData[i + 1].lon, trackData[i + 1].lat, trackData[i].ele, trackData[i + 1].ele);
    }
    trackLenght = distance;
}

float GpxParser::getDistance(float lon1, float lat1, float lon2, float lat2, float h1, float h2){
    float R = 6371000; //straal van de aarde in meters

    lon1 = lon1 * M_PI / 180;
    lat1 = lat1 * M_PI / 180;
    lon2 = lon2 * M_PI / 180;
    lat2 = lat2 * M_PI / 180;

    float dlon = lon2 - lon1;
    float dlat = lat2 - lat1;

    float a = sin(dlat/2) * sin(dlat/2) + cos(lat1) * cos(lat2) * sin(dlon/2) * sin(dlon/2);

    float dHorizontal = 2 * R * asin(sqrt(a));

    float dH = h2 - h1;

    return sqrt(dHorizontal * dHorizontal + dH* dH);
}

void GpxParser::getElevationProfile(){
    elevationProfile.clear();
    totalAscent = 0;     
    totalDescent = 0;     
    if(trackData.empty()) return;  
    
    for (int i = 0; i < trackData.size(); i++)
    {
        elevationData point = {0};

        if(i == 0){
            point.elevation = trackData[i].ele;
            point.distance = 0;
            point.grade = 0;
        } else{
            point.elevation = trackData[i].ele;
            point.distance = elevationProfile[i - 1].distance + getDistance(trackData[i - 1].lon, trackData[i - 1].lat, trackData[i].lon, trackData[i].lat, trackData[i - 1].ele, trackData[i].ele);
            float elevationDelta = point.elevation - elevationProfile[i - 1].elevation;
            float distanceDelta   = point.distance - elevationProfile[i - 1].distance;
            if(elevationDelta > 0){
                totalAscent = totalAscent + elevationDelta;
            } else {
                totalDescent = totalDescent + (elevationDelta * -1);
            }
            
            if (distanceDelta > 0.0f) {
                point.grade = elevationDelta / distanceDelta;
            } else {
                point.grade = 0.0f;
            }
        }
        elevationProfile.push_back(point);
    }
}

void GpxParser::detectClimbs(){
    const float maxNegativeGrade = -0.01;
    const float mapDropAllowed = 5.0f;

    bool climbing = false;
    size_t climbStart = 0;

    float accumulatedDrop = 0.0f;

    for(int i = 1; i < elevationProfile.size(); i++){
        float elevationDelta = elevationProfile[i].elevation - elevationProfile[i - 1].elevation;
        float distanceDelta = elevationProfile[i].distance - elevationProfile[i - 1].distance;

        float segmentGrade = 0.0f;
        if(distanceDelta > 0){
            segmentGrade = elevationDelta / distanceDelta;
        }

        bool isAscending;
        bool isSmallDrop;

        if(elevationDelta > 0){
            isAscending = true;
            isSmallDrop = false;
        } else {
            isAscending = false;
            if(segmentGrade >= maxNegativeGrade){
                isSmallDrop = true;
            }
        }

        if(isAscending || isSmallDrop){
            if(!climbing){
                climbing = true;
                climbStart = i - 1;
                accumulatedDrop = 0.0f;
            }

            if(elevationDelta < 0){
                accumulatedDrop = accumulatedDrop - elevationDelta;
            }

            if(accumulatedDrop > mapDropAllowed){
                climbing = false;
                size_t climbEnd = i - 1;

                ClimbSegment segment;
                segment.startIndex = climbStart;
                segment.endIndex = climbEnd;

                segment.totalDistance = elevationProfile[climbEnd].distance - elevationProfile[climbStart].distance;

                segment.totalElevationGain = elevationProfile[climbEnd].elevation - elevationProfile[climbStart].elevation;

                if(segment.totalDistance > 0){
                    segment.avgGrade = segment.totalElevationGain / segment.totalDistance;
                } else {
                    segment.avgGrade = 0.0;
                }

                bool valid = false;
                
                if(segment.totalDistance < 100){
                    if(segment.avgGrade > 0.05f){
                        valid = true;
                    } else{
                        valid = false;
                    }
                } else{
                    if(segment.avgGrade > 0.03f){
                        valid = true;
                    } else {
                        valid = false;
                    }
                }

                if(valid){
                    climbs.push_back(segment);
                }
            }
        } else {
            if(climbing){
                climbing = false;
                size_t climbEnd = i - 1;

                ClimbSegment segment;
                segment.startIndex = climbStart;
                segment.endIndex = climbEnd;

                segment.totalDistance = elevationProfile[climbEnd].distance - elevationProfile[climbStart].distance;

                segment.totalElevationGain = elevationProfile[climbEnd].elevation - elevationProfile[climbStart].elevation;

                if(segment.totalDistance > 0){
                    segment.avgGrade = segment.totalElevationGain / segment.totalDistance;
                } else {
                    segment.avgGrade = 0.0;
                }

                bool valid = false;
                
                if(segment.totalDistance < 100){
                    if(segment.avgGrade > 0.05f){
                        valid = true;
                    } else{
                        valid = false;
                    }
                } else{
                    if(segment.avgGrade > 0.03f){
                        valid = true;
                    } else {
                        valid = false;
                    }
                }

                if(valid){
                    climbs.push_back(segment);
                }
            }
        }
    }

    if(climbing){
        size_t climbEnd = elevationProfile.size() - 1;

        ClimbSegment segment;
        segment.startIndex = climbStart;
        segment.endIndex = climbEnd;

        segment.totalDistance = elevationProfile[climbEnd].distance - elevationProfile[climbStart].distance;

        segment.totalElevationGain = elevationProfile[climbEnd].elevation - elevationProfile[climbStart].elevation;

        if(segment.totalDistance > 0){
            segment.avgGrade = segment.totalElevationGain / segment.totalDistance;
        } else {
            segment.avgGrade = 0.0;
        }

        bool valid = false;
        
        if(segment.totalDistance < 100){
            if(segment.avgGrade > 0.05f){
                valid = true;
            } else{
                valid = false;
            }
        } else{
            if(segment.avgGrade > 0.03f){
                valid = true;
            } else {
                valid = false;
            }
        }

        if(valid){
            climbs.push_back(segment);
        }
    }
}