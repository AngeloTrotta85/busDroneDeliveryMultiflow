/*
 * Trips.h
 *
 *  Created on: 07 lug 2017
 *      Author: angelo
 */

#ifndef TRIPS_H_
#define TRIPS_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

class Trips {
public:
	typedef enum {
		TRIPS_ROUTE_ID = 0,
		TRIPS_SERVICE_ID,
		TRIPS_TRIP_ID,
		TRIPS_DIRECTION_ID,
		TRIPS_SHAPE_ID
	} ENUM_TRIPS;

public:
	Trips();
	virtual ~Trips();

	bool parseInput(const std::string toParse);

	const std::string& getDirectionId() const { return direction_id; }
	const std::string& getRouteId() const { return route_id; }
	const std::string& getServiceId() const { return service_id; }
	const std::string& getShapeId() const { return shape_id; }
	const std::string& getTripId() const { return trip_id; }

	unsigned long int getDirectionIdNum() const {		return direction_id_num;	}
	unsigned long int getRouteIdNum() const {		return route_id_num;	}
	unsigned long int getServiceIdNumPrefix() const {		return service_id_num_prefix;	}
	unsigned long int getServiceIdNumSuffix() const {		return service_id_num_suffix;	}
	unsigned long int getShapeIdNum() const {		return shape_id_num;	}
	unsigned long int getTripIdNumPrefix() const {		return trip_id_num_prefix;	}
	unsigned long int getTripIdNumSuffix() const {		return trip_id_num_suffix;	}

private:
	std::string route_id;
	std::string service_id;
	std::string trip_id;
	std::string direction_id;
	std::string shape_id;

	unsigned long int route_id_num;
	unsigned long int service_id_num_prefix;
	unsigned long int service_id_num_suffix;
	unsigned long int trip_id_num_prefix;
	unsigned long int trip_id_num_suffix;
	unsigned long int direction_id_num;
	unsigned long int shape_id_num;
};

#endif /* TRIPS_H_ */
