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

	//position_id = std::numeric_limits<unsigned long int>::max();
	position_node = nullptr;
	actual_arch = nullptr;

	timeInStop = timeOnBus = 0;

	belongingHome = nullptr;
	batt = nullptr;
	carryingPackage = nullptr;
	averageSpeed = 10;
	deliveredPackage = 0;

	pos_lon = 0;
	pos_lat = 0;

	id = idCounter++;
}

Uav::~Uav() {
	// TODO Auto-generated destructor stub
}

double Uav::getResudualEnergy()  {
	if (batt != nullptr) {
		return batt->getResudualEnergy();
	}
	else {
		return -1;
	}
}

void Uav::setResudualEnergy(double resudualEnergy) {
	if (batt != nullptr) {
		batt->setResudualEnergy(resudualEnergy);
	}

}

double Uav::addEnergy(double watt, double seconds) {

	return batt->addEnergy(watt, seconds);
}


