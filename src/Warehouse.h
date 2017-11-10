/*
 * Warehouse.h
 *
 *  Created on: 18 ott 2017
 *      Author: angelo
 */

#ifndef WAREHOUSE_H_
#define WAREHOUSE_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <algorithm>    // std::find
#include <map>
#include <list>
#include <limits>       // std::numeric_limits
#include <ctime>        // struct std::tm
#include <time.h>

#include "Package.h"
#include "DeliveryPoint.h"

using namespace std;

class Warehouse {
public:
	Warehouse();
	virtual ~Warehouse();

	void initTime(struct std::tm start_time_tm);
	void update(struct std::tm now_time_tm, std::map<unsigned int, DeliveryPoint> &dp);

	void setDefaultWeight_grams(double defaultWeight) { this->defaultWeight = defaultWeight;	}
	void setPacketInitNumber(double packetNumber, std::map<unsigned int, DeliveryPoint> &dp);
	void setPacketGenerationRate(double packetGenerationRate) {		this->packetGenerationRate = packetGenerationRate;	}

	unsigned int getWarehousePktNumber(void) { return wareHouse.size(); }

public:
	std::list<Package *> wareHouse;

private:

	struct std::tm lastPacketGeneration;

	double defaultWeight;
	double packetGenerationRate;
};

#endif /* WAREHOUSE_H_ */
