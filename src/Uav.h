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

#include "Home.h"
#include "Battery.h"
//#include "FlowGraph.h"


class Simulator;
class NodeGraph;
class ArcGraph;

class Uav {
	static int idCounter;

public:
	typedef enum {
		UAV_WAIT_HOME,
		UAV_WAIT_DP,
		UAV_WAIT_STOP,
		UAV_FLYING
	} UAV_STATE;

public:
	Uav(Simulator *sim);
	virtual ~Uav();

	double addEnergy(double watt, double seconds);

	//unsigned long int getPositionId() const {		return position_id;	}
	//void setPositionId(unsigned long int positionId) {		position_id = positionId;	}
	NodeGraph *getPositionNode() const {		return position_node;	}
	void setPositionNode(NodeGraph *positionNode) {		position_node = positionNode;	}
	ArcGraph* getActualArch()  {		return actual_arch;	}
	void setActualArch(ArcGraph* actualArch) {		actual_arch = actualArch;	}
	double getResudualEnergy();
	void setResudualEnergy(double resudualEnergy);
	UAV_STATE getState() const {	return state;	}
	void setState(UAV_STATE state) {		this->state = state;	}
	double getAverageSpeed() const {		return averageSpeed;	}
	void setAverageSpeed(double averageSpeed) {		this->averageSpeed = averageSpeed;	}
	unsigned int getDeliveredPackage() const {		return deliveredPackage;	}
	void setDeliveredPackage(unsigned int deliveredPackage) {		this->deliveredPackage = deliveredPackage;	}
	void addDeliveredPackage(unsigned int deliveredPackageNumber){		this->deliveredPackage += deliveredPackageNumber;	}
	int getId() const {		return id;	}
	Home* getBelongingHome()  {		return belongingHome;	}
	void setBelongingHome(Home* belongingHome) {		this->belongingHome = belongingHome;	}
	Battery* getBatt() {		return batt;	}
	void setBatt(Battery* batt) {		this->batt = batt;	}
	Battery* removeBatt(void) { Battery* ris = this->batt; this->batt = nullptr; return ris; }
	double getPosLon() const {		return pos_lon;	}
	void setPosLon(double posLon) {		pos_lon = posLon;	}
	double getPosLat() const {		return pos_lat;	}
	void setPosLat(double posLat) {		pos_lat = posLat;	}
	Package* getCarryingPackage() {		return carryingPackage;	}
	void setCarryingPackage(Package* carryingPackage) {		this->carryingPackage = carryingPackage;	}
	Package* removeCarryingPackage(void) { Package* ris = this->carryingPackage; this->carryingPackage = nullptr; return ris; }
	bool isCarryingPackage(void) { return this->carryingPackage != nullptr; }

	unsigned int getTimeInStop() const {		return timeInStop;	}
	void addTimeInStop(unsigned int timeInStop_val) {		this->timeInStop += timeInStop_val;	}
	unsigned int getTimeOnBus() const {		return timeOnBus;	}
	void addTimeOnBus(unsigned int timeOnBus_val) {		this->timeOnBus += timeOnBus_val;	}

	bool hasFullBattery(void) { return batt->isFull(); }

public:
	// This function converts decimal degrees to radians
	static double deg2rad(double deg) { return (deg * M_PI / 180.0); }

	//  This function converts radians to decimal degrees
	static double rad2deg(double rad) { return (rad * 180.0 / M_PI); }

	/**
	 * Returns the distance between two points on the Earth.
	 * Direct translation from http://en.wikipedia.org/wiki/Haversine_formula
	 * @param lat1d Latitude of the first point in degrees
	 * @param lon1d Longitude of the first point in degrees
	 * @param lat2d Latitude of the second point in degrees
	 * @param lon2d Longitude of the second point in degrees
	 * @return The distance between the two points in kilometers
	 */
	static double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
		double lat1r, lon1r, lat2r, lon2r, u, v;
		lat1r = deg2rad(lat1d);
		lon1r = deg2rad(lon1d);
		lat2r = deg2rad(lat2d);
		lon2r = deg2rad(lon2d);
		u = sin((lat2r - lat1r)/2);
		v = sin((lon2r - lon1r)/2);
		return 2.0 * 6371.0 * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v)) * 1000.0;
	}

	static double getSpeedWithLoad(double speedNoLoad, double loadW) {
		double maxW = 4000.0;
		double packagePenality = 0.6;
		double speedUAV;

		if (loadW <= 0) {
			speedUAV = speedNoLoad;
		}
		else {
			if (loadW >= maxW) {
				speedUAV = speedNoLoad * packagePenality;
			}
			else {
				speedUAV = (speedNoLoad - ((speedNoLoad * loadW * (1.0 - packagePenality)) / maxW));
			}
		}
		return speedUAV;
	}

	static int getFlyTime_sec(double s_lat, double s_lon, double a_lat, double a_lon, double baseSpeed, double w) {
		double dist = distanceEarth(s_lat, s_lon, a_lat, a_lon);
		double speedUAV = getSpeedWithLoad(baseSpeed, w);

		return ((int) dist / speedUAV);
	}

private:
	//double resudualEnergy;
	//unsigned long int position_stop_id;
	//unsigned long int position_id;
	//NodeGraph::NODE_TYPE position_type;
	NodeGraph *position_node;
	ArcGraph *actual_arch;
	int id;
	double averageSpeed;
	unsigned int deliveredPackage;

	unsigned int timeInStop;
	unsigned int timeOnBus;

	double pos_lon;
	double pos_lat;


	UAV_STATE state;
	Simulator *simulator;
	Battery *batt;
	Home *belongingHome;
	Package *carryingPackage;
};


#endif /* UAV_H_ */
