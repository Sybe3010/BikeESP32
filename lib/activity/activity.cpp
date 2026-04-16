#include "activity.hpp"

Activity::Activity(const char* filePath){
    _activityFilePath = filePath;
}

bool Activity::startActivity(){
    if(isStarted){
        return true;
    } else {

        createActivityFile();

		// createFitFile();
		activityTimer = lv_timer_create(activityTimerUpdate, 1000, this);
		isStarted = true;
		_lastActivityPoint.lat = 0;
		_lastActivityPoint.lon = 0;
		return true;
    }
}

void Activity::activityTimerUpdate(lv_timer_t *t){
	uint32_t startTime = millis();
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

		// bool gelukt = activity->writeFitData();

		uint32_t dataWrite = millis() - startTime;

		float plusAfstand = 0;
		if(activity->_activityPoints.size() > 1){
			plusAfstand = activity->getDistance(activity->_lastActivityPoint.lon, activity->_lastActivityPoint.lat, ap.lon, ap.lat, activity->_lastActivityPoint.ele, ap.ele);
			if(activity->totalData.timer > 0) {
        		activity->totalData.avgSpeed = (activity->totalData.distance / activity->totalData.timer) * 3.6;
    		}
			activity->totalCadance += ap.cadance;
			activity->totalData.avgCadance = activity->totalCadance / activity->totalData.timer;
		}
		
		activity->totalData.distance += plusAfstand;

		activity->_lastActivityPoint = ap;
		activity->uiNeedsUpdate = true;
		uint32_t statsTime = millis() - startTime;

		log_e("dataWrite: %d ms, Stats: %d ms\n", dataWrite, statsTime);
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
    // Buffer in memory instead of writing every second
    return true;
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
		for(const auto& point : _activityPoints) {
            writeGpxData(point);
        }
		
		return true;
	}
}



std::string Activity::formatFloat(float value, int precision) 
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(precision) << value;
	return out.str();
}

// bool Activity::generateFitFile() {
// 	data_crc = 0;

// 	activityFile = storage.open(_activityFilePath.c_str(), "w+b");

// 	writeFitHeader(activityFile);
	
// 	timestamp = 1000000000; // 2021-09-08T01:46:40-0600Z in seconds since the FIT Epoch of 1989-12-31T:00:00:00Z
// 	start_time = timestamp;

// 	// Write file id message.
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_FILE_ID_MESG file_id_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_FILE_ID], &file_id_mesg);

// 		file_id_mesg.time_created = timestamp;
// 		file_id_mesg.type = FIT_FILE_ACTIVITY;
// 		file_id_mesg.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_FILE_ID], FIT_FILE_ID_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &file_id_mesg, FIT_FILE_ID_MESG_SIZE, activityFile);
// 	}

// 	// Write Device Info message.
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_DEVICE_INFO_MESG device_info_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_DEVICE_INFO], &device_info_mesg);

// 		device_info_mesg.device_index = FIT_DEVICE_INDEX_CREATOR;
// 		device_info_mesg.manufacturer = FIT_MANUFACTURER_DEVELOPMENT;
// 		device_info_mesg.product = 0; // USE A UNIQUE ID FOR EACH OF YOUR PRODUCTS
// 		strcpy(device_info_mesg.product_name, "FIT Cookbook"); // Max 20 Chars
// 		device_info_mesg.serial_number = 123456;
// 		device_info_mesg.software_version = 100; // 1.0 * 100
// 		device_info_mesg.timestamp = timestamp;

// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_DEVICE_INFO], FIT_DEVICE_INFO_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &device_info_mesg, FIT_DEVICE_INFO_MESG_SIZE, activityFile);
// 	}

// 	// Write Event message - START Event
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_EVENT_MESG event_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_EVENT], &event_mesg);

// 		event_mesg.timestamp = timestamp;
// 		event_mesg.event = FIT_EVENT_TIMER;
// 		event_mesg.event_type = FIT_EVENT_TYPE_START;

// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_EVENT], FIT_EVENT_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &event_mesg, FIT_EVENT_MESG_SIZE, activityFile);
// 	}

// 	// Write Record messages.

// 	//The message definition only needs to be written once.  
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_RECORD], FIT_RECORD_MESG_DEF_SIZE, activityFile);
// 	}
// }

// bool Activity::writeFitHeader(FILE *file){
// 	FIT_FILE_HDR file_header;
// 	file_header.header_size = FIT_FILE_HDR_SIZE;
// 	file_header.protocol_version = FIT_PROTOCOL_VERSION;
// 	file_header.profile_version = FIT_PROFILE_VERSION_MAJOR;
// 	memcpy((FIT_UINT8 *)&file_header.data_type, ".FIT", 4);
// 	fseek(file, 0, SEEK_END);
// 	file_header.data_size = ftell(file) - FIT_FILE_HDR_SIZE - sizeof(FIT_UINT16);
// 	file_header.crc = FitCRC_Calc16(&file_header, FIT_STRUCT_OFFSET(crc, FIT_FILE_HDR));

// 	fseek(file, 0, SEEK_SET);
// 	fwrite((void *)&file_header, 1, FIT_FILE_HDR_SIZE, file);
// }

// bool Activity::writeFitData(const ActivityPoint& ap){
// 	FIT_UINT8 local_mesg_number = 0;
// 	FIT_RECORD_MESG record_mesg;
// 	Fit_InitMesg(fit_mesg_defs[FIT_MESG_RECORD], &record_mesg);

// 	record_mesg.timestamp = timestamp;

// 	record_mesg.speed = (ap.speed) / 3.6 * 1000; 
// 	record_mesg.heart_rate = ap.hartrate; 
// 	record_mesg.cadence = ap.cadance; 
// 	record_mesg.power = ap.power; 
// 	record_mesg.enhanced_altitude = 5*ap.ele + 500; 
// 	record_mesg.position_lat = (FIT_SINT32)(ap.lat * SC_PER_M);
// 	record_mesg.position_long = (FIT_SINT32)(ap.lon * SC_PER_M);

// 	WriteMessage(local_mesg_number, &record_mesg, FIT_RECORD_MESG_SIZE, activityFile);

// 	timestamp++;
// }

// bool Activity::writeFitFooter(){
// 	// Write Event message - STOP Event
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_EVENT_MESG event_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_EVENT], &event_mesg);

// 		event_mesg.timestamp = timestamp;
// 		event_mesg.event = FIT_EVENT_TIMER;
// 		event_mesg.event_type = FIT_EVENT_TYPE_STOP;

// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_EVENT], FIT_EVENT_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &event_mesg, FIT_EVENT_MESG_SIZE, activityFile);
// 	}

// 	// Write Lap message.
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_LAP_MESG lap_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_LAP], &lap_mesg);

// 		lap_mesg.message_index = 0;
// 		lap_mesg.timestamp = timestamp;
// 		lap_mesg.start_time = start_time;
// 		lap_mesg.total_elapsed_time = (timestamp - start_time) * 1000;
// 		lap_mesg.total_timer_time = (timestamp - start_time) * 1000;

// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_LAP], FIT_LAP_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &lap_mesg, FIT_LAP_MESG_SIZE, activityFile);
// 	}

// 	// Write Session message.
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_SESSION_MESG session_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_SESSION], &session_mesg);

// 		session_mesg.message_index = 0;
// 		session_mesg.timestamp = timestamp;
// 		session_mesg.start_time = start_time;
// 		session_mesg.total_elapsed_time = (timestamp - start_time) * 1000;
// 		session_mesg.total_timer_time = (timestamp - start_time) * 1000;
// 		session_mesg.sport = FIT_SPORT_STAND_UP_PADDLEBOARDING;
// 		session_mesg.sub_sport = FIT_SUB_SPORT_GENERIC;
// 		session_mesg.first_lap_index = 0;
// 		session_mesg.num_laps = 1;

// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_SESSION], FIT_SESSION_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &session_mesg, FIT_SESSION_MESG_SIZE, activityFile);
// 	}

// 	// Write Activity message.
// 	{
// 		FIT_UINT8 local_mesg_number = 0;
// 		FIT_ACTIVITY_MESG activity_mesg;
// 		Fit_InitMesg(fit_mesg_defs[FIT_MESG_ACTIVITY], &activity_mesg);

// 		activity_mesg.timestamp = timestamp;
// 		activity_mesg.num_sessions = 1;
// 		activity_mesg.total_timer_time = (timestamp - start_time) * 1000;

// 		int timezoneOffset = -7 * 3600;
// 		activity_mesg.local_timestamp = timestamp + timezoneOffset;

// 		WriteMessageDefinition(local_mesg_number, fit_mesg_defs[FIT_MESG_ACTIVITY], FIT_ACTIVITY_MESG_DEF_SIZE, activityFile);
// 		WriteMessage(local_mesg_number, &activity_mesg, FIT_ACTIVITY_MESG_SIZE, activityFile);
// 	}

// 	// Write CRC.
// 	fwrite(&data_crc, 1, sizeof(FIT_UINT16), activityFile);

// 	// Update file header with data size.
// 	writeFitHeader(activityFile);

// 	fclose(activityFile);

// 	return true;
// }

// void Activity::WriteMessageDefinition(FIT_UINT8 local_mesg_number, const void *mesg_def_pointer, FIT_UINT16 mesg_def_size, FILE *fp)
// {
// 	FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT;
// 	WriteData(&header, FIT_HDR_SIZE, fp);
// 	WriteData(mesg_def_pointer, mesg_def_size, fp);
// }

// void Activity::WriteMessageDefinitionWithDevFields
// (
// 	FIT_UINT8 local_mesg_number,
// 	const void *mesg_def_pointer,
// 	FIT_UINT8 mesg_def_size,
// 	FIT_UINT8 number_dev_fields,
// 	FIT_DEV_FIELD_DEF *dev_field_definitions,
// 	FILE *fp
// )
// {
// 	FIT_UINT16 i;
// 	FIT_UINT8 header = local_mesg_number | FIT_HDR_TYPE_DEF_BIT | FIT_HDR_DEV_DATA_BIT;
// 	WriteData(&header, FIT_HDR_SIZE, fp);
// 	WriteData(mesg_def_pointer, mesg_def_size, fp);

// 	WriteData(&number_dev_fields, sizeof(FIT_UINT8), fp);
// 	for (i = 0; i < number_dev_fields; i++)
// 	{
// 		WriteData(&dev_field_definitions[i], sizeof(FIT_DEV_FIELD_DEF), fp);
// 	}
// }

// void Activity::WriteMessage(FIT_UINT8 local_mesg_number, const void *mesg_pointer, FIT_UINT16 mesg_size, FILE *fp)
// {
// 	WriteData(&local_mesg_number, FIT_HDR_SIZE, fp);
// 	WriteData(mesg_pointer, mesg_size, fp);
// }

// void Activity::WriteDeveloperField(const void *data, FIT_UINT16 data_size, FILE *fp)
// {
// 	WriteData(data, data_size, fp);
// }

// void Activity::WriteData(const void *data, FIT_UINT16 data_size, FILE *fp)
// {
// 	FIT_UINT16 offset;

// 	fwrite(data, 1, data_size, fp);

// 	for (offset = 0; offset < data_size; offset++)
// 		data_crc = FitCRC_Get16(data_crc, *((FIT_UINT8 *)data + offset));
// }