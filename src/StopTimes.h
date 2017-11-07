/*
 * StopTimes.h
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#ifndef STOPTIMES_H_
#define STOPTIMES_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <boost/tokenizer.hpp>
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm

using namespace std;
using namespace boost;

class StopTimes {
public:
	typedef enum {
		STOPTIMES_TRIP_ID = 0,
		STOPTIMES_ARRIVAL_TIME,
		STOPTIMES_DEPARTURE_TIME,
		STOPTIMES_STOP_ID,
		STOPTIMES_STOP_SEQUENCE
	} ENUM_STOPTIMES;

public:
	StopTimes();
	virtual ~StopTimes();

	bool parseInput(const std::string toParse);

	const std::string& getArrivalTime() const { return arrival_time; }
	const std::string& getDepartureTime() const { return departure_time; }
	const std::string& getStopId() const { return stop_id; }
	const std::string& getStopSequence() const { return stop_sequence; }
	const std::string& getTripId() const { return trip_id; }

	unsigned long int getStopIdNum() const {		return stop_id_num;	}
	unsigned long int getTripIdNumPrefix() const {		return trip_id_num_prefix;	}
	unsigned long int getTripIdNumSuffix() const {		return trip_id_num_suffix;	}
	unsigned long int getStopSequenceNum() const {		return stop_sequence_num;	}

	const struct std::tm& getArrivalTimeTm() const {		return arrival_time_tm;	}
	const struct std::tm& getDepartureTimeTm() const {		return departure_time_tm;	}

	bool isArrivalNextDay() const {		return arrival_next_day;	}
	bool isDepartureNextDay() const {		return departure_next_day;	}

private:
	std::string trip_id;
	std::string arrival_time;
	std::string departure_time;
	std::string stop_id;
	std::string stop_sequence;

	struct std::tm arrival_time_tm;
	struct std::tm departure_time_tm;


	unsigned long int stop_id_num;
	unsigned long int trip_id_num_prefix;
	unsigned long int trip_id_num_suffix;
	unsigned long int stop_sequence_num;

	bool arrival_next_day;
	bool departure_next_day;
};

#endif /* STOPTIMES_H_ */
