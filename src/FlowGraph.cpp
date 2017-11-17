/*
 * FlowGraph.cpp
 *
 *  Created on: 19 lug 2017
 *      Author: angelo
 */

#include "FlowGraph.h"

#include <chrono>

using namespace std;

FlowGraph::FlowGraph(Simulator *s) {
	// TODO Auto-generated constructor stub
	sim = s;
	swapCount = 0;
}

FlowGraph::~FlowGraph() {
	for (auto& m : graphMapMap) {
		m.second.clear();
	}
	graphMapMap.clear();
}

void FlowGraph::addInitStop(unsigned int stop, struct std::tm time) {
	NodeGraph *ng = new NodeGraph(day_tm2seconds(time), stop, NodeGraph::STOP);

	//TODO DEPRECATED remove graphMapMap
	if (graphMapMap.count(stop) == 0) {
		std::map<unsigned int, NodeGraph *> newMap;
		graphMapMap[stop] = newMap;
	}
	graphMapMap[stop][day_tm2seconds(time)] = ng;
	////////////////////////////////////

	if (graphMapMapMap.count(NodeGraph::STOP) == 0) {
		std::map<unsigned int, std::map<unsigned int, NodeGraph *> > newMap;
		graphMapMapMap[NodeGraph::STOP] = newMap;
	}
	if (graphMapMapMap[NodeGraph::STOP].count(stop) == 0) {
		std::map<unsigned int, NodeGraph *> newMap;
		graphMapMapMap[NodeGraph::STOP][stop] = newMap;
	}
	graphMapMapMap[NodeGraph::STOP][stop][day_tm2seconds(time)] = ng;
}

void FlowGraph::addInitHome(unsigned int home, struct std::tm time) {
	NodeGraph *ng = new NodeGraph(day_tm2seconds(time), home, NodeGraph::HOME);

	if (graphMapMapMap.count(NodeGraph::HOME) == 0) {
		std::map<unsigned int, std::map<unsigned int, NodeGraph *> > newMap;
		graphMapMapMap[NodeGraph::HOME] = newMap;
	}
	if (graphMapMapMap[NodeGraph::HOME].count(home) == 0) {
		std::map<unsigned int, NodeGraph *> newMap;
		graphMapMapMap[NodeGraph::HOME][home] = newMap;
	}
	graphMapMapMap[NodeGraph::HOME][home][day_tm2seconds(time)] = ng;
}

void FlowGraph::addInitDeliveryPoint(unsigned int dp, struct std::tm time) {
	NodeGraph *ng = new NodeGraph(day_tm2seconds(time), dp, NodeGraph::DELIVERY_POINT);

	if (graphMapMapMap.count(NodeGraph::DELIVERY_POINT) == 0) {
		std::map<unsigned int, std::map<unsigned int, NodeGraph *> > newMap;
		graphMapMapMap[NodeGraph::DELIVERY_POINT] = newMap;
	}
	if (graphMapMapMap[NodeGraph::DELIVERY_POINT].count(dp) == 0) {
		std::map<unsigned int, NodeGraph *> newMap;
		graphMapMapMap[NodeGraph::DELIVERY_POINT][dp] = newMap;
	}
	graphMapMapMap[NodeGraph::DELIVERY_POINT][dp][day_tm2seconds(time)] = ng;
}

void FlowGraph::addFollowingStop(unsigned int stop, struct std::tm time) {
	//TODO DEPRECATED remove graphMapMap
	if (graphMapMap.count(NodeGraph::STOP) > 0) {
		NodeGraph *ng = new NodeGraph(day_tm2seconds(time), stop, NodeGraph::STOP);
		graphMapMap[stop][day_tm2seconds(time)] = ng;
	}
	////////////////////////////////////

	if (graphMapMapMap.count(NodeGraph::STOP) > 0) {
		if (graphMapMapMap[NodeGraph::STOP].count(stop) > 0) {
			NodeGraph *ng = new NodeGraph(day_tm2seconds(time), stop, NodeGraph::STOP);
			graphMapMapMap[NodeGraph::STOP][stop][day_tm2seconds(time)] = ng;
		}
	}
}

void FlowGraph::addFollowingHome(unsigned int home, struct std::tm time) {
	if (graphMapMapMap.count(NodeGraph::HOME) > 0) {
		if (graphMapMapMap[NodeGraph::HOME].count(home) > 0) {
			NodeGraph *ng = new NodeGraph(day_tm2seconds(time), home, NodeGraph::HOME);
			graphMapMapMap[NodeGraph::HOME][home][day_tm2seconds(time)] = ng;
		}
	}
}

void FlowGraph::addFollowingDeliveryPoint(unsigned int dp, struct std::tm time) {
	if (graphMapMapMap.count(NodeGraph::DELIVERY_POINT) > 0) {
		if (graphMapMapMap[NodeGraph::DELIVERY_POINT].count(dp) > 0) {
			NodeGraph *ng = new NodeGraph(day_tm2seconds(time), dp, NodeGraph::DELIVERY_POINT);
			graphMapMapMap[NodeGraph::DELIVERY_POINT][dp][day_tm2seconds(time)] = ng;
		}
	}
}


void FlowGraph::generateStaticArcsStop(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE at) {
	if ((graphMapMapMap.count(NodeGraph::STOP) > 0) && (graphMapMapMap[NodeGraph::STOP].count(id) > 0)) {

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->src = graphMapMapMap[NodeGraph::STOP][id][day_tm2seconds(time1)];
		newArcStop->dest = graphMapMapMap[NodeGraph::STOP][id][day_tm2seconds(time2)];
		graphMapMapMap[NodeGraph::STOP][id][day_tm2seconds(time1)]->arcs.push_back(newArcStop);
	}
}

void FlowGraph::generateStaticArcsHome(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE at) {
	if ((graphMapMapMap.count(NodeGraph::HOME) > 0) && (graphMapMapMap[NodeGraph::HOME].count(id) > 0)) {

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->src = graphMapMapMap[NodeGraph::HOME][id][day_tm2seconds(time1)];
		newArcStop->dest = graphMapMapMap[NodeGraph::HOME][id][day_tm2seconds(time2)];
		graphMapMapMap[NodeGraph::HOME][id][day_tm2seconds(time1)]->arcs.push_back(newArcStop);
	}
}

void FlowGraph::generateStaticArcsFromRoute(BusRoute *br, struct std::tm timeBegin, struct std::tm timeEnd) {
	unsigned int nodeStart, nodeDest;
	unsigned int timeStart, timeDest;
	auto it = br->orderedList_Stops.begin();

	if (it != br->orderedList_Stops.end()) {

		nodeStart = it->stop_id;
		timeStart = day_tm2seconds(it->departure_time);
		it++;

		while (it != br->orderedList_Stops.end()) {

			nodeDest = it->stop_id;
			timeDest = day_tm2seconds(it->arrival_time);

			if (	(timeDest >= day_tm2seconds(timeBegin)) && (timeDest <= day_tm2seconds(timeEnd)) &&
					(timeStart >= day_tm2seconds(timeBegin)) && (timeStart <= day_tm2seconds(timeEnd)) ) {

				if (timeDest > timeStart) {
					ArcGraph *newArcStop = new ArcGraph();

					newArcStop->arc_t = ArcGraph::BUS;
					newArcStop->src = graphMapMapMap[NodeGraph::STOP][nodeStart][timeStart];
					newArcStop->dest = graphMapMapMap[NodeGraph::STOP][nodeDest][timeDest];

					graphMapMapMap[NodeGraph::STOP][nodeStart][timeStart]->arcs.push_back(newArcStop);
				}
			}

			it++;
			nodeStart = nodeDest;
			timeStart = timeDest;
		}
	}
}

void FlowGraph::generateFlyArcs(struct std::tm s_time, NodeGraph::NODE_TYPE s_type, unsigned int s_id,
		struct std::tm a_time, NodeGraph::NODE_TYPE a_type, unsigned int a_id, ArcGraph::ARC_TYPE at) {
	if (((graphMapMapMap.count(s_type) > 0) && (graphMapMapMap[s_type].count(s_id) > 0) && (graphMapMapMap[s_type][s_id].count(day_tm2seconds(s_time)) > 0)) &&
			((graphMapMapMap.count(a_type) > 0) && (graphMapMapMap[a_type].count(a_id) > 0) && (graphMapMapMap[a_type][a_id].count(day_tm2seconds(a_time)) > 0)) ){

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->src = graphMapMapMap[s_type][s_id][day_tm2seconds(s_time)];
		newArcStop->dest = graphMapMapMap[a_type][a_id][day_tm2seconds(a_time)];
		graphMapMapMap[s_type][s_id][day_tm2seconds(s_time)]->arcs.push_back(newArcStop);
	}
}

NodeGraph *FlowGraph::getNodePtr(NodeGraph::NODE_TYPE n_type, unsigned int id, struct std::tm time_tm) {
	NodeGraph *ris = nullptr;

	if ((graphMapMapMap.count(n_type) > 0) && (graphMapMapMap[n_type].count(id) > 0) && (graphMapMapMap[n_type][id].count(day_tm2seconds(time_tm)) > 0)) {
		ris = graphMapMapMap[n_type][id][day_tm2seconds(time_tm)];
	}
	return ris;
}

//void FlowGraph::setUavPosition(struct std::tm time, Uav *uav) {
	//TODO TODO graphMapMap[uav->getPositionStopId()][day_tm2seconds(time)]->uavs.push_back(uav);
//}

void FlowGraph::setUavPosition(NodeGraph *ng, Uav *uav) {
	if (ng != nullptr) {
		graphMapMapMap[ng->node_t][ng->node_id][ng->time]->uavs.push_back(uav);
	}
}

void FlowGraph::setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap) {
	//TODO TODO
	/*
	std::vector<Stops *> lStops;
	for (auto& ss : stopsMap) {
		lStops.push_back(&(ss.second));
	}
	std::random_shuffle ( lStops.begin(), lStops.end() );

	//setto gli altri in modo randomico
	for (auto& uav : remainingUAV) {
		uav->setState(Uav::UAV_FLYING);
		uav->setPositionStopId(lStops[(std::rand() % lStops.size())]->getStopIdNum());

		setUavPosition(time_tm, uav);

		cout << "UAV " << uav->getId() << " set on random stop: " << uav->getPositionStopId() << endl;
	}
*/
}

void FlowGraph::updateUavOnFlow(unsigned int time){

	//cout << time << " BEFORE Active arc: ";
	//for (auto& a : activeArc) {
	//	cout << "Type: " << a->arc_t << "; U_N: " << a->uavOnTheArc.size() << " " << a->src->stop_id << "_" << a->src->time << "->" << a->dest->stop_id << "_" << a->dest->time << ". ";
	//}
	//cout << endl;

	//TODO TODO
/*
	for (auto itA = activeArc.begin(); itA != activeArc.end(); itA++) {
		ArcGraph *a = *itA;
		if (a->dest->time == time) {
			for (auto& u : a->uavOnTheArc) {
				graphMapMap[a->dest->stop_id][a->dest->time]->uavs.push_back(u);

				u->setPositionStopId(a->dest->stop_id);
				u->setState(Uav::UAV_FLYING);	// for sure is flying if ends an arc

				//if (a->arc_t == ArcGraph::BUS) {
				//	cout << time << " UAV " << u->getId() << " is arriving at stop " << a->dest->stop_id << endl;
				//}
				//cout << "Uav " << u->getId() << " is leaving the BUS at time " << time << endl;
			}

			//itA = activeArc.erase(itA);
		}
	}

	// MAH! FACCIO UNA COPIA
	std::list<ArcGraph *> activeArc_copy;
	for (auto& a : activeArc) {
		activeArc_copy.push_back(a);
	}
	activeArc.clear();
	for (auto& a : activeArc_copy) {
		if (a->dest->time > time) {
			activeArc.push_back(a);
		}
	}

	*/

	//cout << time << " AFTER  Active arc: ";
	//for (auto& a : activeArc) {
	//	cout << "Type: " << a->arc_t << "; U_N: " << a->uavOnTheArc.size() << " " << a->src->stop_id << "_" << a->src->time << "->" << a->dest->stop_id << "_" << a->dest->time << ". ";
	//}
	//cout << endl;
}

void FlowGraph::getMinimumPathAll(std::map<unsigned int, std::list<ArcGraph *> > &arcMapList, unsigned int stopStart, unsigned int timeStart) {
	std::list<NodeGraph *> q;
	std::map<unsigned int, NodeGraph *> minimumTimeMap;

	//TODO TODO
/*

	//cout << endl << "START getMinimumPathAll" << endl; fflush(stdout);

	//reset
	for (auto& l : arcMapList) {
		l.second.clear();
	}
	arcMapList.clear();

	for (auto n : graphMapMap) {
		std::list<ArcGraph *> *l = new std::list<ArcGraph *>();
		arcMapList[n.first] = *l;
	}


	if ((graphMapMap.count(stopStart) > 0) && (graphMapMap[stopStart].count(timeStart) > 0)) {

		//cout << "The node exists" << endl; fflush(stdout);

		NodeGraph *root = graphMapMap[stopStart][timeStart];

		// init the full graph
		for (auto n : graphMapMap) {
			for (auto g : n.second) {
				NodeGraph *act = g.second;
				act->bfs_state = NodeGraph::NOT_VISITED;
				act->distenace_from_root = std::numeric_limits<int>::max();
				act->predecessor_arc = nullptr;
			}
		}

		q.push_back(root);		// Q := queue initialized with {root}
		minimumTimeMap[stopStart] = root;
		root->distenace_from_root = 0;

		//cout << "starting making the BFS" << endl; fflush(stdout);

		while (!q.empty()) {
			NodeGraph *current = q.front(); 				//current = Q.dequeue()
			q.pop_front();

			current->bfs_state = NodeGraph::VISITED;

			// check if I need to go forward or I found already all the minimum paths
			//cout << "starting checking end" << endl; fflush(stdout);
			if ( (current->node_id == root->node_id) && (current->predecessor_arc != nullptr) && (current->predecessor_arc->arc_t == ArcGraph::STOP) ) {
				bool allOK = true;

				for (auto n : arcMapList) {
					if (minimumTimeMap.count(n.first) == 0) { // node not found yet
						allOK = false;
						break;
					}
					else if (minimumTimeMap[n.first]->distenace_from_root > current->distenace_from_root) {	// I can enhance this node
						allOK = false;
						break;
					}
				}

				if (allOK) {
					// found the minimum for everybody
					break;
				}
			}
			//cout << "end checking end" << endl; fflush(stdout);

			if (minimumTimeMap.count(current->stop_id) == 0){
				minimumTimeMap[current->stop_id] = current;
			}
			else if (minimumTimeMap[current->stop_id]->distenace_from_root > current->distenace_from_root){
				minimumTimeMap[current->stop_id] = current;
			}

			for (auto a : current->arcs) {					//for each node n that is adjacent to current:
				NodeGraph *adj = a->dest;

				if (adj->bfs_state == NodeGraph::NOT_VISITED) { // if n is not labeled as discovered:
					adj->bfs_state = NodeGraph::DISCOVERED;	// label n as discovered
					adj->predecessor_arc = a;				// n.parent = current
					adj->distenace_from_root = current->distenace_from_root + (a->dest->time - a->src->time);

					q.push_back(adj);						// Q.enqueue(n)
				}
			}
		}

		//cout << "end making the BFS" << endl; fflush(stdout);

		// finished the BFS
		for (auto n : arcMapList) {
			if (minimumTimeMap.count(n.first) > 0){
				NodeGraph *act = minimumTimeMap[n.first];
				while (act->predecessor_arc != nullptr) {
					arcMapList[n.first].push_front(act->predecessor_arc);
					act = act->predecessor_arc->src;
				}
			}
		}

//		cout << "end making the results" << endl; fflush(stdout);
//
//		// print example
//		for (auto n : arcMapList) {
//			cout << "BFS for node " << n.first << " = ";
//			for (auto a : n.second) {
//				cout << a->src->stop_id << "_" << a->src->time << "->" << a->dest->stop_id << "_" << a->dest->time << " | ";
//			}
//			cout << endl;
//		}
//
//		cout << "END1 getMinimumPathAll" << endl; fflush(stdout);

		//exit(EXIT_SUCCESS);
	}
	*/
	//cout << "END2 getMinimumPathAll" << endl; fflush(stdout);
}

void FlowGraph::getMinimumPathToFew(std::map<unsigned int, std::list<ArcGraph *> > &arcMapList, std::map<unsigned int, unsigned int > &arcMapListCost, unsigned int stopStart, unsigned int timeStart, std::vector<Stops *> &stopsEnd) {
	std::list<NodeGraph *> q;
	std::map<unsigned int, NodeGraph *> minimumTimeMap;

	//TODO TODO
/*
	//cout << endl << "getMinimumPathToFew - START" << endl; fflush(stdout);
	//cout << "getMinimumPathToFew - Looking for path from stop: " << stopStart << "_" << timeStart << endl;
	//cout << "getMinimumPathToFew - To the end nodes: ";
	//for (auto& nn : stopsEnd) {
	//	cout << nn->getStopIdNum() << " ";
	//}
	//cout << endl;

	//reset
	for (auto& l : arcMapList) {
		l.second.clear();
	}
	arcMapList.clear();
	arcMapListCost.clear();

	for (auto& n : graphMapMap) {
		std::list<ArcGraph *> *l = new std::list<ArcGraph *>();
		arcMapList[n.first] = *l;
		arcMapListCost[n.first] = std::numeric_limits<unsigned int>::max();
	}

	if ((graphMapMap.count(stopStart) > 0) && (graphMapMap[stopStart].count(timeStart) > 0)) {
		std::list<NodeGraph *> usedNode;

		//cout << "The node exists" << endl; fflush(stdout);

		NodeGraph *root = graphMapMap[stopStart][timeStart];

		// init the full graph
		//for (auto n : graphMapMap) {
		//	for (auto g : n.second) {
		//		NodeGraph *act = g.second;
		//		act->bfs_state = NodeGraph::NOT_VISITED;
		//		act->distenace_from_root = std::numeric_limits<int>::max();
		//		act->predecessor_arc = nullptr;
		//	}
		//}

		q.push_back(root);		// Q := queue initialized with {root}
		minimumTimeMap[stopStart] = root;
		root->distenace_from_root = 0;

		//cout << "starting making the BFS" << endl; fflush(stdout);

		while (!q.empty()) {
			NodeGraph *current = q.front(); 				//current = Q.dequeue()
			q.pop_front();

			current->bfs_state = NodeGraph::VISITED;

			// check if I need to go forward or I found already all the minimum paths
			//cout << "starting checking end" << endl; fflush(stdout);
			if ( (current->stop_id == root->stop_id) && (current->predecessor_arc != nullptr) && (current->predecessor_arc->arc_t == ArcGraph::STOP) ) {
				bool allOK = true;

				for (auto n : stopsEnd) {
					if (minimumTimeMap.count(n->getStopIdNum()) == 0) { // node not found yet
						allOK = false;
						break;
					}
					else if (minimumTimeMap[n->getStopIdNum()]->distenace_from_root > current->distenace_from_root) {	// I can enhance this node
						allOK = false;
						break;
					}
				}

				if (allOK) {
					// found the minimum for everybody
					break;
				}
			}
			//cout << "end checking end" << endl; fflush(stdout);

			if (minimumTimeMap.count(current->stop_id) == 0){
				minimumTimeMap[current->stop_id] = current;
			}
			else if (minimumTimeMap[current->stop_id]->distenace_from_root > current->distenace_from_root){
				minimumTimeMap[current->stop_id] = current;
			}

			for (auto& a : current->arcs) {					//for each node n that is adjacent to current:
				if ( (a->arc_t != ArcGraph::COVER) && (!(a->reserved)) ) {
				//if (a->arc_t != ArcGraph::COVER) {
					NodeGraph *adj = a->dest;

					if (adj->bfs_state == NodeGraph::NOT_VISITED) { // if n is not labeled as discovered:
//						bool onlySTOP = true;
//						NodeGraph *true_adj = adj;
//						ArcGraph *actPredArc = a;
//						while (onlySTOP) {
//							NodeGraph *nextIfStop;
//							for (auto& at : true_adj->arcs) {
//								if (at->arc_t == ArcGraph::BUS) {
//									onlySTOP = false;
//									break;
//								}
//								else if (at->arc_t == ArcGraph::STOP) {
//									nextIfStop = at->dest;
//									actPredArc = at;
//								}
//							}
//							if (onlySTOP) {
//								true_adj->bfs_state = NodeGraph::VISITED;	// label n as discovered
//								true_adj->predecessor_arc = a;				// n.parent = current
//								true_adj->distenace_from_root = current->distenace_from_root + (a->dest->time - a->src->time);
//
//								true_adj = nextIfStop;
//							}
//						}


						adj->bfs_state = NodeGraph::DISCOVERED;	// label n as discovered
						adj->predecessor_arc = a;				// n.parent = current
						adj->distenace_from_root = current->distenace_from_root + (a->dest->time - a->src->time);



						q.push_back(adj);						// Q.enqueue(n)

						// improvements
						usedNode.push_back(adj);
					}
				}
			}
		}

		//cout << "end making the BFS" << endl; fflush(stdout);

		// finished the BFS
		for (auto& n : arcMapList) {
			bool isRequested = false;
			for (auto& checkS : stopsEnd) {
				if (checkS->getStopIdNum() == n.first) {
					isRequested = true;
					break;
				}
			}
			if (isRequested) {
				if (minimumTimeMap.count(n.first) > 0){
					unsigned int cost = 0;
					NodeGraph *act = minimumTimeMap[n.first];
					while (act->predecessor_arc != nullptr) {
						arcMapList[n.first].push_front(act->predecessor_arc);
						cost += act->predecessor_arc->dest->time - act->predecessor_arc->src->time;

						act = act->predecessor_arc->src;
					}

					if (arcMapList[n.first].size() > 0) {
						arcMapListCost[n.first] = cost;
					}
				}
			}
		}

		for (auto& un : usedNode) {
			un->bfs_state = NodeGraph::NOT_VISITED;
			un->distenace_from_root = std::numeric_limits<int>::max();
			un->predecessor_arc = nullptr;
		}
//
//		cout << "getMinimumPathToFew - Minimum paths:" << endl;
//		for (auto& n : arcMapList) {
//			cout << "getMinimumPathToFew - BFS for node " << n.first << " = ";
//			for (auto a : n.second) {
//				cout << a->src->stop_id << "_" << a->src->time << "->" << a->dest->stop_id << "_" << a->dest->time << " | ";
//			}
//			cout << endl;
//		}
//
//		cout << "end making the results" << endl; fflush(stdout);
//
//			// print example
//			for (auto n : arcMapList) {
//				cout << "BFS for node " << n.first << " = ";
//				for (auto a : n.second) {
//					cout << a->src->stop_id << "_" << a->src->time << "->" << a->dest->stop_id << "_" << a->dest->time << " | ";
//				}
//				cout << endl;
//			}
//
//			cout << "END1 getMinimumPathAll" << endl; fflush(stdout);

		//exit(EXIT_SUCCESS);
	}
	*/

	//cout << "END2 getMinimumPathAll" << endl; fflush(stdout);
}

void FlowGraph::getMinimumPathFromAll(std::list<ArcGraph *> &arcList, unsigned int stopStart, unsigned int timeStart, unsigned int stopEnd) {
	std::map<unsigned int, std::list<ArcGraph *> > arcMapList;

	getMinimumPathAll(arcMapList, stopStart, timeStart);

	if (arcMapList.count(stopEnd) > 0) {
		for (auto it = arcMapList[stopEnd].begin(); it != arcMapList[stopEnd].end(); it++) {
			arcList.push_back(*it);
		}
	}
}

void FlowGraph::activateUavFlow(unsigned int time, std::list<Uav *> &uavList){
	//TODO TODO
	/*
	for (auto& u : uavList){
		if (u->getState() == Uav::UAV_FLYING) {
			NodeGraph *ng = graphMapMap[u->getPositionStopId()][time];
			if (u->isCovering()) {
				for (auto a : ng->arcs) {
					//if ( 	(a->dest->stop_id == u->getPositionStopId()) &&
					//		(a->dest->time == (time + 1)) &&
					//		(a->arc_t == ArcGraph::COVER) && (a->p != nullptr) && (a->p->getPoiIdNum() == u->getCoveringPoiId())) {
					if ( (a->arc_t == ArcGraph::COVER) && (a->p != nullptr) && (a->p->getPoiIdNum() == u->getCoveringPoiId()) ) {
						a->uavOnTheArc.push_back(u);
						activeArc.push_back(a);

						break;
					}
				}
			}
			else {
				bool foundBus = false;
				for (auto a : ng->arcs) {
					//search for buses
					if (a->arc_t == ArcGraph::BUS) {
						a->uavOnTheArc.push_back(u);
						activeArc.push_back(a);

						u->setState(Uav::UAV_ONBUS);

						foundBus = true;
						break;
					}
				}

				if (!foundBus) {
					for (auto a : ng->arcs) {
						//if ( 	(a->dest->stop_id == u->getPositionStopId()) && (a->src->stop_id == u->getPositionStopId()) &&
						//		(a->dest->time == (time + 1)) &&
						//		(a->arc_t == ArcGraph::STOP) && (a->p == nullptr) ) {
						if ((a->arc_t == ArcGraph::STOP) && (a->uavOnTheArc.size() == 0)) {
							a->uavOnTheArc.push_back(u);
							activeArc.push_back(a);
							//cout << "Uav " << u->getId() << " is taking the BUS at time " << time << endl;
							break;
						}
					}
				}
			}
		}
		else { // (u->getState() == Uav::UAV_ONBUS)
			// DO NOTHING
		}
	}
	*/
}

void FlowGraph::execute(struct std::tm time, std::list<Uav *> &uavList){
	unsigned int t = day_tm2seconds(time);

	//TODO only test. remove
	//std::map<unsigned int, std::list<ArcGraph *> > ag;
	//getMinimumPathAll(ag, 469, graphMapMap.begin()->second.begin()->first);

	//cout << "Time: " << t << " - Begin activateUavFlow" << endl; fflush(stdout);
	activateUavFlow(t, uavList);
	//cout << "Time: " << t << " - Begin updateUavOnFlow" << endl; fflush(stdout);
	updateUavOnFlow(t+1);
	//cout << "Time: " << t << " - End updateUavOnFlow" << endl; fflush(stdout);

}
bool FlowGraph::exportDotResult(std::string dotFileName) {

	std::ofstream fout(dotFileName, std::ofstream::out);
	std::map<unsigned int, int> mapStops;
	std::map< std::pair<unsigned int, unsigned int>, bool> mapStopsOK;
	int cNode = 0;
	int sOffest = 3;
	int sSize = 2;

	//TODO TODO
	if (fout.is_open()) {
		fout << "digraph G{" << endl;

		/*
		//for (auto sMap : graphMapMap){
		for (auto itM = graphMapMap.begin(); itM != graphMapMap.end(); itM++){
			//for (auto ngMap : sMap.second){
			for (auto ngMap : itM->second){
				NodeGraph *ng = ngMap.second;

				for (auto a : ng->arcs) {
					if (!a->uavOnTheArc.empty()) {

						if (mapStops.count(a->src->stop_id) == 0) {
							mapStops[a->src->stop_id] = cNode++;
						}
						if (mapStops.count(a->dest->stop_id) == 0) {
							mapStops[a->dest->stop_id] = cNode++;
						}

						//fout << "node" << a->src->stop_id << "_" << a->src->time
						//		<< " -> node" << a->dest->stop_id << "_" << a->dest->time << ";" << endl;

						if (mapStopsOK.count(make_pair(a->src->stop_id, a->src->time)) == 0) {
							//fout << "node" << a->src->stop_id << "_" << a->src->time <<
							//		" [pos=\"" << mapStops[a->src->stop_id] * sOffest << "," << a->src->time * sOffest <<
							//		"!\" width=" << sSize << ", height=" << sSize << "]" << endl;
							fout << "node" << a->src->stop_id << "_" << a->src->time <<
									" [pos=\"" << a->src->time * sOffest << "," << mapStops[a->src->stop_id] * sOffest <<
									"!\" width=" << sSize << ", height=" << sSize << "]" << endl;

							mapStopsOK[make_pair(a->src->stop_id, a->src->time)] = true;
						}

						if (mapStopsOK.count(make_pair(a->dest->stop_id, a->dest->time)) == 0) {
							fout << "node" << a->dest->stop_id << "_" << a->dest->time <<
									" [pos=\"" << a->dest->time * sOffest << "," << mapStops[a->dest->stop_id] * sOffest <<
									"!\" width=" << sSize << ", height=" << sSize << "]" << endl;

							mapStopsOK[make_pair(a->dest->stop_id, a->dest->time)] = true;
						}

						//fout << "node" << a->src->stop_id << "_" << a->src->time
						//		<< " -> node" << a->dest->stop_id << "_" << a->dest->time << ";" << endl;

					}
				}
			}


		}

		//for (auto sMap : graphMapMap){
		for (auto itM = graphMapMap.begin(); itM != graphMapMap.end(); itM++){
			//for (auto ngMap : sMap.second){
			for (auto ngMap : itM->second){
				NodeGraph *ng = ngMap.second;

				for (auto a : ng->arcs) {
					if (!a->uavOnTheArc.empty()) {
						//fout << "node" << a->src->stop_id << "_" << a->src->time
						//		<< " -> node" << a->dest->stop_id << "_" << a->dest->time << ";" << endl;

						fout << "node" << a->src->stop_id << "_" << a->src->time
								<< " -> node" << a->dest->stop_id << "_" << a->dest->time << ";" << endl;

						//fout << "S" << count << " [shape=\"point\" color=\"" << color << "\" pos=\""
						//	<< p.x << "," << p.y << "!\" width=" << pSize << ", height=" << pSize << "]" << endl;
					}
				}
			}
		}*/

		fout << "}" << endl;

		fout.close();

		return true;
	}
	return false;
}

bool FlowGraph::exportDotFullEmptyGraph(std::string dotFileName) {

	std::ofstream fout(dotFileName, std::ofstream::out);
	std::map<unsigned int, int> mapStops;
	std::map< std::pair<unsigned int, unsigned int>, bool> mapStopsOK;
	int cNode = 0;
	int sOffest = 3;
	int sSize = 2;

	int nHome = graphMapMapMap[NodeGraph::HOME].size();
	int nDP = graphMapMapMap[NodeGraph::DELIVERY_POINT].size();
	//int nStop = graphMapMapMap[NodeGraph::STOP].size();
	unsigned int time0 = graphMapMapMap[NodeGraph::HOME].begin()->second.begin()->second->time;

	if (fout.is_open()) {
		fout << "digraph G{" << endl;

		for (auto& l1map : graphMapMapMap) {
			//for (auto& l2map : l1map.second) {
			cNode = 0;
			for (auto it2map = l1map.second.begin(); it2map != l1map.second.end(); ++it2map){
				//for (auto& l3map : l2map.second) {
				for (auto& l3map : it2map->second) {
					NodeGraph *act = l3map.second;
					int hOffset = 0;
					std::string shape = std::string("shape=ellipse");

					switch (act->node_t) {
					case NodeGraph::HOME:
						hOffset = 0;
						shape = std::string("shape=diamond");
						break;
					case NodeGraph::DELIVERY_POINT:
						hOffset = nHome * sOffest;
						shape = std::string("shape=box");
						break;
					case NodeGraph::STOP:
						hOffset = (nHome + nDP) * sOffest;
						shape = std::string("shape=ellipse");
						break;
					}

					fout << "node_" << act->node_t << "_" << act->node_id << "_" << act->time <<
							" [pos=\"" << (act->time - time0) * sOffest << "," << hOffset + (cNode * sOffest) <<
							"!\" width=" << sSize << ", height=" << sSize << ", " << shape << "]" << endl;
				}

				++cNode;
			}
		}

		fout << "}" << endl;
		fout.close();
		return true;
	}
	return false;
}



