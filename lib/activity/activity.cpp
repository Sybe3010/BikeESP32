#include "activity.hpp"

Activity::Activity(const char* filePath){
    _activityFilePath = filePath;
}

bool Activity::startActivity(){
    if(isStarted){
        return true;
    } else {
        createActivityFile();
		const esp_timer_create_args_t activity_timer_args = { .callback = &ActivityTimer, .arg = this, .name = "activityTimer" };
 		esp_timer_handle_t activity_timer; 
  		esp_timer_create(&activity_timer_args, &activity_timer);
  		esp_timer_start_periodic(activity_timer, 1000000); // 1000000 µs = 1 s
		isStarted = true;
		return true;
    }
}

void Activity::ActivityTimer(void *arg){
	Activity* activity = static_cast<Activity*>(arg);
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

		activity->uiNeedsUpdate = true;
	}
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

std::string Activity::formatFloat(float value, int precision) 
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(precision) << value;
	return out.str();
}