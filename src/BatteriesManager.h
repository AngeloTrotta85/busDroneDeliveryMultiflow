/*
 * BatteriesManager.h
 *
 *  Created on: 27 ott 2017
 *      Author: angelo
 */

#ifndef BATTERIESMANAGER_H_
#define BATTERIESMANAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm
#include <limits>       // std::numeric_limits
#include <list>        // struct std::tm

#include "Battery.h"
#include "Charger.h"


using namespace std;

class BatteriesManager {
public:
	BatteriesManager();
	virtual ~BatteriesManager();

	void initTime(struct std::tm start_time_tm);
	void init(unsigned int chargNum, unsigned int chargBattNum, double chargBattVal, double chargBattMaxVal, double chargerPow);
	void update(struct std::tm now_time_tm);

	void printBatteriesState(void);

private:
	std::list<Battery *> batteryList;
	std::list<Charger *> chargerList;

	struct std::tm lastUpdate;
};

#endif /* BATTERIESMANAGER_H_ */
