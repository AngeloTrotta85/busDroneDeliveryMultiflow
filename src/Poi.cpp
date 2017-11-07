/*
 * Poi.cpp
 *
 *  Created on: 17 lug 2017
 *      Author: angelo
 */

#include "Poi.h"

int Poi::counterRandom = 0;

Poi::Poi() {
	// TODO Auto-generated constructor stub
	coveringUav = nullptr;
	counterRandom = 0;
}

Poi::~Poi() {
	// TODO Auto-generated destructor stub
}

bool Poi::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	int tmp;
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case POI_POI_ID:
			poi_id = t;
			poi_id_num = atol(poi_id.c_str());
			break;

		case POI_NAME:
			poi_name = t;
			break;

		case POI_LAT:
			poi_lat = t;
			poi_lat_num = atof(poi_lat.c_str());
			break;

		case POI_LON:
			poi_lon = t;
			poi_lon_num = atof(poi_lon.c_str());
			break;

		case POI_TYPE:
			poi_type = t;
			tmp = atoi(poi_type.c_str());
			if (tmp == 0) {
				poi_t = RECORDING;
			}
			else if (tmp == 1){
				poi_t = RELAYING;
			}
			else {
				poi_t = RECORDING_RELAYING;
			}
			break;

		default:
			cerr << "Too much columns in Stops file" << endl;
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

void Poi::createRandom(unsigned int id, double lat, double lon) {
	char buff[32];

	snprintf(buff, sizeof(buff), "%d", id);
	poi_id = std::string(buff);
	poi_id_num = id;

	snprintf(buff, sizeof(buff), "POI%u", id);
	poi_name = std::string(buff);

	poi_lat = lat;
	poi_lat_num = atof(poi_lat.c_str());

	poi_lon = lon;
	poi_lon_num = atof(poi_lon.c_str());

	int randomType = rand()%100;

	if (randomType <= 70) {
		poi_type = std::string("0");
		poi_t = RECORDING;
	}
	else if (randomType <= 90) {
		poi_type = std::string("1");
		poi_t = RELAYING;
	}
	else {
		poi_type = std::string("2");
		poi_t = RECORDING_RELAYING;
	}

	counterRandom++;
}

