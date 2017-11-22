/*
 * Battery.cpp
 *
 *  Created on: 27 ott 2017
 *      Author: angelo
 */

#include "Battery.h"

int Battery::id_batt_counter = 0;

Battery::Battery() {
	resudualEnergy = 130000;
	maxEnergy = 130000;
	state = BATTERY_SELFDISCHARGING;

	id_batt = id_batt_counter++;
}

Battery::~Battery() {
	// TODO Auto-generated destructor stub
}

double Battery::addEnergy(double watt, double seconds) {

	//cout << " Adding/Removing energy: " << (difference * seconds) << endl;
	resudualEnergy += watt * seconds;

	if (resudualEnergy < 0) {
		resudualEnergy = 0;
	} else if (resudualEnergy > maxEnergy) {
		resudualEnergy = maxEnergy;
	}

	return resudualEnergy;
}
