/*
 * WorstUav.h
 *
 *  Created on: 28 lug 2017
 *      Author: angelo
 */

#ifndef WORSTUAV_H_
#define WORSTUAV_H_

#include "FlowGraph.h"

class WorstUav : public FlowGraph {
public:
	WorstUav(Simulator *s) : FlowGraph(s) {
	};
	virtual ~WorstUav();

	virtual void initPoi(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName);
	virtual void activateUavFlow(unsigned int time, std::list<Uav *> &uavList);

	virtual void setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap);

	int updateUavPathBestPoi(Uav *uav, unsigned int t, bool reserve);

private:
	std::map<int, std::list<ArcGraph *> > uavArcMapList;
	std::map<int, Poi *> poiDestMap;
	std::list<Poi *> backup_list_poi;

	std::list<Poi *> savingPOIs;
};


#endif /* WORSTUAV_H_ */
