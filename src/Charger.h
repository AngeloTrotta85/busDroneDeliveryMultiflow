/*
 * Charger.h
 *
 *  Created on: 29 ott 2017
 *      Author: angelo
 */

#ifndef CHARGER_H_
#define CHARGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm
#include <list>        // struct std::tm

#include "Battery.h"

using namespace std;

class Charger {
public:
	typedef enum {
		CHARGER_HOME,
		CHARGER_BUS
	} CHARGER_TYPE;

public:
	Charger(CHARGER_TYPE chargetT);
	virtual ~Charger();

	bool isFree(void) {return (batterySlot == nullptr);};
	void rechargeTheBattery(struct std::tm time_tm);
	bool setBatteryInCharge(Battery *batt, struct std::tm time_tm);
	Battery *removeBatteryInCharge(void);
	bool isBatteryFull(void) {return ((batterySlot != nullptr) && (batterySlot->isFull()));};

	Battery* getBatterySlot() {		return batterySlot;	}
	double getChargingPower() const {		return chargingPower;	}
	void setChargingPower(double chargingPower) {		this->chargingPower = chargingPower;	}

private:
	Battery *batterySlot;
	CHARGER_TYPE ct;

	struct std::tm lastChargeTime;

	double chargingPower;  //Watt
};

#endif /* CHARGER_H_ */
