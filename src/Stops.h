/*
 * Stops.h
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#ifndef STOPS_H_
#define STOPS_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <boost/tokenizer.hpp>
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm
#include <list>        // struct std::tm

using namespace std;
using namespace boost;

class Stops {
public:
	typedef enum {
		STOPS_STOP_ID = 0,
		STOPS_STOP_NAME,
		STOPS_STOP_LAT,
		STOPS_STOP_LON,
		STOPS_LOCATION_TYPE,
		STOPS_PARENT_STATION
	} ENUM_STOPS;

	typedef struct StopArrivalTimeTable {
		struct std::tm arrival_time_tm;
		struct std::tm departure_time_tm;
	} StopArrivalTimeTable;

public:
	Stops();
	virtual ~Stops();

	bool parseInput(const std::string toParse);

	const std::string& getLocationType() const { return location_type; }
	const std::string& getParentStation() const { return parent_station; }
	const std::string& getStopId() const { return stop_id; }
	const std::string& getStopLat() const { return stop_lat; }
	const std::string& getStopLon() const { return stop_lon; }
	const std::string& getStopName() const { return stop_name; }

	unsigned long int getStopIdNum() const {		return stop_id_num;	}
	double getStopLatNum() const {		return stop_lat_num;	}
	double getStopLonNum() const {		return stop_lon_num;	}

private:
	std::string stop_id;
	std::string stop_name;
	std::string stop_lat;
	std::string stop_lon;
	std::string location_type;
	std::string parent_station;

	unsigned long int stop_id_num;
	double stop_lat_num;
	double stop_lon_num;

public:
	std::list<StopArrivalTimeTable> stopTimeTable;
};

#endif /* STOPS_H_ */
