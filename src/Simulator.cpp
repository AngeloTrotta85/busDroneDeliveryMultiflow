/*
 * Simulator.cpp
 *
 *  Created on: 15 lug 2017
 *      Author: angelo
 */

#include "Simulator.h"

using namespace std;

Simulator::Simulator() {
	time = 0;
	timeslot = 1;
	maxTime = 0;
	nUAV = 5;

	eSTOP = -100;
	eRELAY = -120;
	eRECORDING = -150;
	eRECORDINGRELAY = -170;
	eRECHARGING = 25;

	maxDistancePoiStop = 200;

	initialUavEnergy = 130000; //Joule

	stopsMinLat = 44.484336; //std::numeric_limits<double>::min();
	stopsMaxLat = 44.506704; //std::numeric_limits<double>::max();
	stopsMinLon = 11.326122; //std::numeric_limits<double>::min();
	stopsMaxLon = 11.358350; //std::numeric_limits<double>::max();

	operationalDay = std::string("20170522"); // Monday 2017-05-22
	operationalDay_tm.tm_year = 117;
	operationalDay_tm.tm_mon = 4;
	operationalDay_tm.tm_mday = 22;
	operationalDay_tm.tm_wday = 1;
	operationalDay_tm.tm_sec = operationalDay_tm.tm_min = operationalDay_tm.tm_hour = 0;
	operationalDay_tm.tm_yday = 141;
	operationalDay_tm.tm_isdst = 1;

	start_sim_time_tm = operationalDay_tm;
	start_sim_time_tm.tm_hour = 8;

	end_sim_time_tm = operationalDay_tm;
	end_sim_time_tm.tm_hour = 9;

	//flowGraph = new FlowGraph(this);
	//flowGraph = new TspGraph(this);
	//flowGraph = new BestPoiGraph(this);
	//flowGraph = new WorstUav(this);
	flowGraph = nullptr;

	toCluster = false;

	finalLifetime = 0;

	setRandomPoint = false;
	numberOfRandomPoint = 0;
}

Simulator::~Simulator() { }

bool Simulator::isInsideBoudaries(double lat, double lon) {
	bool ris = false;

	for (auto& b : boundaryList) {
		if (	(lat >= b.minLAT) &&
				(lat <= b.maxLAT) &&
				(lon >= b.minLON) &&
				(lon <= b.maxLON)	){
			ris = true;
			break;
		}
	}

	return ris;
}

void Simulator::importSomeParameterFromInputLine(InputParser *inputVal) {

	cout << "Reading PARAMETERS" << endl;

	// CLUSTERING FLAG
	const std::string &toClusterString = inputVal->getCmdOption("-c");
	if (!toClusterString.empty()) {
		if (toClusterString.compare("0") == 0) {
			toCluster = false;
		}
		else {
			toCluster = true;
		}
	}
	else {
		toCluster = false;
	}

	// ALGORITHM TYPE
	const std::string &algoTypeString = inputVal->getCmdOption("-a");
	if (!algoTypeString.empty()) {
		if (algoTypeString.compare("TSP") == 0) {
			cout << "Setting algo TSP" << endl;
			flowGraph = new TspGraph(this);
		}
		else if (algoTypeString.compare("BESTPOI") == 0) {
			cout << "Setting algo BESTPOI" << endl;
			flowGraph = new BestPoiGraph(this);
		}
		else if (algoTypeString.compare("WORSTUAV") == 0) {
			cout << "Setting algo WORSUAV" << endl;
			flowGraph = new WorstUav(this);
		}
		else {
			cout << "Setting algo NOREC" << endl;
			flowGraph = new FlowGraph(this);
		}
	}
	else {
		cout << "Setting algo default NOREC" << endl;
		flowGraph = new FlowGraph(this);
	}

	// START SIMULATION TIME
	const std::string &sSimString = inputVal->getCmdOption("-sSIM");
	if (!sSimString.empty()) {
		unsigned int hh, mm, ss;
		start_sim_time_tm = operationalDay_tm;

		sscanf(sSimString.c_str(), "%u:%u:%u", &hh, &mm, &ss);
		start_sim_time_tm.tm_hour = hh;
		start_sim_time_tm.tm_min = mm;
		start_sim_time_tm.tm_sec = ss;
	}
	else {
		start_sim_time_tm = operationalDay_tm;
		start_sim_time_tm.tm_hour = 8;
		start_sim_time_tm.tm_min = 0;
		start_sim_time_tm.tm_sec = 0;
	}

	// END SIMULATION TIME
	const std::string &eSimString = inputVal->getCmdOption("-eSIM");
	if (!eSimString.empty()) {
		unsigned int hh, mm, ss;
		end_sim_time_tm = operationalDay_tm;

		sscanf(eSimString.c_str(), "%u:%u:%u", &hh, &mm, &ss);
		end_sim_time_tm.tm_hour = hh;
		end_sim_time_tm.tm_min = mm;
		end_sim_time_tm.tm_sec = ss;
	}
	else {
		end_sim_time_tm = operationalDay_tm;
		end_sim_time_tm.tm_hour = 10;
		end_sim_time_tm.tm_min = 0;
		end_sim_time_tm.tm_sec = 0;
	}

	// MAXIMUM DISTANCE POI-STOPS
	const std::string &distPoiString = inputVal->getCmdOption("-distPOI");
	if (!distPoiString.empty()) {
		maxDistancePoiStop = atof(distPoiString.c_str());
	}
	else {
		maxDistancePoiStop = 200;
	}

	// INITIAL ENERGY
	const std::string &eInitString = inputVal->getCmdOption("-eINIT");
	if (!eInitString.empty()) {
		initialUavEnergy = atof(eInitString.c_str());
	}
	else {
		initialUavEnergy = 130000;
	}

	// ENERGY IF UAV IS IN STOP-ARC
	const std::string &eStopString = inputVal->getCmdOption("-eSTOP");
	if (!eStopString.empty()) {
		eSTOP = atof(eStopString.c_str());
	}
	else {
		eSTOP = -100;
	}

	// ENERGY IF UAV IS IN POI_RELAY-ARC
	const std::string &eRelayString = inputVal->getCmdOption("-eREL");
	if (!eRelayString.empty()) {
		eRELAY = atof(eRelayString.c_str());
	}
	else {
		eRELAY = -120;
	}

	// ENERGY IF UAV IS IN POI_RECORDING-ARC
	const std::string &eRecordingString = inputVal->getCmdOption("-eREC");
	if (!eRecordingString.empty()) {
		eRECORDING = atof(eRecordingString.c_str());
	}
	else {
		eRECORDING = -150;
	}

	// ENERGY IF UAV IS IN POI_RELAYRECORDING-ARC
	const std::string &eRecordingRelayString = inputVal->getCmdOption("-eRECREL");
	if (!eRecordingRelayString.empty()) {
		eRECORDINGRELAY = atof(eRecordingRelayString.c_str());
	}
	else {
		eRECORDINGRELAY = -170;
	}

	// ENERGY IF UAV IS RECHARGING ON BUS
	const std::string &eRechargingString = inputVal->getCmdOption("-eCHA");
	if (!eRechargingString.empty()) {
		eRECHARGING = atof(eRechargingString.c_str());
	}
	else {
		eRECHARGING = 25;
	}

	// MAP BOUNDARY-1
	const std::string &boundary1String = inputVal->getCmdOption("-b1");
	if (!boundary1String.empty()) {
		Boundary newB;
		sscanf(boundary1String.c_str(), "%lf,%lf;%lf,%lf", &newB.minLAT, &newB.maxLAT, &newB.minLON, &newB.maxLON);
		boundaryList.push_back(newB);

		cout << "Looking for stops in boundaries 1: " << newB.minLAT << "," << newB.minLON << ";" << newB.maxLAT << "," << newB.maxLON << endl;
	}
	else {
		Boundary newB;
		newB.minLAT = 44.484336; //std::numeric_limits<double>::min();
		newB.maxLAT = 44.506704; //std::numeric_limits<double>::max();
		newB.minLON = 11.326122; //std::numeric_limits<double>::min();
		newB.maxLON = 11.358350; //std::numeric_limits<double>::max();
		boundaryList.push_back(newB);

		cout << "Looking for stops in boundaries 1: " << newB.minLAT << "," << newB.minLON << ";" << newB.maxLAT << "," << newB.maxLON << endl;
	}

	// MAP BOUNDARY-2
	const std::string &boundary2String = inputVal->getCmdOption("-b2");
	if (!boundary2String.empty()) {
		Boundary newB;
		sscanf(boundary2String.c_str(), "%lf,%lf;%lf,%lf", &newB.minLAT, &newB.maxLAT, &newB.minLON, &newB.maxLON);
		boundaryList.push_back(newB);

		cout << "Looking for stops in boundaries 2: " << newB.minLAT << "," << newB.minLON << ";" << newB.maxLAT << "," << newB.maxLON << endl;
	}

	// MAP BOUNDARY-3
	const std::string &boundary3String = inputVal->getCmdOption("-b3");
	if (!boundary3String.empty()) {
		Boundary newB;
		sscanf(boundary3String.c_str(), "%lf,%lf;%lf,%lf", &newB.minLAT, &newB.maxLAT, &newB.minLON, &newB.maxLON);
		boundaryList.push_back(newB);
		cout << "Looking for stops in boundaries 3: " << newB.minLAT << "," << newB.minLON << ";" << newB.maxLAT << "," << newB.maxLON << endl;
	}

	// MAP BOUNDARY-4
	const std::string &boundary4String = inputVal->getCmdOption("-b4");
	if (!boundary4String.empty()) {
		Boundary newB;
		sscanf(boundary4String.c_str(), "%lf,%lf;%lf,%lf", &newB.minLAT, &newB.maxLAT, &newB.minLON, &newB.maxLON);
		boundaryList.push_back(newB);
		cout << "Looking for stops in boundaries 4: " << newB.minLAT << "," << newB.minLON << ";" << newB.maxLAT << "," << newB.maxLON << endl;
	}

	//POI random points
	const std::string &randomPoiString = inputVal->getCmdOption("-randPOI");
	if (!randomPoiString.empty()) {
		setRandomPoint = true;
		numberOfRandomPoint = atol(randomPoiString.c_str());
	}
	else {
		setRandomPoint = false;
	}

	//DEBUG PRINT
	cout << "Using Energy -> init:" << initialUavEnergy
			<< "; stop:" << eSTOP
			<< "; relay:" << eRELAY
			<< "; recording:" << eRECORDING
			<< "; rel-rec:" << eRECORDINGRELAY
			<< "; recharge:" << eRECHARGING
			<< endl;
}

bool Simulator::importStops(std::string stopsFileName) {
	int rowCount;
	std::string str;

	cout << "START PARSING STOPS" << endl;

	std::ifstream fileStops(stopsFileName, std::ifstream::in);
	if(!fileStops.is_open()) return false;

	std::getline(fileStops, str); // read first line
	rowCount = 1;
	while (std::getline(fileStops, str)) {
		Stops newStop;

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

		if(newStop.parseInput(str)){
			//if (	(newStop.getStopLatNum() >= stopsMinLat) &&
			//		(newStop.getStopLatNum() <= stopsMaxLat) &&
			//		(newStop.getStopLonNum() >= stopsMinLon) &&
			//		(newStop.getStopLonNum() <= stopsMaxLon)	){
			if (isInsideBoudaries(newStop.getStopLatNum(), newStop.getStopLonNum())) {
				stopsMap[newStop.getStopIdNum()] = newStop;
				//cout << "Adding Stops -> " << str << endl;
			}
		}
		else {
			cerr << "Error parsing Stops -> " << str << endl;
			return EXIT_FAILURE;
		}
		rowCount++;
		fprintf(stdout, "\rStops parsing: %.03f%%", ((((double) rowCount) / STOPS_FILE_SMALL_SIZE) * 100.0));
	}
	cout << "  -  Parsed " << stopsMap.size() << " stops out of " << (STOPS_FILE_SMALL_SIZE - 1) << endl;
	fileStops.close();
	//cout << "END PARSING STOPS. Imported " << db.getStopsSize() << " out of " << (STOPS_FILE_SIZE - 1) << endl << endl;

	return true;
}

bool Simulator::importTrips(std::string stopsFileName) {
	int rowCount;
	std::string str;

	cout << "START PARSING TRIPS" << endl;

	std::ifstream fileTrips(stopsFileName, std::ifstream::in);
	if(!fileTrips.is_open()) return false;

	std::getline(fileTrips, str); // read first line
	rowCount = 1;
	while (std::getline(fileTrips, str)) {
		Trips newTrip;

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

		if(newTrip.parseInput(str)){
			std::string calendarDateKey = newTrip.getServiceId() + " " + operationalDay;
			if (calendarDateMap.count(calendarDateKey) > 0) {
				tripsMap[newTrip.getTripId()] = newTrip;
				//cout << "Adding Trip -> " << str << endl;
			}
		}
		else {
			cerr << "Error parsing Trips -> " << str << endl;
			return EXIT_FAILURE;
		}
		rowCount++;
		fprintf(stdout, "\rTrips parsing: %.03f%%", ((((double) rowCount) / TRIPS_FILE_SMALL_SIZE) * 100.0));
	}
	cout << "  -  Parsed " << tripsMap.size() << " trips out of " << (TRIPS_FILE_SMALL_SIZE - 1) << endl;
	fileTrips.close();
	//cout << "END PARSING STOPS. Imported " << db.getStopsSize() << " out of " << (STOPS_FILE_SIZE - 1) << endl << endl;

	return true;
}

bool Simulator::importCalendarDates(std::string stopsFileName) {
	int rowCount;
	std::string str;

	cout << "START PARSING CALENDAR DATES" << endl;

	std::ifstream fileCD(stopsFileName, std::ifstream::in);
	if(!fileCD.is_open()) return false;

	std::getline(fileCD, str); // read first line
	rowCount = 1;
	while (std::getline(fileCD, str)) {
		CalendarDate newCD;

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

		if(newCD.parseInput(str)){
			if (newCD.getDate().compare(operationalDay) == 0) {
				std::string key = newCD.getServiceId() + " " + newCD.getDate();
				calendarDateMap[key] = newCD;
				//cout << "Adding Stops -> " << str << endl;
			}
		}
		else {
			cerr << "Error parsing Calendar Dates -> " << str << endl;
			return EXIT_FAILURE;
		}
		rowCount++;
		fprintf(stdout, "\rCalendar Dates parsing: %.03f%%", ((((double) rowCount) / CALENDAR_DATES_FILE_SMALL_SIZE) * 100.0));
	}
	cout << "  -  Parsed " << calendarDateMap.size() << " calendar dates out of " << (CALENDAR_DATES_FILE_SMALL_SIZE - 1) << endl;
	fileCD.close();
	//cout << "END PARSING STOPS. Imported " << db.getStopsSize() << " out of " << (STOPS_FILE_SIZE - 1) << endl << endl;

	return true;
}

bool Simulator::importStopTimes(std::string stopsFileName) {
	int rowCount;
	std::string str;

	cout << "START PARSING STOP TIMES" << endl;

	std::ifstream fileStopTImes(stopsFileName, std::ifstream::in);
	if(!fileStopTImes.is_open()) return false;

	std::getline(fileStopTImes, str); // read first line
	rowCount = 1;
	while (std::getline(fileStopTImes, str)) {
		StopTimes newStopTime;

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

		//cout << "Adding Stops -> " << str << " ";
		if(newStopTime.parseInput(str)){
			if ( (tripsMap.count(newStopTime.getTripId()) > 0) && (stopsMap.count(newStopTime.getStopIdNum()) > 0) ) {
				std::string key = newStopTime.getTripId() + " " + newStopTime.getStopId() + " " + newStopTime.getStopSequence();
				stopTimesMap[key] = newStopTime;


				if (stopTimesMapMap.count(newStopTime.getTripId()) == 0) {
					std::map<unsigned long int, StopTimes> newMap;
					stopTimesMapMap[newStopTime.getTripId()] = newMap;
				}
				stopTimesMapMap[newStopTime.getTripId()][newStopTime.getStopSequenceNum()] = newStopTime;

				//cout << "Adding Stops -> " << str << " - Parsed: " << newStopTime.getStopSequenceNum() << " [" << newStopTime.getArrivalTimeTm().tm_hour << "." << newStopTime.getArrivalTimeTm().tm_min << "." << newStopTime.getArrivalTimeTm().tm_sec << "]" << endl;
			}
		}
		else {
			cerr << "Error parsing Stop Times -> " << str << endl;
			return EXIT_FAILURE;
		}
		rowCount++;
		fprintf(stdout, "\rStop Times parsing: %.03f%%", ((((double) rowCount) / STOP_TIMES_FILE_SMALL_SIZE) * 100.0));
	}
	cout << "  -  Parsed1 " << stopTimesMap.size() << " stop times out of " << (STOP_TIMES_FILE_SMALL_SIZE - 1);
	int c2 = 0;
	for (auto& p1 : stopTimesMapMap) { c2 += p1.second.size(); }
	cout << "  -  Parsed2 " << c2 << " stop times out of " << (STOP_TIMES_FILE_SMALL_SIZE - 1) << endl;
	fileStopTImes.close();
	//cout << "END PARSING STOPS. Imported " << db.getStopsSize() << " out of " << (STOPS_FILE_SIZE - 1) << endl << endl;

	// debug print
	/*for (auto &sm : stopTimesMapMap) {
		cout << "Trip ID " << sm.first << ":";
		for (auto& st : sm.second) {
			cout << " " << st.second.getStopSequenceNum() << " [" << st.second.getArrivalTimeTm().tm_hour << "." << st.second.getArrivalTimeTm().tm_min << "." << st.second.getArrivalTimeTm().tm_sec << "]";
		}
		cout << endl;
	}*/

	return true;
}

bool Simulator::importPoi(std::string poiFileName) {
	if (!setRandomPoint) {
		int rowCount;
		std::string str;

		cout << "START PARSING POI" << endl;

		std::ifstream filePoi(poiFileName, std::ifstream::in);
		if(!filePoi.is_open()) return false;

		std::getline(filePoi, str); // read first line
		rowCount = 1;
		while (std::getline(filePoi, str)) {
			Poi newPoi;

			str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
			str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

			if(newPoi.parseInput(str)){
				//if (	(newPoi.getPoiLatNum() >= stopsMinLat) &&
				//		(newPoi.getPoiLatNum() <= stopsMaxLat) &&
				//		(newPoi.getPoiLonNum() >= stopsMinLon) &&
				//		(newPoi.getPoiLonNum() <= stopsMaxLon)	){
				if (isInsideBoudaries(newPoi.getPoiLatNum(), newPoi.getPoiLonNum())) {
					poiMap[newPoi.getPoiIdNum()] = newPoi;
					cout << "Adding Poi -> " << str << endl;
				}
				else {
					cout << "POI " << newPoi.getPoiId() << " is outside the map" << endl;
				}
			}
			else {
				cerr << "Error parsing Poi -> " << str << endl;
				return EXIT_FAILURE;
			}
			rowCount++;
			fprintf(stdout, "\rPoi parsing: %.03f%%", ((((double) rowCount) / POI_FILE_SIZE) * 100.0));
		}
		cout << "  -  Parsed " << poiMap.size() << " poi out of " << (POI_FILE_SIZE - 1) << endl;
		filePoi.close();

		return true;
	}
	else {
		std::vector<Boundary *> boundaryVec;
		int noIdx = 0;
		for (auto& b : boundaryList) {
			if (noIdx == 1) continue;
			boundaryVec.push_back(&b);
			noIdx++;
		}

		cout << "Generating " << numberOfRandomPoint << " random POI" << endl;

		for (int i = 0; i < numberOfRandomPoint; i++) {
			Poi newPoi;
			int vecIdx = std::rand() % boundaryVec.size();

			bool toFIND = true;
			double poi_lat;
			double poi_lon;

			while (toFIND) {
				double f;

				f = ((double)rand()) / RAND_MAX;
				poi_lat = boundaryVec[vecIdx]->minLAT + f * (boundaryVec[vecIdx]->maxLAT - boundaryVec[vecIdx]->minLAT);

				f = ((double)rand()) / RAND_MAX;
				poi_lon = boundaryVec[vecIdx]->minLON + f * (boundaryVec[vecIdx]->maxLON - boundaryVec[vecIdx]->minLON);


				cout << "Random from " << boundaryVec[vecIdx]->minLAT << " to " << boundaryVec[vecIdx]->maxLAT  << " get " << poi_lat << " latitude" << endl;
				cout << "Random from " << boundaryVec[vecIdx]->minLON << " to " << boundaryVec[vecIdx]->maxLON  << " get " << poi_lon << " latitude" << endl;

				//poi_lat = Poi::fRand(boundaryVec[vecIdx]->minLAT, boundaryVec[vecIdx]->maxLAT);
				//poi_lon = Poi::fRand(boundaryVec[vecIdx]->minLON, boundaryVec[vecIdx]->maxLON);

				unsigned int cStops = countNeighStopLanLon(poi_lat, poi_lon);

				cout << "Poi at " << poi_lat << ";" << poi_lon << " has " << cStops << " stops" << endl;

				if (cStops > 2) {
					toFIND = false;
				}
			}


			newPoi.createRandom(i, poi_lat, poi_lon);

			poiMap[newPoi.getPoiIdNum()] = newPoi;
		}

		cout << "Created " << poiMap.size() << " poi" << endl;

		return true;
	}
}


bool Simulator::importHomes(std::string homesFileName) {
	int rowCount;
	std::string str;

	cout << "START PARSING HOMES" << endl;

	std::ifstream fileHomes(homesFileName, std::ifstream::in);
	if(!fileHomes.is_open()) return false;

	std::getline(fileHomes, str); // read first line
	rowCount = 0;
	while (std::getline(fileHomes, str)) {

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

		if (str.substr(0, 1).compare("#") == 0) {
			continue;
		}
		else {
			Home newHome;
			newHome.setSimulator(this);

			if(newHome.parseInput(str)){
				newHome.init(start_sim_time_tm);
				//newHome.wa.setDefaultWeight_grams(waDefaultW);
				//newHome.wa.setPacketInitNumber(waPckInitNum);
				//newHome.wa.setPacketGenerationRate(waPckGenRate);
				//newHome.setWA_parameters(waDefaultW, waPckInitNum, waPckGenRate);
				homesMap[newHome.getHomeIdNum()] = newHome;
			}
			else {
				cerr << "Error parsing Stops -> " << str << endl;
				return EXIT_FAILURE;
			}
		}

		rowCount++;
		fprintf(stdout, "\rHomes parsing: %d", rowCount);
	}
	//cout << endl;
	cout << "  -  Parsed " << homesMap.size() << " homes" << endl; // out of " << (STOPS_FILE_SMALL_SIZE - 1) << endl;
	fileHomes.close();
	//cout << "END PARSING STOPS. Imported " << db.getStopsSize() << " out of " << (STOPS_FILE_SIZE - 1) << endl << endl;

	return true;
}

bool Simulator::importDeliveryPoints(std::string deliverypointsFileName) {
	int rowCount;
	std::string str;

	cout << "START PARSING DELIVERY POINTS" << endl;

	std::ifstream fileDP(deliverypointsFileName, std::ifstream::in);
	if(!fileDP.is_open()) return false;

	std::getline(fileDP, str); // read first line
	rowCount = 0;
	while (std::getline(fileDP, str)) {

		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

		if (str.substr(0, 1).compare("#") == 0) {
			continue;
		}
		else {
			DeliveryPoint newDP;

			if(newDP.parseInput(str)){
				deliveryPointsMap[newDP.getDpIdNum()] = newDP;
			}
			else {
				cerr << "Error parsing Delivery Point -> " << str << endl;
				return EXIT_FAILURE;
			}
		}

		rowCount++;
		fprintf(stdout, "\rDelivery Points parsing: %d", rowCount);
	}
	//cout << endl;
	cout << "  -  Parsed " << deliveryPointsMap.size() << " delivery points" << endl; // out of " << (STOPS_FILE_SMALL_SIZE - 1) << endl;
	fileDP.close();
	//cout << "END PARSING STOPS. Imported " << db.getStopsSize() << " out of " << (STOPS_FILE_SIZE - 1) << endl << endl;

	return true;
}

bool Simulator::exportStops(std::string stopsFileName) {
	std::ofstream stopsStream(stopsFileName, std::ofstream::out);

	if (!stopsStream.is_open()) return false;

	stopsStream << "stop_id,stop_name,stop_lat,stop_lon,location_type,parent_station" << endl;

	for (auto& ss : stopsMap) {
		stopsStream << "\"" << ss.second.getStopId() << "\","
				<< "\"" << ss.second.getStopName() << "\","
				<< "\"" << ss.second.getStopLat() << "\","
				<< "\"" << ss.second.getStopLon() << "\","
				<< "\"" << ss.second.getLocationType() << "\","
				<< "\"" << ss.second.getParentStation() << "\""
				<< endl;
	}

	stopsStream.close();

	return true;
}

bool Simulator::exportTrips(std::string tripsFileName) {
	std::ofstream tripsStream(tripsFileName, std::ofstream::out);

	if (!tripsStream.is_open()) return false;

	tripsStream << "route_id,service_id,trip_id,direction_id,shape_id" << endl;

	for (auto& ss : tripsMap) {
		tripsStream << "\"" << ss.second.getRouteId() << "\","
				<< "\"" << ss.second.getServiceId() << "\","
				<< "\"" << ss.second.getTripId() << "\","
				<< "\"" << ss.second.getDirectionId() << "\","
				<< "\"" << ss.second.getShapeId() << "\""
				<< endl;
	}

	tripsStream.close();

	return true;
}

bool Simulator::exportCalendarDates(std::string calendarDAtesFileName) {
	std::ofstream cdStream(calendarDAtesFileName, std::ofstream::out);

	if (!cdStream.is_open()) return false;

	cdStream << "service_id,date,exception_type" << endl;

	for (auto& ss : calendarDateMap) {
		cdStream << "\"" << ss.second.getServiceId() << "\","
				<< "\"" << ss.second.getDate() << "\","
				<< "\"" << ss.second.getExceptionType() << "\""
				<< endl;
	}

	cdStream.close();

	return true;
}

bool Simulator::exportStopTimes(std::string stopTimesFileName) {
	std::ofstream stStream(stopTimesFileName, std::ofstream::out);

	if (!stStream.is_open()) return false;

	stStream << "trip_id,arrival_time,departure_time,stop_id,stop_sequence" << endl;

	for (auto& ss : stopTimesMap) {
		stStream << "\"" << ss.second.getTripId() << "\","
				<< "\"" << ss.second.getArrivalTime() << "\","
				<< "\"" << ss.second.getDepartureTime() << "\","
				<< "\"" << ss.second.getStopId() << "\","
				<< "\"" << ss.second.getStopSequence() << "\""
				<< endl;
	}

	stStream.close();

	return true;
}

bool Simulator::exportPoi(std::string poiFileName) {
	std::ofstream poiStream(poiFileName, std::ofstream::out);

	if (!poiStream.is_open()) return false;

	poiStream << "poi_id,poi_name,poi_lat,poi_lon,poi_type" << endl;

	for (auto& ss : poiMap) {
		poiStream << "\"" << ss.second.getPoiId() << "\","
				<< "\"" << ss.second.getPoiName() << "\","
				<< "\"" << ss.second.getPoiLat() << "\","
				<< "\"" << ss.second.getPoiLon() << "\","
				<< "\"" << ss.second.getPoiType() << "\""
				<< endl;
	}

	poiStream.close();

	return true;
}

bool Simulator::exportHomes(std::string homesFileName) {
	std::ofstream homesStream(homesFileName, std::ofstream::out);

	if (!homesStream.is_open()) return false;

	homesStream << "home_id,home_name,home_lat,home_lon,wa_pkt_w,wa_pkt_init,wa_pkt_genrate,charg_num,charg_batt_init_num,charg_batt_init_val" << endl;

	for (auto& ss : homesMap) {
		homesStream << "\"" << ss.second.getHomeIdNum() << "\","
				<< "\"" << ss.second.getHomeName() << "\","
				<< "\"" << ss.second.getHomeLat() << "\","
				<< "\"" << ss.second.getHomeLon() << "\","
				<< "\"" << ss.second.getHomeWaDefpktW() << "\","
				<< "\"" << ss.second.getHomeWaPktInitnum() << "\","
				<< "\"" << ss.second.getHomeWaPktGenrate() << "\","
				<< "\"" << ss.second.getHomeChargNum() << "\","
				<< "\"" << ss.second.getHomeChargBattInitnum() << "\","
				<< "\"" << ss.second.getHomeChargBattInitval() << "\""
				<< endl;
	}

	homesStream.close();

	return true;
}

bool Simulator::exportDeliveryPoints(std::string deliverypointsFileName) {
	std::ofstream deliverypointsStream(deliverypointsFileName, std::ofstream::out);

	if (!deliverypointsStream.is_open()) return false;

	deliverypointsStream << "dp_id,dp_name,dp_lat,dp_lon" << endl;

	for (auto& ss : deliveryPointsMap) {
		deliverypointsStream << "\"" << ss.second.getDpId() << "\","
				<< "\"" << ss.second.getDpName() << "\","
				<< "\"" << ss.second.getDpLat() << "\","
				<< "\"" << ss.second.getDpLon() << "\""
				<< endl;
	}

	deliverypointsStream.close();

	return true;
}

bool Simulator::exportDotResult(std::string dotFileName) {
	return flowGraph->exportDotResult(dotFileName);
}

bool Simulator::generateBusRoute(void) {
	bool ris = true;

	busRouteMap.clear();

	cout << "START GENERATING BUS ROUTE" << endl;

	int rowCount = 0;
	for (auto& trip : tripsMap) {
		BusRoute newRoute;
		Trips *t = &(trip.second);
		newRoute.route_id = t->getTripId();
		newRoute.bus_id = t->getRouteId();

		rowCount++;
		fprintf(stdout, "\rBus Route generating: %.03f%%", ((((double) rowCount) / ((double) tripsMap.size())) * 100.0));

		for (auto& stop_t : stopTimesMapMap[t->getTripId()]) {
			BusRoute::Route_Node newRN;
			StopTimes *st = &(stop_t.second);

			newRN.arrival_time = operationalDay_tm;
			newRN.arrival_time.tm_hour = st->getArrivalTimeTm().tm_hour;
			newRN.arrival_time.tm_min = st->getArrivalTimeTm().tm_min;
			newRN.arrival_time.tm_sec = st->getArrivalTimeTm().tm_sec;
			if (st->isArrivalNextDay()) {
				newRN.arrival_time.tm_mday += 1;
				newRN.arrival_time.tm_wday += 1;
				newRN.arrival_time.tm_yday += 1;
			}

			newRN.departure_time = operationalDay_tm;
			newRN.departure_time.tm_hour = st->getDepartureTimeTm().tm_hour;
			newRN.departure_time.tm_min = st->getDepartureTimeTm().tm_min;
			newRN.departure_time.tm_sec = st->getDepartureTimeTm().tm_sec;
			if (st->isDepartureNextDay()) {
				newRN.departure_time.tm_mday += 1;
				newRN.departure_time.tm_wday += 1;
				newRN.departure_time.tm_yday += 1;
			}

			newRN.stop_id = st->getStopIdNum();
			newRN.stop_sequence = st->getStopSequenceNum();

			newRoute.orderedList_Stops.push_back(newRN);
		}

		/*if (newRoute.orderedList_Stops.size() == 0) {
			cerr << "Route " << newRoute.bus_id << "[" << newRoute.route_id << "] without stops" << endl;
			ris = false;
		}*/

		newRoute.orderedList_Stops.sort(compare_Route);

		bool addRouteOK = true;

		// check sequential dates
		for (auto itStops = newRoute.orderedList_Stops.begin(); itStops != newRoute.orderedList_Stops.end(); itStops++) {
			auto itStopFollow = itStops;
			itStopFollow++;
			if (itStopFollow != newRoute.orderedList_Stops.end()) {
				double secondsToBus = difftime(mktime(&(itStopFollow->arrival_time)), mktime(&(itStops->arrival_time)));
				if (secondsToBus <= 0) {
					//cerr << "Wrong timetable for route " << newRoute.bus_id << "[" << newRoute.route_id << "] without stops: ";// << endl;
					//for (auto& rs : newRoute.orderedList_Stops) {
					//	cerr << " " << rs.stop_id << "[" << rs.arrival_time.tm_hour << "." << rs.arrival_time.tm_min << "." << rs.arrival_time.tm_sec << "]";
					//}
					//cerr << endl;

					//ris = false;
					addRouteOK = false;
				}
			}
		}

		if (addRouteOK) {

			if (newRoute.orderedList_Stops.size() > 1) {
				busRouteMap[newRoute.route_id] = newRoute;
			}
		}
	}

	// debug print
	/*for (auto &r : busRouteMap) {
		cout << "Route " << r.second.bus_id << "[" << r.second.route_id << "]:";
		for (auto& rs : r.second.orderedList_Stops) {
			cout << " " << rs.stop_id << "[" << rs.arrival_time.tm_hour << "." << rs.arrival_time.tm_min << "." << rs.arrival_time.tm_sec << "]";
		}
		cout << endl;
	}*/

	/*std::tm tm_min = operationalDay_tm;
	std::tm tm_max = operationalDay_tm;
	tm_min.tm_year = 300;
	tm_max.tm_year = 1;
	for (auto &r : busRouteMap) {
		//cout << "Route " << r.second.bus_id << "[" << r.second.route_id << "]:";
		for (auto& rs : r.second.orderedList_Stops) {
			//cout << " " << rs.stop_id << "[" << rs.arrival_time.tm_hour << "." << rs.arrival_time.tm_min << "." << rs.arrival_time.tm_sec << "]";
			if(difftime(mktime(&tm_min), mktime(&rs.arrival_time)) > 0) {
				tm_min = rs.arrival_time;
			}
			if(difftime(mktime(&tm_max), mktime(&rs.arrival_time)) < 0) {
				tm_max = rs.arrival_time;
			}
		}
		//cout << endl;
	}
	cout << endl << "Time min: " << tm_min.tm_hour << "." << tm_min.tm_min << "." << tm_min.tm_sec << "]" << endl;
	char buffer1[64];
	char buffer2[64];
	std::strftime(buffer1, 64, "%a, %d.%m.%Y %H:%M:%S", &tm_min);
	std::strftime(buffer2, 64, "%a, %d.%m.%Y %H:%M:%S", &tm_max);
	cout << buffer1 << " " << buffer2 << endl;
	cout << endl << "Time max: " << tm_max.tm_hour << "." << tm_max.tm_min << "." << tm_max.tm_sec << "]" << endl;*/

	cout << endl << "END GENERATING BUS ROUTE" << endl;

	return ris;
}

void Simulator::getNeighPoi(unsigned int stopID, std::vector<Poi *> &neighPoi) {
	neighPoi.clear();
	if (stopsMap.count(stopID) > 0) {
		Stops *ss = &stopsMap[stopID];
		for (auto& p : poiMap) {
			Poi *ppt = &(p.second);
			if (distanceEarth(ppt->getPoiLatNum(), ppt->getPoiLatNum(), ss->getStopLatNum(), ss->getStopLonNum()) <= maxDistancePoiStop) {
				neighPoi.push_back(ppt);
			}
		}
	}
}

void Simulator::getNeighStop(unsigned int poiID, std::vector<Stops *> &neighStops) {
	neighStops.clear();
	if (poiMap.count(poiID) > 0) {
		Poi *ppt = &poiMap[poiID];
		for (auto& s : stopsMap) {
			Stops *ss = &(s.second);
			double dist = distanceEarth(ppt->getPoiLatNum(), ppt->getPoiLonNum(), ss->getStopLatNum(), ss->getStopLonNum());
			//cout << "Distance from "
			//		<< "POI: " << ppt->getPoiIdNum() << "[" << ppt->getPoiLatNum() << "," << ppt->getPoiLonNum() << "]"
			//		<< " and Stop: " << ss->getStopIdNum()  << "[" << ss->getStopLatNum() << "," << ss->getStopLonNum() << "]"
			//		<< " is: " << dist << endl;
			if (dist <= maxDistancePoiStop) {
				neighStops.push_back(ss);
			}
		}
	}
}

unsigned int Simulator::countNeighStopLanLon(double lan, double lon) {
	unsigned int ris = 0;

	for (auto& s : stopsMap) {
		Stops *ss = &(s.second);
		double dist = distanceEarth(lan, lon, ss->getStopLatNum(), ss->getStopLonNum());

		if (dist <= maxDistancePoiStop) {
			ris++;
		}
	}

	return ris;
}

void Simulator::updateBatteries(Uav *u) {
	Poi::POI_TYPE_T pt;
	Uav::UAV_STATE us;

	us = u->getState();

	switch (us) {
		case Uav::UAV_ONBUS:
			u->addEnergy(eRECHARGING);
			u->addTimeOnBus(1);
			break;

		case Uav::UAV_FLYING:
		default:
			if (u->isCovering()) {
				pt = poiMap[u->getCoveringPoiId()].getPoiT();
				switch (pt) {
				case Poi::POI_TYPE_T::RELAYING:
					u->addEnergy(eRELAY);
					u->addTimeInPoiRel(1);
					break;
				case Poi::POI_TYPE_T::RECORDING:
					u->addEnergy(eRECORDING);
					u->addTimeInPoiRec(1);
					break;
				case Poi::POI_TYPE_T::RECORDING_RELAYING:
					u->addEnergy(eRECORDINGRELAY);
					u->addTimeInPoiRecRel(1);
					break;
				}
			}
			else {
				u->addEnergy(eSTOP);
				u->addTimeInStop(1);
			}

			break;
	}
}

void Simulator::generateGraph(struct std::tm start_time, struct std::tm end_time) {
	//char buffer[64];
	struct std::tm before_time;
	struct std::tm act_time = start_time;
	double totSimTime;
	//struct std::tm end_time = start_time;

	//act_time.tm_hour = 0;
	//act_time.tm_min = 0;
	//act_time.tm_sec = 0;

	//end_time.tm_hour = 23;
	//end_time.tm_min = 59;
	//end_time.tm_sec = 59;

	totSimTime = difftime(mktime(&end_time), mktime(&start_time));

	before_time = act_time;

	cout << "START GENERATING GRAPH NODES" << endl;

	for (auto& st : stopsMap) {
		flowGraph->addInitStop(st.second.getStopIdNum(), act_time);
	}
	for (auto& ho : homesMap) {
		flowGraph->addInitHome(ho.second.getHomeIdNum(), act_time);
	}
	for (auto& dp : deliveryPointsMap) {
		flowGraph->addInitDeliveryPoint(dp.second.getDpIdNum(), act_time);
	}
	act_time.tm_sec = act_time.tm_sec + 1;
	mktime(&act_time);

	while (difftime(mktime(&end_time), mktime(&act_time)) >= 0) {

		//struct std::tm before_time = act_time;

		for (auto& st : stopsMap) {
			flowGraph->addFollowingStop(st.second.getStopIdNum(), act_time);
			flowGraph->generateStaticArcs(st.second.getStopIdNum(), before_time, act_time, ArcGraph::STOP);
		}
		for (auto& hh : homesMap) {
			flowGraph->addFollowingHome(hh.second.getHomeIdNum(), act_time);
		}
		for (auto& dp : deliveryPointsMap) {
			flowGraph->addFollowingDeliveryPoint(dp.second.getDpIdNum(), act_time);
		}

		for (auto& poi : poiMap) {
			std::vector<Stops *> stPoi;

			getNeighStop(poi.second.getPoiIdNum(), stPoi);
			for (auto& st : stPoi) {
				flowGraph->generateStaticArcs(st->getStopIdNum(), before_time, act_time, ArcGraph::COVER, &(poi.second));
			}
		}

		//std::strftime(buffer, sizeof(buffer), "%a, %d.%m.%Y %H:%M:%S", &act_time);
		//cout << "Clock time " << buffer << endl;
		//struct std::tm ttt = flowGraph.day_seconds2tm(flowGraph.day_tm2seconds(act_time));
		//std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &ttt);
		//cout << " - seconds: " << flowGraph.day_tm2seconds(act_time) << " - " << buffer << endl;
		//fprintf(stdout, "\rGenerating graph nodes %.03f%%", ((((double) FlowGraph::day_tm2seconds(act_time)) / totSimTime) * 100.0));
		fprintf(stdout, "\rGenerating graph nodes %.03f%%", (100.0 - ((((double) difftime(mktime(&end_time), mktime(&act_time))) / totSimTime) * 100.0)));

		//std::strftime(buffer, sizeof(buffer), " %a, %d.%m.%Y %H:%M:%S", &before_time);
		//cout << " - From time " << buffer;
		//std::strftime(buffer, sizeof(buffer), " %a, %d.%m.%Y %H:%M:%S", &act_time);
		//cout << " to time " << buffer << endl;

		//before_time.tm_sec = before_time.tm_sec + 1;
		//mktime(&before_time);
		before_time = act_time;

		act_time.tm_sec = act_time.tm_sec + 1;
		mktime(&act_time);
	}

	cout << endl << "END GENERATING GRAPH NODES" << endl; fflush(stdout);

	//flowGraph.generateStaticArcs();

	cout << "START GENERATING BUSES ARCS" << endl; fflush(stdout);
	int c = 0;
	for (auto& r : busRouteMap) {
		BusRoute *br = &(r.second);
		flowGraph->generateStaticArcsFromRoute(br, start_time, end_time);

		c++;
		fprintf(stdout, "\rGenerating buses arcs %.03f%%", (((double) c) / ((double) busRouteMap.size())) * 100.0);
	}
	cout << endl << "END GENERATING BUSES ARCS" << endl; fflush(stdout);
}

//bool Simulator::init(std::string tpsFileName) {
bool Simulator::init(void) {

	bool ris = true;


	cout << "START GENERATING GRAPH" << endl; fflush(stdout);
	generateGraph(start_sim_time_tm, end_sim_time_tm);
	cout << "END GENERATING GRAPH" << endl; fflush(stdout);

	cout << "BEGIN INIT FLOWGRAPH" << endl; fflush(stdout);
	flowGraph->initPoi(poiMap, tpsFileName);
	cout << "END INIT FLOWGRAPH" << endl; fflush(stdout);

	// init the UAVs
	cout << "INIT THE " << nUAV << " UAVs" << endl; fflush(stdout);
	for (unsigned int i = 0; i < nUAV; i++) {
		Uav *newUav = new Uav();

		newUav->setResudualEnergy(initialUavEnergy);
		listUav.push_back(newUav);
	}

	if (nUAV < poiMap.size()) {
		cerr << "Number of UAV: " << nUAV << ", number of POIs: " << poiMap.size() << endl;
		ris = false;
	}
	else {
		Uav *uav;
		auto itUAV = listUav.begin();

		for (auto& p : poiMap) {
			std::vector<Stops *> lStops;
			uav = *itUAV;
			Poi *ppt = &(p.second);

			uav->setState(Uav::UAV_FLYING);
			uav->setCoveringPoiId(ppt->getPoiIdNum());
			ppt->setCoveringUav(uav);

			getNeighStop(ppt->getPoiIdNum(), lStops);
			if (lStops.size() > 0) {
				std::random_shuffle ( lStops.begin(), lStops.end() );
				uav->setPositionStopId(lStops[0]->getStopIdNum());

				flowGraph->setUavPosition(start_sim_time_tm, uav);

				cout << "UAV " << uav->getId() << " init on POI: " << ppt->getPoiIdNum() << ". Set on stop: " << uav->getPositionStopId() << endl;
			}
			else {
				cerr << "POI: " << ppt->getPoiIdNum() << " not covered by any stop" << endl;
				ris = false;
				break;
			}

			itUAV++;
		}

		if (ris) { // tutto ok...
			std::list <Uav *> remainingUAV;
			while (itUAV != listUav.end()) {
				remainingUAV.push_back(*itUAV);
				//uav = *itUAV;
				//flowGraph->setInitExtraUAV(uav, start_sim_time_tm, stopsMap);
				itUAV++;
			}
			flowGraph->setInitExtraUAV(remainingUAV, start_sim_time_tm, stopsMap);

			/*std::vector<Stops *> lStops;
			for (auto& ss : stopsMap) {
				lStops.push_back(&(ss.second));
			}
			std::random_shuffle ( lStops.begin(), lStops.end() );

			//setto gli altri in modo randomico
			while (itUAV != listUav.end()) {
				uav = *itUAV;

				uav->setState(Uav::UAV_FLYING);
				uav->unsetCovering();
				uav->setPositionStopId(lStops[(std::rand() % lStops.size())]->getStopIdNum());

				flowGraph->setUavPosition(start_sim_time_tm, uav);

				cout << "UAV " << uav->getId() << " set on stop: " << uav->getPositionStopId() << endl;

				itUAV++;
			}*/
		}

		// Put the UAVs on the Flow Graph
	}
	cout << "END SETTING THE UAVs" << endl; fflush(stdout);

	return ris;
}

void Simulator::run(void) {
	char buffer[64];
	unsigned int t = 0;
	struct std::tm t_tm = start_sim_time_tm;
	bool alive = true;

	do {
	//for (unsigned int t = 0; t < maxTime; t++){
		//cout << "Simulation time: " << t << endl;

		std::strftime(buffer, sizeof(buffer), " %a, %d.%m.%Y %H:%M:%S", &t_tm);
		fprintf(stdout, "\rSimulation time: %u seconds - %s", t, buffer);fflush(stdout);

		//flowGraph.execute(t, listUav);
		flowGraph->execute(t_tm, listUav);

		//updateThe batteries
		for(auto& uav : listUav) {
			updateBatteries(uav);
		}

		// check the lifetime
		for(auto& uav : listUav) {
			if (uav->getResudualEnergy() <= 0) {
				alive = false;
				break;
			}
		}

		t++;
		t_tm.tm_sec = t_tm.tm_sec + 1;
		mktime(&t_tm);
	//} while ((t < maxTime) && alive);
	} while ((difftime(mktime(&end_sim_time_tm), mktime(&t_tm)) >= 0) && alive);
	cout << endl;

	finalLifetime = t;
	//cout << "System lifetime: " << finalLifetime << endl;
}


void Simulator::stats(std::string outFileName) {
	std::ofstream statStream(outFileName, std::ofstream::out);
	bool fOutOK = statStream.is_open();

	cout << "FINAL STATS: " << endl;

	cout << "SYSTEM LIFETIME: " << finalLifetime << " sec" << endl;
	cout << "SYSTEM LIFETIME: " << finalLifetime/60.0 << " min" << endl;

	if (fOutOK) statStream << "LIFETIME_MIN " << finalLifetime/60.0 << endl;
	if (fOutOK) statStream << "LIFETIME_SEC " << finalLifetime << endl;


	cout << "TOTAL SWAP COUNT: " << flowGraph->getSwapCount() << endl;
	if (fOutOK) statStream << "TOTAL_SWAP_COUNT " << flowGraph->getSwapCount() << endl;
	if (fOutOK) statStream << "TOTAL_SWAP_COUNT_PERUAV " << ((double)flowGraph->getSwapCount()) / ((double) listUav.size()) << endl;
	if (fOutOK) statStream << "TOTAL_SWAP_COUNT_PERUAV_AVGINTIME " << (((double)flowGraph->getSwapCount()) / ((double) listUav.size())) / ((double) finalLifetime) << endl;

	double sumStop, sumBus, sumPoiRel, sumPoiRec, sumPoiRelRec, sumALL;
	sumStop = sumBus = sumPoiRel = sumPoiRec = sumPoiRelRec = 0;

	for (auto& uav : listUav) {
		sumStop += uav->getTimeInStop();
		sumBus += uav->getTimeOnBus();
		sumPoiRel += uav->getTimeInPoiRel();
		sumPoiRec += uav->getTimeInPoiRec();
		sumPoiRelRec += uav->getTimeInPoiRecRel();
	}
	sumALL = sumStop + sumBus + sumPoiRel + sumPoiRec + sumPoiRelRec;

	cout << "UAVs LIFESTYLE: "
			<< "Stop " << (sumStop / sumALL) * 100.0 << "%; "
			<< "Bus " << (sumBus / sumALL) * 100.0 << "%; "
			<< "PoiRelay " << (sumPoiRel / sumALL) * 100.0 << "%; "
			<< "PoiRecord " << (sumPoiRec / sumALL) * 100.0 << "%; "
			<< "PoiRecordRelay " << (sumPoiRelRec / sumALL) * 100.0 << "%"
			<< endl;

	if (fOutOK) statStream << "UAV_LIFESTYLE "
			<< "Stop " << (sumStop / sumALL) * 100.0 << " "
			<< "Bus " << (sumBus / sumALL) * 100.0 << " "
			<< "PoiRelay " << (sumPoiRel / sumALL) * 100.0 << " "
			<< "PoiRecord " << (sumPoiRec / sumALL) * 100.0 << " "
			<< "PoiRecordRelay " << (sumPoiRelRec / sumALL) * 100.0
			<< endl;

	for (auto& uav : listUav) {
		double sumUavAll = uav->getTimeInStop() + uav->getTimeOnBus() +
				uav->getTimeInPoiRel() + uav->getTimeInPoiRec() + uav->getTimeInPoiRecRel();

		cout << "   UAV " << uav->getId() << " LIFESTYLE: "
				<< "Stop " << ( ((double) uav->getTimeInStop()) / sumUavAll) * 100.0 << "%; "
				<< "Bus " << (((double) uav->getTimeOnBus()) / sumUavAll) * 100.0 << "%; "
				<< "PoiRelay " << (((double) uav->getTimeInPoiRel()) / sumUavAll) * 100.0 << "%; "
				<< "PoiRecord " << (((double) uav->getTimeInPoiRec()) / sumUavAll) * 100.0 << "%; "
				<< "PoiRecordRelay " << (((double) uav->getTimeInPoiRecRel()) / sumUavAll) * 100.0 << "%"
				<< endl;

		if (fOutOK) statStream << "UAV_N" << uav->getId() << "_LIFESTYLE: "
						<< "Stop " << ( ((double) uav->getTimeInStop()) / sumUavAll) * 100.0 << " "
						<< "Bus " << (((double) uav->getTimeOnBus()) / sumUavAll) * 100.0 << " "
						<< "PoiRelay " << (((double) uav->getTimeInPoiRel()) / sumUavAll) * 100.0 << " "
						<< "PoiRecord " << (((double) uav->getTimeInPoiRec()) / sumUavAll) * 100.0 << " "
						<< "PoiRecordRelay " << (((double) uav->getTimeInPoiRecRel()) / sumUavAll) * 100.0
						<< endl;
	}

	double avgResidualEnergy, maxResidualEnergy, sumResidualEnergy;
	maxResidualEnergy = std::numeric_limits<double>::min();
	for (auto& uav : listUav) {
		sumResidualEnergy += uav->getResudualEnergy();
		if (uav->getResudualEnergy() > maxResidualEnergy) maxResidualEnergy = uav->getResudualEnergy();
	}
	avgResidualEnergy = sumResidualEnergy / ((double) listUav.size());
	cout << "UAVs FINAL RESIDUAL ENERGY AVERAGE: " << avgResidualEnergy << "; MAX: " << maxResidualEnergy <<  endl;
	if (fOutOK) statStream << "UAV_FINAL_RESIDUAL_ENERGY AVERAGE " << avgResidualEnergy << " MAX " << maxResidualEnergy << endl;
	for (auto& uav : listUav) {
		cout << "   UAV " << uav->getId() << " FINAL RESIDUAL ENERGY: " << uav->getResudualEnergy() << endl;
		if (fOutOK) statStream << "UAV_N" << uav->getId() << "_FINAL_RESIDUAL_ENERGY AVERAGE " << uav->getResudualEnergy() << endl;
	}

	if (fOutOK) statStream.close();
}



