/*
 * StopTimes.cpp
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#include <sstream>      // std::stringstream
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm

#include "StopTimes.h"

StopTimes::StopTimes() {
	// TODO Auto-generated constructor stub
	arrival_next_day =  false;
	departure_next_day = false;

}

StopTimes::~StopTimes() {
	// TODO Auto-generated destructor stub
}

bool StopTimes::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	std::size_t pos;
	std::stringstream ss;
	int h, m, s;
	char buff[32];
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case STOPTIMES_TRIP_ID:
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
		case STOPTIMES_ARRIVAL_TIME:
			sscanf(t.c_str(), "%d:%d:%d", &h, &m, &s);
			if (h > 23) {
				h = h - 24;
				arrival_next_day = true;
			}
			snprintf(buff, sizeof(buff), "%02d:%02d:%02d", h, m, s);
			arrival_time = std::string(buff);
			//cout << "Old -> " << t << "New -> " << arrival_time << endl;
			//arrival_time = t;
			ss << arrival_time;
			ss >> std::get_time(&arrival_time_tm,"%H:%M:%S");
			/*arrival_time_tm.tm_year = 117;
			arrival_time_tm.tm_mon = 1;
			arrival_time_tm.tm_mday = 1;*/
			break;
		case STOPTIMES_DEPARTURE_TIME:
			sscanf(t.c_str(), "%d:%d:%d", &h, &m, &s);
			if (h > 23) {
				h = h - 24;
				departure_next_day = true;
			}
			snprintf(buff, sizeof(buff), "%02d:%02d:%02d", h, m, s);
			departure_time = std::string(buff);
			//departure_time = t;
			ss << departure_time;
			ss >> std::get_time(&departure_time_tm,"%H:%M:%S");
			/*departure_time_tm.tm_year = 117;
			departure_time_tm.tm_mon = 1;
			departure_time_tm.tm_mday = 1;*/
			break;
		case STOPTIMES_STOP_ID:
			stop_id = t;
			stop_id_num = atol(stop_id.c_str());

			break;
		case STOPTIMES_STOP_SEQUENCE:
			stop_sequence = t;
			stop_sequence_num = atol(stop_sequence.c_str());
			break;
		default:
			cerr << "Too much columns in Stop Times file" << endl;
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

