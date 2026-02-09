#include "activity.hpp"

Activity::Activity(const char* filePath){
    _activityFilePath = filePath;
}

bool Activity::startActivity(){
    if(isStarted){
        return true;
    } else {
        createActivityFile();
		activityTimer = lv_timer_create(activityTimerUpdate, 1000, this);
		isStarted = true;
		_lastActivityPoint.lat = 0;
		_lastActivityPoint.lon = 0;
		return true;
    }
}

void Activity::activityTimerUpdate(lv_timer_t *t){
	Activity* activity = static_cast<Activity*>(lv_timer_get_user_data(t));
	if(activity->isStarted){
		activity->totalData.timer += 1;
		ActivityPoint ap;
		ap.lat = gps.gpsData.latitude;
		ap.lon = gps.gpsData.longitude;
		ap.ele = gps.gpsData.altitude;
		ap.speed = gps.gpsData.speed;
		ap.cadance = bleSensors.cadanceValue;
		ap.hartrate = bleSensors.hrValue;
		ap.power = 0;
		activity->addActivityPoint(ap);

		float plusAfstand = 0;
		if(activity->_lastActivityPoint.lat != 0 || activity->_lastActivityPoint.lon != 0){
			plusAfstand = activity->getDistance(activity->_lastActivityPoint.lon, activity->_lastActivityPoint.lat, ap.lon, ap.lat, activity->_lastActivityPoint.ele, ap.ele);
		}
		
		activity->totalData.distance += plusAfstand;

		activity->_lastActivityPoint = ap;
		activity->uiNeedsUpdate = true;
	}
}

float Activity::getDistance(float lon1, float lat1, float lon2, float lat2, float h1, float h2){
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

bool Activity::addActivityPoint(const ActivityPoint& point){
    _activityPoints.push_back(point);
    return writeGpxData(point);
}

std::vector<Activity::ActivityPoint>& Activity::getActivityPoints(){
    return _activityPoints;
}

Activity::ActivityData Activity::getActivityData(){
    return totalData;
}

//private functions
bool Activity::createActivityFile(){
    FILE* activityFile = storage.open(_activityFilePath.c_str(), "w");
    if(!activityFile){
        return false;
    }

    storage.print(activityFile, gpxTrkHeader);
    storage.close(activityFile);

    return true;
}


bool Activity::writeGpxData(const ActivityPoint& ap){
    tinyxml2::XMLDocument activityFile;
    tinyxml2::XMLError result = activityFile.LoadFile(_activityFilePath.c_str());

    tinyxml2::XMLElement* root = activityFile.RootElement();
	if (!root)  
	{
		return false;
	}

	tinyxml2::XMLElement* trkElem = root->FirstChildElement(AgpxTrackTag);
	if (!trkElem)
	{
		return false;
	}
	
	tinyxml2::XMLElement* trksegElem = trkElem->FirstChildElement(AgpxTrackSegmentTag);
	if (!trksegElem)	
	{
		return false;
	}

    tinyxml2::XMLElement* newTrkPt = activityFile.NewElement(AgpxTrackPointTag);
	newTrkPt->SetAttribute(AgpxLatElem, formatFloat(ap.lat, 6).c_str());
	newTrkPt->SetAttribute(AgpxLonElem, formatFloat(ap.lon, 6).c_str());

	tinyxml2::XMLElement* element = nullptr;

	element = activityFile.NewElement(AgpxEleElem);
	element->SetText(ap.ele);
	newTrkPt->InsertEndChild(element);

	tinyxml2::XMLElement* extensionsElem = activityFile.NewElement(AgpxExtensionTag);
	newTrkPt->InsertEndChild(extensionsElem);

	tinyxml2::XMLElement* tpExtElem = activityFile.NewElement(AgpxTrackPointExtensionTag);
	extensionsElem->InsertEndChild(tpExtElem);

	tinyxml2::XMLElement* hrElem = activityFile.NewElement(AgpxHrElem);
	hrElem->SetText(formatFloat(ap.hartrate, 1).c_str());
	tpExtElem->InsertEndChild(hrElem);

    tinyxml2::XMLElement* powerElem = activityFile.NewElement(AgpxPowerElem);
	powerElem->SetText(formatFloat(ap.power, 1).c_str());
	tpExtElem->InsertEndChild(powerElem);

	tinyxml2::XMLElement* lastTrkPt = trksegElem->LastChildElement(AgpxTrackPointTag);
	if (lastTrkPt) 
		trksegElem->InsertAfterChild(lastTrkPt, newTrkPt);
	else
		trksegElem->InsertFirstChild(newTrkPt);

	result = activityFile.SaveFile(_activityFilePath.c_str());
	if (result != tinyxml2::XML_SUCCESS)
	{
		return false;
	}
	return result == tinyxml2::XML_SUCCESS;
}

void Activity::calculateActivityData(){

}

bool Activity::stopActivity(){
	if(!isStarted){
		return true;
	} else {
		isStarted = false;
		lv_timer_delete(activityTimer);
		return true;
	}
}



std::string Activity::formatFloat(float value, int precision) 
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(precision) << value;
	return out.str();
}