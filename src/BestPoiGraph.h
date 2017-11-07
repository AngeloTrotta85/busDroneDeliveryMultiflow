/*
 * BestPoiGraph.h
 *
 *  Created on: 27 lug 2017
 *      Author: angelo
 */

#ifndef BESTPOIGRAPH_H_
#define BESTPOIGRAPH_H_

#include "FlowGraph.h"

class BestPoiGraph : public FlowGraph {
public:
	BestPoiGraph(Simulator *s) : FlowGraph(s) {
	};
	virtual ~BestPoiGraph();

	virtual void initPoi(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName);
	virtual void activateUavFlow(unsigned int time, std::list<Uav *> &uavList);

	virtual void setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap);

	int updateUavPathBestPoi(Uav *uav, unsigned int t, bool reserve);

private:
	std::map<int, std::list<ArcGraph *> > uavArcMapList;
	std::map<int, Poi *> poiDestMap;
	std::list<Poi *> backup_list_poi;
};

#endif /* BESTPOIGRAPH_H_ */
