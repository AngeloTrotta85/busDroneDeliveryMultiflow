/*
 * Warehouse.cpp
 *
 *  Created on: 18 ott 2017
 *      Author: angelo
 */

#include "Warehouse.h"

Warehouse::~Warehouse() {
	// TODO Auto-generated destructor stub
}

Warehouse::Warehouse() {
	defaultWeight = 1000;
	packetGenerationRate = 1.0/60.0;
}

void Warehouse::setPacketInitNumber(double packetNumber, std::map<unsigned int, DeliveryPoint> &dp) {
	for (int i = 0; i < packetNumber; ++i) {
		Package *newPck = new Package(defaultWeight, 10, 10, 10);

		//set random destination
		if (dp.size() == 0) {cerr << "Warning, delivery points list is empty" << endl; exit(EXIT_FAILURE);}
		int randDP = rand() % dp.size();
		auto dpIte = dp.begin();
		while (randDP > 0) {
			++dpIte;
			--randDP;
		}
		newPck->dest_dp = &(dpIte->second);

		wareHouse.push_back(newPck);
	}
}

void Warehouse::initTime(struct std::tm start_time_tm) {
	lastPacketGeneration = start_time_tm;
}

void Warehouse::update(struct std::tm now_time_tm, std::map<unsigned int, DeliveryPoint> &dp) {
	int pkt2gen = 0;

	double time_elapsed = difftime(mktime(&now_time_tm), mktime(&lastPacketGeneration));
	//cout << "time elapsed: " << time_elapsed << endl;
	pkt2gen = packetGenerationRate * time_elapsed;

	while (pkt2gen > 0) {
		Package *newPck = new Package(defaultWeight, 10, 10, 10);

		//set random destination
		if (dp.size() == 0) {cerr << "Warning, delivery points list is empty" << endl;}
		int randDP = rand() % dp.size();
		auto dpIte = dp.begin();
		while (randDP > 0) {
			++dpIte;
			--randDP;
		}
		newPck->dest_dp = &(dpIte->second);

		wareHouse.push_back(newPck);

		lastPacketGeneration = now_time_tm;

		--pkt2gen;
	}
}


