/*
 * Simulator.h
 *
 *  Created on: 15 lug 2017
 *      Author: angelo
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <algorithm>    // std::find
#include <map>
#include <list>
#include <limits>       // std::numeric_limits
#include <ctime>        // struct std::tm
#include <time.h>

#include "Stops.h"
#include "Trips.h"
#include "CalendarDate.h"
#include "StopTimes.h"
#include "Uav.h"
#include "BusRoute.h"
#include "FlowGraph.h"
//#include "TspGraph.h"
//#include "BestPoiGraph.h"
//#include "WorstUav.h"

#include "BatteriesManager.h"
#include "Home.h"
#include "DeliveryPoint.h"


#define AGENCY_FILE 					"agency.txt"
#define AGENCY_FILE_SIZE 				2.0
#define CALENDAR_DATES_FILE  			"calendar_dates.txt"
#define CALENDAR_DATES_FILE_SIZE		108926.0
#define CALENDAR_DATES_FILE_SMALL_SIZE	367.0
#define ROUTES_FILE  					"routes.txt"
#define ROUTES_FILE_SIZE 				243.0
#define SHAPES_FILE  					"shapes.txt"
#define SHAPES_FILE_SIZE  				528750.0
#define STOPS_FILE  					"stops.txt"
#define STOPS_FILE_SIZE  				6567.0
#define STOPS_FILE_SMALL_SIZE			213.0
#define STOP_TIMES_FILE  				"stop_times.txt"
#define STOP_TIMES_FILE_SIZE  			1648069.0
#define STOP_TIMES_FILE_SMALL_SIZE		46375.0
#define TRIPS_FILE  					"trips.txt"
#define TRIPS_FILE_SIZE  				53534
#define TRIPS_FILE_SMALL_SIZE			8253
#define FERMATE_FILE  					"fermate.csv"
#define FERMATE_FILE_SIZE  				9877.0
#define POI_FILE  						"poi.txt"
#define POI_FILE_SIZE  					11.0

#define HOMES_FILE  					"homes.txt"
#define DELIVERYPOINTS_FILE  			"deliveryPoints.txt"

#define OUTPUT_STATS_FILE  				"outStats.txt"

using namespace std;

class InputParser{
public:
	InputParser (int &argc, char **argv){
		for (int i=1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}
	const std::string& getCmdOption(const std::string &option) const{
		std::vector<std::string>::const_iterator itr;
		itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()){
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}
	bool cmdOptionExists(const std::string &option) const{
		return std::find(this->tokens.begin(), this->tokens.end(), option)
		!= this->tokens.end();
	}
private:
	std::vector <std::string> tokens;
};

class Simulator {
public:
	typedef struct Boundary {
		double minLAT;
		double maxLAT;
		double minLON;
		double maxLON;
	} Boundary;
public:
	Simulator();
	virtual ~Simulator();

	void run(void);
	bool init(void);
	//bool init(std::string tpsFileName);
	void stats(std::string outFileName);

	bool importStops(std::string stopsFileName);
	bool importTrips(std::string stopsFileName);
	bool importCalendarDates(std::string stopsFileName);
	bool importStopTimes(std::string stopsFileName);
	bool importHomes(std::string homesFileName);
	bool importDeliveryPoints(std::string deliverypointsFileName);

	bool exportStops(std::string stopsFileName);
	bool exportTrips(std::string tripsFileName);
	bool exportCalendarDates(std::string calendarDAtesFileName);
	bool exportStopTimes(std::string stopTimesFileName);
	bool exportHomes(std::string homesFileName);
	bool exportDeliveryPoints(std::string deliverypointsFileName);

	bool exportDotResult(std::string dotFileName);

	bool generateBusRoute(void);

	void importSomeParameterFromInputLine(InputParser *inputVal);

	unsigned int getTime() const { return time; }
	unsigned int getMaxTime() const { return maxTime; }
	void setMaxTime(unsigned int maxTime) { this->maxTime = maxTime; }
	double getStopsMaxLat() const {		return stopsMaxLat;	}
	void setStopsMaxLat(double stopsMaxLat) {		this->stopsMaxLat = stopsMaxLat;	}
	double getStopsMaxLon() const {		return stopsMaxLon;	}
	void setStopsMaxLon(double stopsMaxLon) {		this->stopsMaxLon = stopsMaxLon;	}
	double getStopsMinLat() const {		return stopsMinLat;	}
	void setStopsMinLat(double stopsMinLat) {		this->stopsMinLat = stopsMinLat;	}
	double getStopsMinLon() const {		return stopsMinLon;	}
	void setStopsMinLon(double stopsMinLon) {		this->stopsMinLon = stopsMinLon;	}
	const std::string& getOperationalDay() const {		return operationalDay;	}
	void setOperationalDay(const std::string& operationalDay) {		this->operationalDay = operationalDay;	}
	unsigned int getUav() const {		return nUAV;	}
	void setUav(unsigned int uav) {		nUAV = uav;	}
	unsigned int getTimeslot() const {		return timeslot;	}
	void setTimeslot(unsigned int timeslot) {		this->timeslot = timeslot;	}
	bool isToCluster() const {		return toCluster;	}

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

	static bool compare_Route (const BusRoute::Route_Node& first, const BusRoute::Route_Node& second) {
		return ( first.stop_sequence < second.stop_sequence );
	}

private:
	void updateBatteries(Uav *u);

	void generateGraph(struct std::tm start_time, struct std::tm end_time);

	bool isInsideBoudaries(double lat, double lon);

	void generateBothFlyArc(struct std::tm s_time, NodeGraph::NODE_TYPE s_type, unsigned int s_id, double s_lat, double s_lon,
			NodeGraph::NODE_TYPE a_type, unsigned int a_id, double a_lat, double a_lon, double w);

private:
	unsigned int time;
	unsigned int timeslot;	//seconds
	unsigned int maxTime;

	unsigned int nUAV;

	double initialUavEnergy; //Joule
	double maxUavEnergy; 	// Joule
	double uavAvgSpeed;		 	// m/s

	double maxDistancePoiStop; // meters

	unsigned int finalLifetime;

	bool toCluster;

	bool setRandomPoint;
	int numberOfRandomPoint;

	// Watt
	double eSTOP;
	double eRELAY;
	double eRECORDING;
	double eRECORDINGRELAY;
	double eRECHARGING;

	double stopsMinLat;
	double stopsMaxLat;
	double stopsMinLon;
	double stopsMaxLon;
	std::list<Boundary> boundaryList;

	std::string operationalDay;
	struct std::tm operationalDay_tm;

	struct std::tm start_sim_time_tm;
	struct std::tm end_sim_time_tm;

	std::map<unsigned long int, Stops> stopsMap;
	std::map<std::string, Trips> tripsMap;
	std::map<std::string, CalendarDate> calendarDateMap;
	std::map<std::string, StopTimes> stopTimesMap;
	std::map<std::string, std::map<unsigned long int, StopTimes> > stopTimesMapMap;
	//std::map<unsigned long int, Poi> poiMap;
	std::map<std::string, BusRoute> busRouteMap;

public:
	std::map<unsigned int, Home> homesMap;
	std::map<unsigned int, DeliveryPoint> deliveryPointsMap;

	std::list<Uav *> listUav;

	FlowGraph *flowGraph;
};


#endif /* SIMULATOR_H_ */
