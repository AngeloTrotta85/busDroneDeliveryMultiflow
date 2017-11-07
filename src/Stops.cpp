/*
 * Stops.cpp
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#include "Stops.h"

Stops::Stops() {
	// TODO Auto-generated constructor stub

}

Stops::~Stops() {
	// TODO Auto-generated destructor stub
}

bool Stops::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case STOPS_STOP_ID:
			stop_id = t;
			stop_id_num = atol(stop_id.c_str());
			break;
		case STOPS_STOP_NAME:
			stop_name = t;
			break;
		case STOPS_STOP_LAT:
			stop_lat = t;
			stop_lat_num = atof(stop_lat.c_str());
			break;
		case STOPS_STOP_LON:
			stop_lon = t;
			stop_lon_num = atof(stop_lon.c_str());
			break;
		case STOPS_LOCATION_TYPE:
			location_type = t;
			break;
		case STOPS_PARENT_STATION:
			parent_station = t;
			break;
		default:
			cerr << "Too much columns in Stops file" << endl;
			ris = false;
			break;
		}

		idx++;
	}

	if (idx != 6) {
		ris = false;
	}

	return ris;
}

