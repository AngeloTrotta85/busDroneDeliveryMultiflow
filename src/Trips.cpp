/*
 * Trips.cpp
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#include "Trips.h"

Trips::Trips() {
	route_id_num = service_id_num_prefix = service_id_num_suffix = 0;
	trip_id_num_prefix = trip_id_num_suffix = direction_id_num = shape_id_num = 0;
}

Trips::~Trips() { }

bool Trips::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	std::size_t pos;
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case TRIPS_ROUTE_ID:
			route_id = t;
			route_id_num = atol(route_id.c_str());
			break;
		case TRIPS_SERVICE_ID:
			service_id = t;
			pos = service_id.find("_");
			if (pos != std::string::npos) {
				service_id_num_prefix = atol(service_id.substr(0, pos).c_str());
				service_id_num_suffix = atol(service_id.substr(pos+1).c_str());
			}
			else {
				service_id_num_prefix = service_id_num_suffix = atol(service_id.c_str());
			}
			break;
		case TRIPS_TRIP_ID:
			trip_id = t;
			pos = trip_id.find("_");
			if (pos != std::string::npos) {
				trip_id_num_prefix = atol(trip_id.substr(0, pos).c_str());
				trip_id_num_suffix = atol(trip_id.substr(pos+1).c_str());
			}
			else {
				trip_id_num_prefix = trip_id_num_suffix = atol(trip_id.c_str());
			}
			break;
		case TRIPS_DIRECTION_ID:
			direction_id = t;
			direction_id_num = atol(direction_id.c_str());
			break;
		case TRIPS_SHAPE_ID:
			shape_id = t;
			shape_id_num = atol(shape_id.c_str());
			break;
		default:
			cerr << "Too much columns in Trips file" << endl;
			ris = false;
			break;
		}

		idx++;
	}

	if (idx != 5) {
		ris = false;
	}

	return ris;
}

