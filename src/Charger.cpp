/*
 * Charger.cpp
 *
 *  Created on: 29 ott 2017
 *      Author: angelo
 */

#include "Charger.h"

Charger::Charger(CHARGER_TYPE chargetT) {
	// TODO Auto-generated constructor stub
	batterySlot = nullptr;
	chargingPower = 0;
	ct = chargetT;
}

Charger::~Charger() {
	// TODO Auto-generated destructor stub
}

void Charger::rechargeTheBattery(struct std::tm time_tm) {
	double diffT = difftime(mktime(&time_tm), mktime(&lastChargeTime));
	//cout << "Charger elapsed time: " << diffT << endl;
	if (batterySlot != nullptr) {
		//cout << "Charger elapsed time: " << diffT << " - Charging power: " << chargingPower << endl;
		batterySlot->addEnergy(chargingPower, diffT);

		/*if ((ct == CHARGER_HOME) && (batterySlot->isFull())) {
			batterySlot->setState(Battery::BATTERY_SELFDISCHARGING);
			batterySlot = nullptr;
		}*/
	}
	else {
		cerr << "Warning: the charger is charging a nullptr battery..." << endl;
	}

	lastChargeTime = time_tm;
}

Battery *Charger::removeBatteryInCharge(void) {
	Battery *ris = batterySlot;
	batterySlot = nullptr;
	return ris;
}

bool Charger::setBatteryInCharge(Battery *batt, struct std::tm time_tm) {
	if (batterySlot == nullptr) {
		batterySlot = batt;
		if (ct == CHARGER_BUS){
			batterySlot->setState(Battery::BATTERY_CHARGING_BUS);
		}
		else {
			batterySlot->setState(Battery::BATTERY_CHARGING_HOME);
		}
		lastChargeTime = time_tm;
		return true;
	}
	else {
		return false;
	}
}

