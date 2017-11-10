/*
 * Battery.cpp
 *
 *  Created on: 27 ott 2017
 *      Author: angelo
 */

#include "Battery.h"

Battery::Battery() {
	resudualEnergy = 130000;
	maxEnergy = 130000;
	state = BATTERY_SELFDISCHARGING;
}

Battery::~Battery() {
	// TODO Auto-generated destructor stub
}

double Battery::addEnergy(double difference, double seconds) {

	//cout << " Adding/Removing energy: " << (difference * seconds) << endl;
	resudualEnergy += difference * seconds;

	if (resudualEnergy < 0) {
		resudualEnergy = 0;
	} else if (resudualEnergy > maxEnergy) {
		resudualEnergy = maxEnergy;
	}

	return resudualEnergy;
}
