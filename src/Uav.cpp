/*
 * Uav.cpp
 *
 *  Created on: 17 lug 2017
 *      Author: angelo
 */

#include "Uav.h"

int Uav::idCounter = 0;

Uav::Uav(Simulator *sim) {
	simulator = sim;
	state = UAV_FLYING;

	resudualEnergy = 0;
	position_id = std::numeric_limits<unsigned long int>::max();

	timeInStop = timeOnBus = 0;

	belongingHome = nullptr;
	batt = nullptr;
	averageSpeed = 10;

	pos_lon = 0;
	pos_lat = 0;

	id = idCounter++;
}

Uav::~Uav() {
	// TODO Auto-generated destructor stub
}

double Uav::addEnergy(double difference) {

	resudualEnergy += difference;

	return resudualEnergy;
}


