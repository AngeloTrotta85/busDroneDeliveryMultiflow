/*
 * FlowGraph.h
 *
 *  Created on: 19 lug 2017
 *      Author: angelo
 */

#ifndef FLOWGRAPH_H_
#define FLOWGRAPH_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>
#include <boost/tokenizer.hpp>
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <time.h>        // struct std::tm
#include <map>

#include "BusRoute.h"
#include "Stops.h"
#include "Uav.h"
//#include "Simulator.h"

//class NodeGraph;
class Simulator;
class ArcGraph;


class NodeGraph {
public:
	typedef enum {
		VISITED,
		NOT_VISITED,
		DISCOVERED
	} BFS_STATE;

	typedef enum {
		STOP,
		HOME,
		DELIVERY_POINT
	} NODE_TYPE;
/*public:
	typedef enum {
		STOP,
		COVER,
		BUS
	} ARC_TYPE;
	typedef struct ARC {
		NodeGraph *dest;
		Poi *p;
		Uav *uav;
		ARC_TYPE arc_t;
	} ARC;*/
public:
	NodeGraph(unsigned int t, unsigned int id, NODE_TYPE type){
		time = t;
		//stop_id = s;
		node_id = id;
		node_t = type;
		bfs_state = NOT_VISITED;
		main_path_check = false;
		distenace_from_root = std::numeric_limits<int>::max();
		predecessor_arc = nullptr;
		stop_ptr = nullptr;
		home_ptr = nullptr;
		dp_ptr = nullptr;
	};
	virtual ~NodeGraph(){
		arcs.clear();
		uavs.clear();
	};

public:
	unsigned int time;
	//unsigned int stop_id;
	unsigned int node_id;
	NODE_TYPE node_t;
	std::list<ArcGraph *> arcs;
	std::list<Uav *> uavs;

	//possible pointers
	Stops *stop_ptr;
	Home *home_ptr;
	DeliveryPoint *dp_ptr;

	//BFS variables
	BFS_STATE bfs_state;
	int distenace_from_root;
	ArcGraph *predecessor_arc;
	bool main_path_check;
};

class ArcGraph {
public:
	typedef enum {
		STOP,
		BUS,
		FLY_EMPTY,
		FLY_WITH_PACKAGE
	} ARC_TYPE;
public:
	ArcGraph(){
		src = nullptr;
		dest = nullptr;
		batt = nullptr;
		arc_t = STOP;
		reserved = false;
		energyCost_watt = 0;
	};
	virtual ~ArcGraph(){ };

	/*double getEnergyCost(double w = 0) {
		double ris = 0;
		if ((src != nullptr) && (dest != nullptr)) {
			if (energyCost_watt > 0) {	// recharging arcs (not w dependent)
				ris = energyCost_watt * (dest->time - src->time);
			}
			else if (energyCost_watt < 0){	// flying arcs (w dependent)
				//TODO sincronizzarsi con Simulator::getEnergyLossUav
				double multiplier = 1.0 + (w / 3000.0);
				ris = (energyCost_watt * multiplier) * (dest->time - src->time);
			}
			else {	// Home static arc
				ris = 0;
			}
		}
		return ris;
	}*/

	double getEnergyCost() {
		double ris = 0;
		if ((src != nullptr) && (dest != nullptr)) {
			ris = energyCost_watt * (dest->time - src->time);
		}
		return ris;
	}

public:
	NodeGraph *src;
	NodeGraph *dest;
	ARC_TYPE arc_t;
	std::list<Uav *> uavOnTheArc;
	Battery *batt;
	bool reserved;
	double energyCost_watt;
};

class FlowGraph {
public:
	FlowGraph(Simulator *s);
	virtual ~FlowGraph();

	void execute(struct std::tm time, std::list<Uav *> &uavList);

	void addInitStop(Stops *st_ptr, unsigned int stop, struct std::tm time);
	void addInitHome(Home *h_ptr, unsigned int home, struct std::tm time);
	void addInitDeliveryPoint(DeliveryPoint *dp_ptr, unsigned int dp, struct std::tm time);
	void addFollowingStop(Stops *st_ptr, unsigned int stop, struct std::tm time);
	void addFollowingHome(Home *h_ptr, unsigned int home, struct std::tm time);
	void addFollowingDeliveryPoint(DeliveryPoint *dp_ptr, unsigned int dp, struct std::tm time);
	void generateStaticArcsStop(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE, double w_cost);
	void generateStaticArcsHome(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE, double w_cost);
	void generateStaticArcsDeliveryPoint(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE, double w_cost);
	void generateStaticArcsFromRoute(BusRoute *br, struct std::tm timeBegin, struct std::tm timeEnd, double w_cost);
	void generateFlyArcs(struct std::tm s_time, NodeGraph::NODE_TYPE s_type, unsigned int s_id, struct std::tm a_time, NodeGraph::NODE_TYPE a_type, unsigned int a_id, ArcGraph::ARC_TYPE at, double w_cost);

	NodeGraph *getNodePtr(NodeGraph::NODE_TYPE n_type, unsigned int id, struct std::tm time_tm);

	//virtual void setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap);
	//void setUavPosition(struct std::tm time, Uav *uav);
	void setUavPosition(NodeGraph * ng, Uav *uav);
	void updateUavOnFlow(unsigned int time);
	virtual void generateUavPath(unsigned int time, Uav *u);
	virtual void activateUavFlow(unsigned int time, std::list<Uav *> &uavList);
	virtual void activateUavFlow_old(unsigned int time, std::list<Uav *> &uavList);
	void updateBatteries(Uav *u, unsigned int t);

	bool exportDotResult(std::string dotFileName);
	bool exportDotFullEmptyGraph(std::string dotFileName);

	void getMinimumPathToFew(std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::list<ArcGraph *> > > &arcMapList, std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, unsigned int > > &arcMapListCost, NodeGraph *nodeStart, std::vector<NodeGraph *> &nodesEnd);
	void getMinimumPathToFew_withEnergy(std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::list<ArcGraph *> > > &arcMapList, std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, unsigned int > > &arcMapListCost, std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, double > > &arcMapListEnergyCost, NodeGraph *nodeStart, std::vector<NodeGraph *> &nodesEnd, bool useFlyFreeLinks, bool useFlyPackageLinks);
	void getMinimumPathOnlyFly_GoAndBack(std::list<ArcGraph *> &arcList, unsigned int &arcListTimeCost, double &arcListEnergyCost, Home *homeStart, unsigned int timeStart, DeliveryPoint *dp);

	bool check_pkt_feasibility(double s_lat, double s_lon, Package *p, Battery *b);

	static unsigned int day_tm2seconds(struct std::tm t) {
		return (t.tm_sec + (60 * (t.tm_min + (60.0 * t.tm_hour))));
	}
	static struct std::tm day_seconds2tm(unsigned int t) {
		struct std::tm ris;
		ris.tm_sec = t % 60;
		ris.tm_min = (t / 60) % 60;
		ris.tm_hour = t / 3600;

		return ris;
	}

	unsigned int getSwapCount() const {		return swapCount;	}

public:
	//std::map<unsigned int, std::map<unsigned int, NodeGraph *> > graphMapMap;
	std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::map<unsigned int, NodeGraph *> > > graphMapMapMap;
	//std::map<unsigned int, std::vector<NodeGraph *> > graphMapVec;
	std::list<ArcGraph *> activeArc;

	Simulator *sim;

	unsigned int swapCount;

protected:
	std::map<int, std::list<ArcGraph *> > uavArcMapList;
};


#endif /* FLOWGRAPH_H_ */
