/*
 * Uav.cpp
 *
 *  Created on: 17 lug 2017
 *      Author: angelo
 */

#include "Uav.h"

int Uav::idCounter = 0;

Uav::Uav() {
	state = UAV_FLYING;

	resudualEnergy = 0;
	covering_poi_id = position_stop_id = std::numeric_limits<unsigned long int>::max();

	timeInStop = timeOnBus = timeInPoiRec = timeInPoiRel = timeInPoiRecRel = 0;

	id = idCounter++;
}

Uav::~Uav() {
	// TODO Auto-generated destructor stub
}

double Uav::addEnergy(double difference) {

	resudualEnergy += difference;

	return resudualEnergy;
}


