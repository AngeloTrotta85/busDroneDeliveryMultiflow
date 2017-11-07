/*
 * CalendarDate.h
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#ifndef CALENDARDATE_H_
#define CALENDARDATE_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

class CalendarDate {
public:
	typedef enum {
		CALENDARDATE_SERVICE_ID = 0,
		CALENDARDATE_DATE,
		CALENDARDATE_EXCEPTION_TYPE
	} ENUM_CALENDARDATE;

public:
	CalendarDate();
	virtual ~CalendarDate();

	bool parseInput(const std::string toParse);

	const std::string& getDate() const { return date; }
	const std::string& getExceptionType() const { return exception_type; }
	const std::string& getServiceId() const { return service_id; }

	unsigned long int getServiceIdNumPrefix() const {		return service_id_num_prefix;	}
	unsigned long int getServiceIdNumSuffix() const {		return service_id_num_suffix;	}
	unsigned long int getDateNum() const {		return date_num;	}

private:
	std::string service_id;
	std::string date;
	std::string exception_type;

	unsigned long int service_id_num_prefix;
	unsigned long int service_id_num_suffix;
	unsigned long int date_num; //TODO is a date
};

#endif /* CALENDARDATE_H_ */
