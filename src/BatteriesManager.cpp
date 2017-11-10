/*
 * BatteriesManager.cpp
 *
 *  Created on: 27 ott 2017
 *      Author: angelo
 */

#include "BatteriesManager.h"

BatteriesManager::BatteriesManager() {
	// TODO Auto-generated constructor stub

}

BatteriesManager::~BatteriesManager() {
	// TODO Auto-generated destructor stub
}

void BatteriesManager::initTime(struct std::tm start_time_tm) {
	lastUpdate = start_time_tm;
}

void BatteriesManager::init(unsigned int chargNum, unsigned int chargBattNum, double chargBattVal, double chargBattMaxVal, double chargerPow) {
	// init the chargers
	for (unsigned int i = 0; i < chargNum; ++i) {
		Charger *nc = new Charger(Charger::CHARGER_HOME);
		nc->setChargingPower(chargerPow);
		chargerList.push_back(nc);
	}

	// init the batteries
	for (unsigned int i = 0; i < chargBattNum; ++i) {
		Battery *bat = new Battery();
		bat->setMaxEnergy(chargBattMaxVal);
		bat->setResudualEnergy(chargBattVal);

		batteryList.push_back(bat);
	}
}

void BatteriesManager::printBatteriesState(void) {
	for (auto& b : batteryList) {
		fprintf(stdout, "|%.01lf", b->getResudualEnergy());
	}
}

void BatteriesManager::update(struct std::tm now_time_tm) {

	for (auto& b : batteryList) {
		if (b->getState() == Battery::BATTERY_SELFDISCHARGING) {
			//TODO
		}
	}

	for (auto& ch : chargerList) {
		if (ch->isFree()) {
			// why is free? charge battery! Looking for the lowest one...
			Battery *bLow = nullptr;
			double lowestResidual = std::numeric_limits<double>::max();
			for (auto& bb : batteryList) {
				if ((bb->getState() == Battery::BATTERY_SELFDISCHARGING) && (!bb->isFull())) {
					//ch->setBatteryInCharge(bb, now_time_tm);
					//break;
					if (lowestResidual > bb->getResudualEnergy()) {
						lowestResidual = bb->getResudualEnergy();
						bLow = bb;
					}
				}
			}
			if (bLow != nullptr) {
				ch->setBatteryInCharge(bLow, now_time_tm);
			}
		}
		if (!(ch->isFree())) {
			ch->rechargeTheBattery(now_time_tm);
			if (ch->isBatteryFull()) {
				Battery *b = ch->removeBatteryInCharge();
				b->setState(Battery::BATTERY_SELFDISCHARGING);
			}
		}
	}

	lastUpdate = now_time_tm;
}

