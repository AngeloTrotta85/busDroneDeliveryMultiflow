/*
 * DeliveryPoint.h
 *
 *  Created on: 17 ott 2017
 *      Author: angelo
 */

#ifndef DELIVERYPOINT_H_
#define DELIVERYPOINT_H_

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


class DeliveryPoint {
public:
	typedef enum {
		DP_ID = 0,
		DP_NAME,
		DP_LAT,
		DP_LON
	} ENUM_DELIVERYPOINT;

public:
	DeliveryPoint();
	virtual ~DeliveryPoint();

	bool parseInput(const std::string toParse);

	const std::string& getDpId() const {		return dp_id;	}
	unsigned int getDpIdNum() const {		return dp_id_num;	}
	const std::string& getDpLat() const {		return dp_lat;	}
	double getDpLatNum() const {		return dp_lat_num;	}
	const std::string& getDpLon() const {		return dp_lon;	}
	double getDpLonNum() const {		return dp_lon_num;	}
	const std::string& getDpName() const {		return dp_name;	}

private:
	std::string dp_id;
	std::string dp_name;
	std::string dp_lat;
	std::string dp_lon;

	unsigned int dp_id_num;
	double dp_lat_num;
	double dp_lon_num;
};

#endif /* DELIVERYPOINT_H_ */
