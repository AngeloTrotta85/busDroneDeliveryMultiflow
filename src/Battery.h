/*
 * Battery.h
 *
 *  Created on: 27 ott 2017
 *      Author: angelo
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm
#include <list>        // struct std::tm

using namespace std;

class Battery {
public:
	typedef enum {
		BATTERY_CHARGING_HOME,
		BATTERY_CHARGING_BUS,
		BATTERY_SELFDISCHARGING,
		BATTERY_DISCHARGING_ONUAV
	} BATTERY_STATE;

public:
	Battery();
	virtual ~Battery();

	double addEnergy(double difference, double seconds);
	bool isFull(void) {return (resudualEnergy == maxEnergy);};

	double getMaxEnergy() const {		return maxEnergy;	}
	void setMaxEnergy(double maxEnergy) {		this->maxEnergy = maxEnergy;	}
	double getResudualEnergy() const {		return resudualEnergy;	}
	void setResudualEnergy(double resudualEnergy) {		this->resudualEnergy = resudualEnergy;	}
	BATTERY_STATE getState() const {		return state;	}
	void setState(BATTERY_STATE state) {		this->state = state;	}

private:
	double resudualEnergy;
	double maxEnergy;
	BATTERY_STATE state;
};

#endif /* BATTERY_H_ */
