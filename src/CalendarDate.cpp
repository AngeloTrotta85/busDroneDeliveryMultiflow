/*
 * CalendarDate.cpp
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#include "CalendarDate.h"

CalendarDate::CalendarDate() {
	// TODO Auto-generated constructor stub

}

CalendarDate::~CalendarDate() {
	// TODO Auto-generated destructor stub
}

bool CalendarDate::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	std::size_t pos;
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case CALENDARDATE_SERVICE_ID:
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
		case CALENDARDATE_DATE:
			date = t;
			date_num = atol(date.c_str());
			break;
		case CALENDARDATE_EXCEPTION_TYPE:
			exception_type = t;
			break;
		default:
			cerr << "Too much columns in CalendarDate file" << endl;
			ris = false;
			break;
		}

		idx++;
	}

	if (idx != 3) {
		ris = false;
	}

	return ris;
}

