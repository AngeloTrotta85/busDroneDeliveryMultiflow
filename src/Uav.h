/*
 * Uav.h
 *
 *  Created on: 17 lug 2017
 *      Author: angelo
 */

#ifndef UAV_H_
#define UAV_H_

#include <limits.h>

#include "BusRoute.h"

class Uav {
	static int idCounter;

public:
	typedef enum {
		UAV_FLYING,
		UAV_ONBUS
	} UAV_STATE;

public:
	Uav();
	virtual ~Uav();

	double addEnergy(double difference);

	bool isCovering(void) {return (covering_poi_id != std::numeric_limits<unsigned long int>::max());};
	void unsetCovering(void) {covering_poi_id = std::numeric_limits<unsigned long int>::max();};

	unsigned long int getCoveringPoiId() const {		return covering_poi_id;	}
	void setCoveringPoiId(unsigned long int coveringPoiId) {		covering_poi_id = coveringPoiId;	}
	unsigned long int getPositionStopId() const {		return position_stop_id;	}
	void setPositionStopId(unsigned long int positionStopId) {		position_stop_id = positionStopId;	}
	double getResudualEnergy() const {		return resudualEnergy;	}
	void setResudualEnergy(double resudualEnergy) {		this->resudualEnergy = resudualEnergy;	}
	UAV_STATE getState() const {	return state;	}
	void setState(UAV_STATE state) {		this->state = state;	}
	int getId() const {		return id;	}

	unsigned int getTimeInPoiRec() const {		return timeInPoiRec;	}
	void addTimeInPoiRec(unsigned int timeInPoiRec_val) {		this->timeInPoiRec += timeInPoiRec_val;	}
	unsigned int getTimeInPoiRecRel() const {		return timeInPoiRecRel;	}
	void addTimeInPoiRecRel(unsigned int timeInPoiRecRel_val) {		this->timeInPoiRecRel += timeInPoiRecRel_val;	}
	unsigned int getTimeInPoiRel() const {		return timeInPoiRel;	}
	void addTimeInPoiRel(unsigned int timeInPoiRel_val) {		this->timeInPoiRel += timeInPoiRel_val;	}
	unsigned int getTimeInStop() const {		return timeInStop;	}
	void addTimeInStop(unsigned int timeInStop_val) {		this->timeInStop += timeInStop_val;	}
	unsigned int getTimeOnBus() const {		return timeOnBus;	}
	void addTimeOnBus(unsigned int timeOnBus_val) {		this->timeOnBus += timeOnBus_val;	}

private:
	double resudualEnergy;
	unsigned long int covering_poi_id;
	unsigned long int position_stop_id;
	int id;

	unsigned int timeInStop;
	unsigned int timeOnBus;
	unsigned int timeInPoiRec;
	unsigned int timeInPoiRel;
	unsigned int timeInPoiRecRel;


	UAV_STATE state;
};


#endif /* UAV_H_ */
