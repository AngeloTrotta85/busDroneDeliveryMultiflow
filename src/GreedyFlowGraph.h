/*
 * GreedyFlowGraph.h
 *
 *  Created on: 18 dic 2017
 *      Author: angelo
 */

#ifndef GREEDYFLOWGRAPH_H_
#define GREEDYFLOWGRAPH_H_

#include "FlowGraph.h"

class GreedyFlowGraph : public FlowGraph{
public:
	GreedyFlowGraph(Simulator *s);
	virtual ~GreedyFlowGraph();

	virtual void generateUavPath(unsigned int time, Uav *u);
};

#endif /* GREEDYFLOWGRAPH_H_ */
