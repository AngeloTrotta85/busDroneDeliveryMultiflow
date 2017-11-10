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
#include "Poi.h"
#include "Stops.h"
#include "Uav.h"
//#include "Simulator.h"

class NodeGraph;
class Simulator;

class ArcGraph {
public:
	typedef enum {
		STOP,
		COVER,
		BUS
	} ARC_TYPE;
public:
	ArcGraph(){
		src = nullptr;
		dest = nullptr;
		p = nullptr;
		arc_t = STOP;
		reserved = false;
	};
	virtual ~ArcGraph(){ };

public:
	NodeGraph *src;
	NodeGraph *dest;
	ARC_TYPE arc_t;
	std::list<Uav *> uavOnTheArc;
	Poi *p;
	bool reserved;
};

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
		distenace_from_root = std::numeric_limits<int>::max();
		predecessor_arc = nullptr;
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

	//BFS variables
	BFS_STATE bfs_state;
	int distenace_from_root;
	ArcGraph *predecessor_arc;
};

class FlowGraph {
public:
	FlowGraph(Simulator *s);
	virtual ~FlowGraph();

	virtual void initPoi(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName) {};
	void execute(struct std::tm time, std::list<Uav *> &uavList);

	void addInitStop(unsigned int stop, struct std::tm time);
	void addInitHome(unsigned int home, struct std::tm time);
	void addInitDeliveryPoint(unsigned int dp, struct std::tm time);
	void addFollowingStop(unsigned int stop, struct std::tm time);
	void addFollowingHome(unsigned int home, struct std::tm time);
	void addFollowingDeliveryPoint(unsigned int dp, struct std::tm time);
	void generateStaticArcs(unsigned int stop, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE, Poi *p = nullptr);
	void generateStaticArcsFromRoute(BusRoute *br, struct std::tm timeBegin, struct std::tm timeEnd);

	virtual void setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap);
	void setUavPosition(struct std::tm time, Uav *uav);
	void updateUavOnFlow(unsigned int time);
	virtual void activateUavFlow(unsigned int time, std::list<Uav *> &uavList);
	void updateBatteries(Uav *u, unsigned int t);

	bool exportDotResult(std::string dotFileName);

	void getMinimumPathAll(std::map<unsigned int, std::list<ArcGraph *> > &arcMapList, unsigned int stopStart, unsigned int timeStart);
	void getMinimumPathFromAll(std::list<ArcGraph *> &arcList, unsigned int stopStart, unsigned int timeStart, unsigned int stopEnd);
	void getMinimumPathToFew(std::map<unsigned int, std::list<ArcGraph *> > &arcMapList, std::map<unsigned int, unsigned int > &arcMapListCost, unsigned int stopStart, unsigned int timeStart, std::vector<Stops *> &stopsEnd);

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

protected:
	std::map<unsigned int, std::map<unsigned int, NodeGraph *> > graphMapMap;
	std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::map<unsigned int, NodeGraph *> > > graphMapMapMap;
	std::map<unsigned int, std::vector<NodeGraph *> > graphMapVec;
	std::list<ArcGraph *> activeArc;

	Simulator *sim;

	unsigned int swapCount;
};


#endif /* FLOWGRAPH_H_ */