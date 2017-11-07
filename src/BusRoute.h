/*
 * BusRoute.h
 *
 *  Created on: 18 lug 2017
 *      Author: angelo
 */

#ifndef BUSROUTE_H_
#define BUSROUTE_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <boost/tokenizer.hpp>
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm
#include <list>        // struct std::tm


class BusRoute {
public:
	typedef struct Route_Node {
		unsigned long int stop_id;
		unsigned long int stop_sequence;
		std::tm arrival_time;
		std::tm departure_time;
		std::string busName;
	} Route_Node;

public:
	BusRoute();
	virtual ~BusRoute();

public:
	std::list<Route_Node> orderedList_Stops;
	std::string route_id;
	std::string bus_id;
};


#endif /* BUSROUTE_H_ */
