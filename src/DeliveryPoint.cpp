/*
 * DeliveryPoint.cpp
 *
 *  Created on: 17 ott 2017
 *      Author: angelo
 */

#include "DeliveryPoint.h"

DeliveryPoint::DeliveryPoint() {
	// TODO Auto-generated constructor stub

}

DeliveryPoint::~DeliveryPoint() {
	// TODO Auto-generated destructor stub
}

bool DeliveryPoint::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case DP_ID:
			dp_id = t;
			dp_id_num = atol(dp_id.c_str());
			break;
		case DP_NAME:
			dp_name = t;
			break;
		case DP_LAT:
			dp_lat = t;
			dp_lat_num = atof(dp_lat.c_str());
			break;
		case DP_LON:
			dp_lon = t;
			dp_lon_num = atof(dp_lon.c_str());
			break;
		default:
			cerr << "Too much columns in Delivery Point file" << endl;
			ris = false;
			break;
		}

		idx++;
	}

	if (idx != 4) {
		ris = false;
	}

	return ris;
}

