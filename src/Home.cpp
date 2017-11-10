/*
 * Home.cpp
 *
 *  Created on: 16 ott 2017
 *      Author: angelo
 */

#include "Home.h"
#include "Simulator.h"

Home::Home() {

}

Home::~Home() {
}

bool Home::parseInput(const std::string toParse) {
	bool ris = true;
	int idx = 0;
	std::string tmp;
	typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

	tokenizer tok{toParse};
	for (const auto &t : tok){
		//std::cout << "|" << t << "|" << '\n';

		switch (idx) {
		case HOME_ID:
			home_id = t;
			home_id_num = atol(home_id.c_str());
			break;
		case HOME_NAME:
			home_name = t;
			break;
		case HOME_LAT:
			home_lat = t;
			home_lat_num = atof(home_lat.c_str());
			break;
		case HOME_LON:
			home_lon = t;
			home_lon_num = atof(home_lon.c_str());
			break;
		case HOME_WA_DEFWEIGHT:
			tmp = t;
			home_wa_defpkt_w = atof(tmp.c_str());
			break;
		case HOME_WA_PKTINITNUM:
			tmp = t;
			home_wa_pkt_initnum = atol(tmp.c_str());
			break;
		case HOME_WA_PKTGENRATE:
			tmp = t;
			home_wa_pkt_genrate = atof(tmp.c_str());
			break;
		case HOME_CHARG_NUM:
			tmp = t;
			home_charg_num = atol(tmp.c_str());
			break;
		case HOME_CHARG_BATTINITNUM:
			tmp = t;
			home_charg_batt_initnum = atol(tmp.c_str());
			break;
		case HOME_CHARG_BATTINITVAL:
			tmp = t;
			home_charg_batt_initval = atof(tmp.c_str());
			break;
		default:
			cerr << "Too much columns in Home file" << endl;
			ris = false;
			break;
		}

		idx++;
	}

	if (idx != 10) {
		ris = false;
	}

	return ris;
}

void Home::init(struct std::tm sim_time_tm) {

	// warehouse initialization
	wa.initTime(sim_time_tm);

	// battery manager initialization
	bm.initTime(sim_time_tm);

}

void Home::update(struct std::tm now_time_tm) {
	// warehouse update
	wa.update(now_time_tm, simulator->deliveryPointsMap);

	// battery manager update
	bm.update(now_time_tm);
}

unsigned int Home::getWA_pktNumber(void) {
	return wa.getWarehousePktNumber();
}

/*void Home::setWA_parameters(double defW, int initPck, double genRate, std::map<unsigned int, DeliveryPoint> &dp) {
	wa.setDefaultWeight_grams(defW);
	wa.setPacketInitNumber(initPck, dp);
	wa.setPacketGenerationRate(genRate);
}*/

void Home::initWA(std::map<unsigned int, DeliveryPoint> &dp) {
	wa.setPacketInitNumber(home_wa_pkt_initnum, dp);
}

void Home::initBM(double battMaxVal, double chargerPow) {

	bm.init(home_charg_num, home_charg_batt_initnum, home_charg_batt_initval, battMaxVal, chargerPow);

}





