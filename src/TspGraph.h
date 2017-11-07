/*
 * TspGraph.h
 *
 *  Created on: 24 lug 2017
 *      Author: angelo
 */

#ifndef TSPGRAPH_H_
#define TSPGRAPH_H_

#include "FlowGraph.h"

class TspNodeGraph;

class TspArcGraph {
public:
	TspArcGraph(){
		src = nullptr;
		dest = nullptr;
		weight = std::numeric_limits<unsigned int>::max();
	};
	virtual ~TspArcGraph(){ };

public:
	TspNodeGraph *src;
	TspNodeGraph *dest;
	unsigned int weight;
};

class TspNodeGraph {
public:
	TspNodeGraph(unsigned int p){
		poi_id = p;
		poiPtr = nullptr;
		tspNextHop = nullptr;
		tspPreviousHop = nullptr;
		mstReached = false;
	};
	virtual ~TspNodeGraph(){ };

public:
	unsigned int poi_id;
	Poi *poiPtr;
	std::list<TspArcGraph *> arcs;
	std::vector<Stops *> neighStops;


	TspArcGraph *tspNextHop;
	TspArcGraph *tspPreviousHop;

	std::list<TspArcGraph *> mstChild;
	bool mstReached;
	std::vector<std::list<TspArcGraph *> >kruskal_mstChild;
	std::vector<TspArcGraph *> kruskal_tspNextHop;
	std::vector<TspArcGraph *> kruskal_tspPreviousHop;
	std::vector<int> kruskal_tspID;
};

class TspGraph : public FlowGraph{
public:
	TspGraph(Simulator *s) : FlowGraph(s) {
		mstRootNode = nullptr;
		bestClusteringLayer = -1;
	};
	virtual ~TspGraph() {};

	virtual void initPoi(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName);
	virtual void activateUavFlow(unsigned int time, std::list<Uav *> &uavList);

	virtual void setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap);

	static bool sortArcs (const TspArcGraph *first, const TspArcGraph *second) {
		return first->weight < second->weight;
	}

private:
	void generatePoiGraph(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName);
	void calculateTSP(void);
	unsigned int calculateTSPcost(std::map<unsigned int, TspNodeGraph *> &tsp);
	unsigned int calculateTSPcost_backward(std::map<unsigned int, TspNodeGraph *> &tsp);
	void calculateMST(void);
	void makeKruskalClusters(void);
	double calculateRI(int clusteringLevel);
	void calculateTSP_kruskal(TspNodeGraph *root, unsigned int kruskal_level);
	unsigned int calculateTSPcost_kruskal(TspNodeGraph *root, unsigned int kruskal_level);
	unsigned int calculateTSPcost_kruskal_backward(TspNodeGraph *root, unsigned int kruskal_level);

	void rearrangeUavAfterClustering(unsigned int kruskal_level, int availableUAV, std::list <TspNodeGraph *> &rootList);

	int updateUavPathToNextPoi(Uav *uav, unsigned int t, bool reserve);
	int updateUavPathToNextPoi_FromStopsSet(unsigned int &risStartStop, std::vector<Stops *> &startPoiNeigh, Uav *uav, unsigned int t, bool reserve);

protected:
	std::map<unsigned int, TspNodeGraph *> tspGraphMap;
	std::map<int, TspNodeGraph *> uavTSP;
	std::map<int, std::list<ArcGraph *> > uavArcMapList;
	TspNodeGraph *mstRootNode;

	std::vector< std::list <TspNodeGraph *> > kruskal_mstRootNode;
	std::vector< std::vector<int> > clusterIndexUav;
	int bestClusteringLayer;

	std::list<Uav *> uav_extra_backup;
};

#endif /* TSPGRAPH_H_ */
