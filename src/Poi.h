/*
 * Poi.h
 *
 *  Created on: 17 lug 2017
 *      Author: angelo
 */

#ifndef POI_H_
#define POI_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <boost/tokenizer.hpp>

#include "Uav.h"

using namespace std;
using namespace boost;

class Poi {
public:
	typedef enum {
		POI_POI_ID = 0,
		POI_NAME,
		POI_LAT,
		POI_LON,
		POI_TYPE
	} ENUM_POI;

	typedef enum {
		RECORDING = 0,
		RELAYING,
		RECORDING_RELAYING
	} POI_TYPE_T;

public:
	Poi();
	virtual ~Poi();

	bool parseInput(const std::string toParse);

	void createRandom(unsigned int id, double lat, double lon);

	const std::string& getPoiId() const {		return poi_id;	}
	unsigned long int getPoiIdNum() const {		return poi_id_num;	}
	const std::string& getPoiLat() const {		return poi_lat;	}
	double getPoiLatNum() const {		return poi_lat_num;	}
	const std::string& getPoiLon() const {		return poi_lon;	}
	double getPoiLonNum() const {		return poi_lon_num;	}
	const std::string& getPoiName() const {		return poi_name;	}
	POI_TYPE_T getPoiT() const {		return poi_t;	}
	const std::string& getPoiType() const {		return poi_type;	}
	Uav* getCoveringUav() const {		return coveringUav;	}
	void setCoveringUav(Uav *coveringUav) {		this->coveringUav = coveringUav;	}

	static double fRand(double fMin, double fMax) {
	    double f = (double)rand() / RAND_MAX;
	    return fMin + f * (fMax - fMin);
	}

private:
	std::string poi_id;
	std::string poi_name;
	std::string poi_lat;
	std::string poi_lon;
	std::string poi_type;

	unsigned long int poi_id_num;
	double poi_lat_num;
	double poi_lon_num;
	POI_TYPE_T poi_t;

	Uav *coveringUav;

	static int counterRandom;

};


#endif /* POI_H_ */
