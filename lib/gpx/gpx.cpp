#include "gpx.hpp"

GpxParser::GpxParser(const char* filePath) {
    _filePath = std::string(filePath);
}

GpxParser::~GpxParser() {
    trackData.clear();
}

/// @brief Functie om alle gpx bestanden in een map te scannen en de waarden van een specifiek element binnen een tag op te halen
/// @param element Dit is het element waarvan de waarde opgehaald moet worden
/// @param folderPath Dit is het pad naar de map met gpx bestanden
/// @return Een map met bestandsnamen als sleutel en een vector van elementwaarden als waarde
/// @note Deze functie maakt gebruik van de TinyXML2 bibliotheek om de gpx bestanden te parsen, gebaseerd op code van Jordi Gauchía
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

/// @brief  Laad en parse het gpx bestand om de track data op te halen
/// @return true als het laden en parsen succesvol was, anders false
/// @note Deze functie maakt gebruik van de TinyXML2 bibliotheek om de gpx bestanden te parsen, gebaseerd op code van Jordi Gauchía. Aangepast om ook het hoogteprofiel en klimsegmenten er uit te halen.
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


/// @brief haal de waypoints op uit het gpx bestand
/// @return true als het ophalen succesvol was, anders false
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

/// @brief haal de default waypoints op uit het gpx bestand
/// @return true als het ophalen succesvol was, anders false
bool GpxParser::getDefaultWaypoints() {
    _filePath = "/sdcard/default_waypoints.gpx";
    bool result = getWaypoints();
    _filePath = "";
    return result;
}

/// @brief Bereken de lengte van het track in meters
/// @note Maakt gebruik van de Haversine formule om de afstand tussen twee GPS punten te berekenen
void GpxParser::getTrackLenght(){
    float distance = 0; 
    for(int i = 0; i < trackData.size() - 1; i++){
        //Calculate distance bewteen coordinates
        distance = distance + getDistance(trackData[i].lon, trackData[i].lat, trackData[i + 1].lon, trackData[i + 1].lat, trackData[i].ele, trackData[i + 1].ele);
    }
    trackLenght = distance;
}

/// @brief Bereken de afstand tussen twee GPS punten met de Haversine formule
/// @param lon1 Longitude van punt 1
/// @param lat1 Latitude van punt 1
/// @param lon2 Longitude van punt 2
/// @param lat2 Latitude van punt 2
/// @param h1 Hoogte van punt 1
/// @param h2 Hoogte van punt 2
/// @return Afstand tussen de twee punten in meters
/// @note De Haversine formule houdt rekening met de kromming van de aarde
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

/// @brief Genereer het elevatieprofiel van het track
/// @note Bereken voor elk trackpoint de afstand vanaf het begin, de elevatie en de gemiddelde helling, code klopt nog niet helemaal
void GpxParser::getElevationProfile(){
    elevationProfile.clear(); 
    totalAscent = 0;     
    totalDescent = 0;     
    if(trackData.empty()) return;   // Geen trackdata beschikbaar
    
    for (int i = 0; i < trackData.size(); i++) // Voor elk trackpoint
    {
        elevationData point = {0}; // Maak een nieuw elevatieData object aan

        if(i == 0){ // Eerste punt
            point.elevation = trackData[i].ele;
            point.distance = 0;
            point.grade = 0;
        } else{
            point.elevation = trackData[i].ele; // Elevatie van het huidige punt
            point.distance = elevationProfile[i - 1].distance + getDistance(trackData[i - 1].lon, trackData[i - 1].lat, trackData[i].lon, trackData[i].lat, trackData[i - 1].ele, trackData[i].ele); // Totale afstand vanaf het begin tot het huidige punt
            float elevationDelta = point.elevation - elevationProfile[i - 1].elevation; // Verandering in elevatie sinds het vorige punt
            float distanceDelta   = point.distance - elevationProfile[i - 1].distance; // Verandering in afstand sinds het vorige punt
            if(elevationDelta > 0){ // Stijging
                totalAscent = totalAscent + elevationDelta;
            } else { // Dalingen
                totalDescent = totalDescent + (elevationDelta * -1);
            }
            
            if (distanceDelta > 0.0f) { //  Voorkom deling door nul
                point.grade = elevationDelta / distanceDelta;
            } else { // Geen verandering in afstand
                point.grade = 0.0f;
            }
        }
        elevationProfile.push_back(point);
    }
}

/// @brief Detecteer klimsegmenten in het track
/// @note het detecteert klimsegmenten op basis van stijgingen en kleine dalingen, en slaat deze op in de climbs vector
/// Het gebruikt drempelwaarden voor maximale negatieve helling en toegestane daling om te bepalen of een segment als klim wordt beschouwdµ
/// Het valideert ook klimsegmenten op basis van hun lengte en gemiddelde helling voordat ze worden toegevoegd aan de lijst
void GpxParser::detectClimbs(){
    const float maxNegativeGrade = -0.015; // Maximale negatieve helling om als kleine daling te worden beschouwd
    const float mapDropAllowed = 10.0f; // Maximale toegestane daling in meters binnen een klimsegment

    bool climbing = false; // Huidige staat: in een klimsegment of niet
    size_t climbStart = 0; // Startindex van het huidige klimsegment

    float accumulatedDrop = 0.0f;

    for(int i = 1; i < elevationProfile.size(); i++){ // Begin bij 1 omdat we de vorige index nodig hebben
        float elevationDelta = elevationProfile[i].elevation - elevationProfile[i - 1].elevation; // Verandering in elevatie sinds het vorige punt
        float distanceDelta = elevationProfile[i].distance - elevationProfile[i - 1].distance; // Verandering in afstand sinds het vorige punt

        float segmentGrade = 0.0f; // Helling van het segment
        if(distanceDelta > 0){ // Voorkom deling door nul
            segmentGrade = elevationDelta / distanceDelta; 
        }

        bool isAscending;
        bool isSmallDrop;

        if(elevationDelta > 0){ // Stijging
            isAscending = true;
            isSmallDrop = false;
        } else { // Dalingen
            isAscending = false;
            if(segmentGrade >= maxNegativeGrade){
                isSmallDrop = true;
            }
        }

        if(isAscending || isSmallDrop){ // Stijging of kleine daling
            if(!climbing){ // Begin van een nieuw klimsegment
                climbing = true;
                climbStart = i - 1;
                accumulatedDrop = 0.0f;
            }

            if(elevationDelta < 0){ // Kleine daling
                accumulatedDrop = accumulatedDrop - elevationDelta;
            }

            if(accumulatedDrop > mapDropAllowed){ // Te veel daling, klimsegment beëindigen
                climbing = false;
                size_t climbEnd = i - 1;

                ClimbSegment segment;
                segment.startIndex = climbStart; // Startindex van het klimsegment
                segment.endIndex = climbEnd; // Eindindex van het klimsegment

                segment.totalDistance = elevationProfile[climbEnd].distance - elevationProfile[climbStart].distance; // Totale afstand van het klimsegment

                segment.totalElevationGain = elevationProfile[climbEnd].elevation - elevationProfile[climbStart].elevation; // Totale elevatie winst van het klimsegment

                if(segment.totalDistance > 0){ // Voorkom deling door nul
                    segment.avgGrade = segment.totalElevationGain / segment.totalDistance;
                } else { 
                    segment.avgGrade = 0.0;
                }

                bool valid = false;
                
                if(segment.totalDistance < 200){ // Korte klimsegmenten vereisen een steilere helling
                    if(segment.avgGrade > 0.035f){
                        valid = true;
                    } else{
                        valid = false;
                    }
                } else{ // Langere klimsegmenten hebben een lagere drempel voor helling
                    if(segment.avgGrade > 0.015f){
                        valid = true;
                    } else {
                        valid = false;
                    }
                }

                if(valid){ // Voeg geldig klimsegment toe aan de lijst
                    climbs.push_back(segment);
                }
            }
        } else { // Dalingen groter dan de drempel
            if(climbing){ // Einde van een klimsegment
                climbing = false;
                size_t climbEnd = i - 1;

                ClimbSegment segment;
                segment.startIndex = climbStart; // Startindex van het klimsegment
                segment.endIndex = climbEnd; // Eindindex van het klimsegment

                segment.totalDistance = elevationProfile[climbEnd].distance - elevationProfile[climbStart].distance; // Totale afstand van het klimsegment

                segment.totalElevationGain = elevationProfile[climbEnd].elevation - elevationProfile[climbStart].elevation; // Totale elevatie winst van het klimsegment

                if(segment.totalDistance > 0){ // Voorkom deling door nul
                    segment.avgGrade = segment.totalElevationGain / segment.totalDistance;
                } else {
                    segment.avgGrade = 0.0;
                }

                bool valid = false;
                
                if(segment.totalDistance < 200){ // Korte klimsegmenten vereisen een steilere helling
                    if(segment.avgGrade > 0.035f){
                        valid = true;
                    } else{
                        valid = false;
                    }
                } else{ // Langere klimsegmenten hebben een lagere drempel voor helling
                    if(segment.avgGrade > 0.015f){
                        valid = true;
                    } else {
                        valid = false;
                    }
                }

                if(valid){ // Voeg geldig klimsegment toe aan de lijst
                    climbs.push_back(segment);
                }
            }
        }
    }

    if(climbing){ // Afsluiten van een lopend klimsegment aan het einde van het profiel
        size_t climbEnd = elevationProfile.size() - 1; // Laatste index

        ClimbSegment segment; // Maak een nieuw klimsegment aan
        segment.startIndex = climbStart; // Startindex van het klimsegment
        segment.endIndex = climbEnd; // Eindindex van het klimsegment

        segment.totalDistance = elevationProfile[climbEnd].distance - elevationProfile[climbStart].distance; // Totale afstand van het klimsegment

        segment.totalElevationGain = elevationProfile[climbEnd].elevation - elevationProfile[climbStart].elevation; // Totale elevatie winst van het klimsegment

        if(segment.totalDistance > 0){ //  Voorkom deling door nul
            segment.avgGrade = segment.totalElevationGain / segment.totalDistance;
        } else {
            segment.avgGrade = 0.0;
        }

        bool valid = false;
        
        if(segment.totalDistance < 200){ // Korte klimsegmenten vereisen een steilere helling
            if(segment.avgGrade > 0.035f){
                valid = true;
            } else{
                valid = false;
            }
        } else{ //  Langere klimsegmenten hebben een lagere drempel voor helling
            if(segment.avgGrade > 0.015f){
                valid = true;
            } else {
                valid = false;
            }
        }

        if(valid){ // Voeg geldig klimsegment toe aan de lijst
            climbs.push_back(segment);
        }
    }
}