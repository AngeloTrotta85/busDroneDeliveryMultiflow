/*
 * FlowGraph.cpp
 *
 *  Created on: 19 lug 2017
 *      Author: angelo
 */

#include "FlowGraph.h"
#include "Simulator.h"

#include <chrono>

using namespace std;

FlowGraph::FlowGraph(Simulator *s) {
	// TODO Auto-generated constructor stub
	sim = s;
	swapCount = 0;
}

FlowGraph::~FlowGraph() {
	for (auto& t : graphMapMapMap) {
		for (auto& id : t.second) {
			for (auto& n : id.second) {
				delete (n.second);
			}
			id.second.clear();
		}
		t.second.clear();
	}
	graphMapMapMap.clear();
}

void FlowGraph::addInitStop(Stops *st_ptr, unsigned int stop, struct std::tm time) {
	NodeGraph *ng = new NodeGraph(day_tm2seconds(time), stop, NodeGraph::STOP);
	ng->stop_ptr = st_ptr;

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

void FlowGraph::addInitHome(Home *h_ptr, unsigned int home, struct std::tm time) {
	NodeGraph *ng = new NodeGraph(day_tm2seconds(time), home, NodeGraph::HOME);
	ng->home_ptr = h_ptr;

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

void FlowGraph::addInitDeliveryPoint(DeliveryPoint *dp_ptr, unsigned int dp, struct std::tm time) {
	NodeGraph *ng = new NodeGraph(day_tm2seconds(time), dp, NodeGraph::DELIVERY_POINT);
	ng->dp_ptr = dp_ptr;

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

void FlowGraph::addFollowingStop(Stops *st_ptr, unsigned int stop, struct std::tm time) {

	if (graphMapMapMap.count(NodeGraph::STOP) > 0) {
		if (graphMapMapMap[NodeGraph::STOP].count(stop) > 0) {
			NodeGraph *ng = new NodeGraph(day_tm2seconds(time), stop, NodeGraph::STOP);
			ng->stop_ptr = st_ptr;
			graphMapMapMap[NodeGraph::STOP][stop][day_tm2seconds(time)] = ng;
		}
	}
}

void FlowGraph::addFollowingHome(Home *h_ptr, unsigned int home, struct std::tm time) {
	if (graphMapMapMap.count(NodeGraph::HOME) > 0) {
		if (graphMapMapMap[NodeGraph::HOME].count(home) > 0) {
			NodeGraph *ng = new NodeGraph(day_tm2seconds(time), home, NodeGraph::HOME);
			ng->home_ptr = h_ptr;
			graphMapMapMap[NodeGraph::HOME][home][day_tm2seconds(time)] = ng;
		}
	}
}

void FlowGraph::addFollowingDeliveryPoint(DeliveryPoint *dp_ptr, unsigned int dp, struct std::tm time) {
	if (graphMapMapMap.count(NodeGraph::DELIVERY_POINT) > 0) {
		if (graphMapMapMap[NodeGraph::DELIVERY_POINT].count(dp) > 0) {
			NodeGraph *ng = new NodeGraph(day_tm2seconds(time), dp, NodeGraph::DELIVERY_POINT);
			ng->dp_ptr = dp_ptr;
			graphMapMapMap[NodeGraph::DELIVERY_POINT][dp][day_tm2seconds(time)] = ng;
		}
	}
}


void FlowGraph::generateStaticArcsStop(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE at, double w_cost) {
	if ((graphMapMapMap.count(NodeGraph::STOP) > 0) && (graphMapMapMap[NodeGraph::STOP].count(id) > 0)) {

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->energyCost_watt = w_cost;
		newArcStop->src = graphMapMapMap[NodeGraph::STOP][id][day_tm2seconds(time1)];
		newArcStop->dest = graphMapMapMap[NodeGraph::STOP][id][day_tm2seconds(time2)];
		graphMapMapMap[NodeGraph::STOP][id][day_tm2seconds(time1)]->arcs.push_back(newArcStop);
	}
}

void FlowGraph::generateStaticArcsHome(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE at, double w_cost) {
	if ((graphMapMapMap.count(NodeGraph::HOME) > 0) && (graphMapMapMap[NodeGraph::HOME].count(id) > 0)) {

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->energyCost_watt = w_cost;
		newArcStop->src = graphMapMapMap[NodeGraph::HOME][id][day_tm2seconds(time1)];
		newArcStop->dest = graphMapMapMap[NodeGraph::HOME][id][day_tm2seconds(time2)];
		graphMapMapMap[NodeGraph::HOME][id][day_tm2seconds(time1)]->arcs.push_back(newArcStop);
	}
}

void FlowGraph::generateStaticArcsDeliveryPoint(unsigned int id, struct std::tm time1, struct std::tm time2, ArcGraph::ARC_TYPE at, double w_cost) {
	if ((graphMapMapMap.count(NodeGraph::DELIVERY_POINT) > 0) && (graphMapMapMap[NodeGraph::DELIVERY_POINT].count(id) > 0)) {

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->energyCost_watt = w_cost;
		newArcStop->src = graphMapMapMap[NodeGraph::DELIVERY_POINT][id][day_tm2seconds(time1)];
		newArcStop->dest = graphMapMapMap[NodeGraph::DELIVERY_POINT][id][day_tm2seconds(time2)];
		graphMapMapMap[NodeGraph::DELIVERY_POINT][id][day_tm2seconds(time1)]->arcs.push_back(newArcStop);
	}
}

void FlowGraph::generateStaticArcsFromRoute(BusRoute *br, struct std::tm timeBegin, struct std::tm timeEnd, double w_cost) {
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
					newArcStop->energyCost_watt = w_cost;
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
		struct std::tm a_time, NodeGraph::NODE_TYPE a_type, unsigned int a_id, ArcGraph::ARC_TYPE at, double w_cost) {
	if (((graphMapMapMap.count(s_type) > 0) && (graphMapMapMap[s_type].count(s_id) > 0) && (graphMapMapMap[s_type][s_id].count(day_tm2seconds(s_time)) > 0)) &&
			((graphMapMapMap.count(a_type) > 0) && (graphMapMapMap[a_type].count(a_id) > 0) && (graphMapMapMap[a_type][a_id].count(day_tm2seconds(a_time)) > 0)) ){

		ArcGraph *newArcStop = new ArcGraph();
		newArcStop->arc_t = at;
		newArcStop->energyCost_watt = w_cost;
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

	for (auto itA = activeArc.begin(); itA != activeArc.end(); itA++) {
		ArcGraph *a = *itA;
		if (a->dest->time == time) {
			for (auto& u : a->uavOnTheArc) {
				Package *deliveredP;
				Battery *battUAV;

				//graphMapMapMap[a->dest->stop_id][a->dest->time]->uavs.push_back(u);
				a->dest->uavs.push_back(u);
				u->setPositionNode(a->dest);

				switch (a->dest->node_t) {
					case NodeGraph::HOME:
						u->setState(Uav::UAV_WAIT_HOME);	// I'm arrived back home
						// release the battery
						battUAV = u->removeBatt();
						if (battUAV != nullptr) {
							a->dest->home_ptr->bm->addBattery(battUAV);
						}
						u->setPosLat(a->dest->home_ptr->getHomeLatNum());
						u->setPosLon(a->dest->home_ptr->getHomeLonNum());
						break;

					case NodeGraph::DELIVERY_POINT:
						u->setState(Uav::UAV_WAIT_DP);	// I reached the delivery point

						deliveredP = u->removeCarryingPackage();
						if (deliveredP != nullptr) {
							a->dest->dp_ptr->manageArrivedPackage(deliveredP);
							u->addDeliveredPackage(1);
						}

						u->setPosLat(a->dest->dp_ptr->getDpLatNum());
						u->setPosLon(a->dest->dp_ptr->getDpLonNum());

						break;

					case NodeGraph::STOP:
					default:
						cerr << "WARNING in updateUavOnFlow. Unexpected NodeGraph: " << a->dest->node_t << endl;
						break;
				}
			}
		}
	}

	// MAH! FACCIO UNA COPIA PER ELIMINARE GLI ARCHI TERMINATI
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

void FlowGraph::getMinimumPathToFew(std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::list<ArcGraph *> > > &arcMapList, std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, unsigned int > > &arcMapListCost, NodeGraph *nodeStart, std::vector<NodeGraph *> &nodesEnd) {
	std::list<NodeGraph *> q;
	std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, NodeGraph *> > minimumTimeMap;

	//reset
	for (auto& l : arcMapList) {
		for (auto& l1 : l.second) {
			l1.second.clear();
		}
		l.second.clear();
	}
	arcMapList.clear();

	for (auto& l : arcMapListCost) {
		l.second.clear();
	}
	arcMapListCost.clear();

	for (auto& nt : graphMapMapMap) {
		for (auto& nid : nt.second) {
			if (arcMapList.count(nt.first) == 0) {
				std::map<unsigned int, std::list<ArcGraph *> > *new_map = new std::map<unsigned int, std::list<ArcGraph *> >();
				arcMapList[nt.first] = *new_map;
			}
			std::list<ArcGraph *> *l = new std::list<ArcGraph *>();
			arcMapList[nt.first][nid.first] = *l;

			if (arcMapListCost.count(nt.first) == 0) {
				std::map<unsigned int, unsigned int> *new_map = new std::map<unsigned int, unsigned int>();
				arcMapListCost[nt.first] = *new_map;
			}
			arcMapListCost[nt.first][nid.first] = std::numeric_limits<unsigned int>::max();
		}
	}

	if (	(graphMapMapMap.count(nodeStart->node_t) > 0) &&
			(graphMapMapMap[nodeStart->node_t].count(nodeStart->node_id) > 0) &&
			(graphMapMapMap[nodeStart->node_t][nodeStart->node_id].count(nodeStart->time) > 0) ) {

		std::list<NodeGraph *> usedNode;
		NodeGraph *root = nodeStart;

		usedNode.push_back(root);
		q.push_back(root);		// Q := queue initialized with {root}
		minimumTimeMap[root->node_t][root->node_id] = root;
		root->distenace_from_root = 0;
		root->main_path_check = true;

		//cout << "starting making the BFS" << endl; fflush(stdout);

		while (!q.empty()) {
			NodeGraph *current = q.front(); 				//current = Q.dequeue()
			q.pop_front();

			current->bfs_state = NodeGraph::VISITED;

			// check if I need to go forward or I found already all the minimum paths
			//cout << "starting checking end" << endl; fflush(stdout);
			if (current->main_path_check) {
			//if ( 	(current->node_id == root->node_id) &&
			//		(current->node_t == root->node_t) &&
			//		(current->predecessor_arc != nullptr) &&
			//		(current->predecessor_arc->arc_t == ArcGraph::STOP) ) {
				bool allOK = true;

				for (auto n : nodesEnd) {
					if ((minimumTimeMap.count(n->node_t) == 0) || (minimumTimeMap[n->node_t].count(n->node_id) == 0)) { // node not found yet
						allOK = false;
						break;
					}
					else if (minimumTimeMap[n->node_t][n->node_id]->distenace_from_root > current->distenace_from_root) {	// I can enhance this node
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

			if (minimumTimeMap.count(current->node_t) == 0){
				std::map<unsigned int, NodeGraph *> *nm = new std::map<unsigned int, NodeGraph *>();
				(*nm)[current->node_id] = current;
				minimumTimeMap[current->node_t] = *nm;
			}
			else if (minimumTimeMap[current->node_t].count(current->node_id) == 0) {
				minimumTimeMap[current->node_t][current->node_id] = current;
			}
			else if (minimumTimeMap[current->node_t][current->node_id]->distenace_from_root > current->distenace_from_root){
				minimumTimeMap[current->node_t][current->node_id] = current;
			}

			for (auto& a : current->arcs) {					//for each node n that is adjacent to current:
				//if ( (a->arc_t != ArcGraph::COVER) && (!(a->reserved)) ) {
				if ( !(a->reserved) ) {
					NodeGraph *adj = a->dest;

					if (adj->bfs_state == NodeGraph::NOT_VISITED) { // if n is not labeled as discovered:

						adj->bfs_state = NodeGraph::DISCOVERED;	// label n as discovered
						adj->predecessor_arc = a;				// n.parent = current
						adj->distenace_from_root = current->distenace_from_root + (a->dest->time - a->src->time);

						if ( (a->arc_t == ArcGraph::STOP) && (current->main_path_check) &&
								(current->node_id == adj->node_id) && (current->node_t == adj->node_t) ) {
							adj->main_path_check = true;
						}

						q.push_back(adj);						// Q.enqueue(n)

						// improvements
						usedNode.push_back(adj);
					}
				}
			}
		}

		//cout << "end making the BFS" << endl; fflush(stdout);

		// finished the BFS
		for (auto& n1 : arcMapList) {
			for (auto& n2 : n1.second) {
				bool isRequested = false;
				for (auto& checkS : nodesEnd) {
					if ((checkS->node_t == n1.first) && (checkS->node_id == n2.first)) {
						isRequested = true;
						break;
					}
				}
				if (isRequested) {
					if ((minimumTimeMap.count(n1.first) > 0) && (minimumTimeMap[n1.first].count(n2.first) > 0)){
						unsigned int cost = 0;
						NodeGraph *act = minimumTimeMap[n1.first][n2.first];
						while (act->predecessor_arc != nullptr) {
							arcMapList[n1.first][n2.first].push_front(act->predecessor_arc);
							cost += act->predecessor_arc->dest->time - act->predecessor_arc->src->time;

							act = act->predecessor_arc->src;
						}

						if (arcMapList[n1.first][n2.first].size() > 0) {
							arcMapListCost[n1.first][n2.first] = cost;
						}
					}
				}
			}
		}

		for (auto& un : usedNode) {
			un->bfs_state = NodeGraph::NOT_VISITED;
			un->distenace_from_root = std::numeric_limits<int>::max();
			un->predecessor_arc = nullptr;
			un->main_path_check = false;
		}

		// PRINT DEBUG
		/*cout << "getMinimumPathToFew - Minimum paths:" << endl;
		for (auto& n1 : arcMapList) {
			for (auto& n2 : n1.second) {
				cout << "getMinimumPathToFew - BFS for node " << n1.first << "-" << n2.first << " = " << endl;
				for (auto a : n2.second) {
					cout << "     " << a->src->node_t << "-" << a->src->node_id << "_" << a->src->time << "-" <<
							a->arc_t << "->" << a->dest->node_t << "-" << a->dest->node_id << "_" << a->dest->time << " | " << endl;
				}
				cout << endl;
			}
		}*/

	}
}

void FlowGraph::getMinimumPathToFew_withEnergy(	std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::list<ArcGraph *> > > &arcMapList,
		std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, unsigned int > > &arcMapListCost,
		std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, double > > &arcMapListEnergyCost,
		NodeGraph *nodeStart, std::vector<NodeGraph *> &nodesEnd) {
	std::list<NodeGraph *> q;
	std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, NodeGraph *> > minimumTimeMap;

	//reset
	for (auto& l : arcMapList) {
		for (auto& l1 : l.second) {
			l1.second.clear();
		}
		l.second.clear();
	}
	arcMapList.clear();

	for (auto& l : arcMapListCost) {
		l.second.clear();
	}
	arcMapListCost.clear();

	for (auto& l : arcMapListEnergyCost) {
		l.second.clear();
	}
	arcMapListEnergyCost.clear();

	for (auto& nt : graphMapMapMap) {
		for (auto& nid : nt.second) {
			if (arcMapList.count(nt.first) == 0) {
				std::map<unsigned int, std::list<ArcGraph *> > *new_map = new std::map<unsigned int, std::list<ArcGraph *> >();
				arcMapList[nt.first] = *new_map;
			}
			std::list<ArcGraph *> *l = new std::list<ArcGraph *>();
			arcMapList[nt.first][nid.first] = *l;

			if (arcMapListCost.count(nt.first) == 0) {
				std::map<unsigned int, unsigned int> *new_map = new std::map<unsigned int, unsigned int>();
				arcMapListCost[nt.first] = *new_map;
			}
			arcMapListCost[nt.first][nid.first] = std::numeric_limits<unsigned int>::max();

			if (arcMapListEnergyCost.count(nt.first) == 0) {
				std::map<unsigned int, double> *new_map = new std::map<unsigned int, double>();
				arcMapListEnergyCost[nt.first] = *new_map;
			}
			arcMapListEnergyCost[nt.first][nid.first] = std::numeric_limits<double>::max();
		}
	}

	if (	(graphMapMapMap.count(nodeStart->node_t) > 0) &&
			(graphMapMapMap[nodeStart->node_t].count(nodeStart->node_id) > 0) &&
			(graphMapMapMap[nodeStart->node_t][nodeStart->node_id].count(nodeStart->time) > 0) ) {

		std::list<NodeGraph *> usedNode;
		NodeGraph *root = nodeStart;

		usedNode.push_back(root);
		q.push_back(root);		// Q := queue initialized with {root}
		minimumTimeMap[root->node_t][root->node_id] = root;
		root->distenace_from_root = 0;
		root->main_path_check = true;

		//cout << "starting making the BFS" << endl; fflush(stdout);

		while (!q.empty()) {
			NodeGraph *current = q.front(); 				//current = Q.dequeue()
			q.pop_front();

			current->bfs_state = NodeGraph::VISITED;

			// check if I need to go forward or I found already all the minimum paths
			//cout << "starting checking end" << endl; fflush(stdout);
			if (current->main_path_check) {
				//if ( 	(current->node_id == root->node_id) &&
				//		(current->node_t == root->node_t) &&
				//		(current->predecessor_arc != nullptr) &&
				//		(current->predecessor_arc->arc_t == ArcGraph::STOP) ) {
				bool allOK = true;

				for (auto n : nodesEnd) {
					if ((minimumTimeMap.count(n->node_t) == 0) || (minimumTimeMap[n->node_t].count(n->node_id) == 0)) { // node not found yet
						allOK = false;
						break;
					}
					else if (minimumTimeMap[n->node_t][n->node_id]->distenace_from_root > current->distenace_from_root) {	// I can enhance this node
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

			if (minimumTimeMap.count(current->node_t) == 0){
				std::map<unsigned int, NodeGraph *> *nm = new std::map<unsigned int, NodeGraph *>();
				(*nm)[current->node_id] = current;
				minimumTimeMap[current->node_t] = *nm;
			}
			else if (minimumTimeMap[current->node_t].count(current->node_id) == 0) {
				minimumTimeMap[current->node_t][current->node_id] = current;
			}
			else if (minimumTimeMap[current->node_t][current->node_id]->distenace_from_root > current->distenace_from_root){
				minimumTimeMap[current->node_t][current->node_id] = current;
			}

			for (auto& a : current->arcs) {					//for each node n that is adjacent to current:
				//if ( (a->arc_t != ArcGraph::COVER) && (!(a->reserved)) ) {
				if ( !(a->reserved) ) {
					NodeGraph *adj = a->dest;

					if (adj->bfs_state == NodeGraph::NOT_VISITED) { // if n is not labeled as discovered:

						adj->bfs_state = NodeGraph::DISCOVERED;	// label n as discovered
						adj->predecessor_arc = a;				// n.parent = current
						adj->distenace_from_root = current->distenace_from_root + (a->dest->time - a->src->time);

						if ( (a->arc_t == ArcGraph::STOP) && (current->main_path_check) &&
								(current->node_id == adj->node_id) && (current->node_t == adj->node_t) ) {
							adj->main_path_check = true;
						}

						q.push_back(adj);						// Q.enqueue(n)

						// improvements
						usedNode.push_back(adj);
					}
				}
			}
		}

		//cout << "end making the BFS" << endl; fflush(stdout);

		// finished the BFS
		for (auto& n1 : arcMapList) {
			for (auto& n2 : n1.second) {
				bool isRequested = false;
				for (auto& checkS : nodesEnd) {
					if ((checkS->node_t == n1.first) && (checkS->node_id == n2.first)) {
						isRequested = true;
						break;
					}
				}
				if (isRequested) {
					if ((minimumTimeMap.count(n1.first) > 0) && (minimumTimeMap[n1.first].count(n2.first) > 0)){
						unsigned int cost = 0;
						double ecost = 0;
						NodeGraph *act = minimumTimeMap[n1.first][n2.first];
						while (act->predecessor_arc != nullptr) {
							arcMapList[n1.first][n2.first].push_front(act->predecessor_arc);
							cost += act->predecessor_arc->dest->time - act->predecessor_arc->src->time;
							ecost += act->predecessor_arc->getEnergyCost();

							act = act->predecessor_arc->src;
						}

						if (arcMapList[n1.first][n2.first].size() > 0) {
							arcMapListCost[n1.first][n2.first] = cost;
							arcMapListEnergyCost[n1.first][n2.first] = ecost;
						}
					}
				}
			}
		}

		for (auto& un : usedNode) {
			un->bfs_state = NodeGraph::NOT_VISITED;
			un->distenace_from_root = std::numeric_limits<int>::max();
			un->predecessor_arc = nullptr;
			un->main_path_check = false;
		}

		// PRINT DEBUG
		/*cout << "getMinimumPathToFew - Minimum paths:" << endl;
		for (auto& n1 : arcMapList) {
			for (auto& n2 : n1.second) {
				cout << "getMinimumPathToFew - BFS for node " << n1.first << "-" << n2.first << " = " << endl;
				cout << "  from " << nodeStart->node_t << "-" << nodeStart->node_id <<
						" having time cost: " << arcMapListCost[n1.first][n2.first] <<
						" having energy cost: " << arcMapListEnergyCost[n1.first][n2.first] << endl;
				for (auto a : n2.second) {
					cout << "      " << a->src->node_t << "-" << a->src->node_id << "_" << a->src->time << "--" <<
							a->arc_t << "|" << a->getEnergyCost() << "-->" << a->dest->node_t << "-" << a->dest->node_id << "_" << a->dest->time << " | " << endl;
				}
				cout << endl;
			}
		}*/

	}
}

void FlowGraph::getMinimumPathOnlyFly_GoAndBack(std::list<ArcGraph *> &arcList,
		unsigned int &arcListTimeCost,
		double &arcListEnergyCost,
		Home *homeStart, unsigned int timeStart, DeliveryPoint *dp) {

	//reset
	arcList.clear();
	arcListTimeCost = 0;
	arcListEnergyCost = 0;

	if (	(graphMapMapMap.count(NodeGraph::HOME) > 0) &&
			(graphMapMapMap[NodeGraph::HOME].count(homeStart->getHomeIdNum()) > 0) &&
			(graphMapMapMap[NodeGraph::HOME][homeStart->getHomeIdNum()].count(timeStart) > 0) ) {
		NodeGraph *current = graphMapMapMap[NodeGraph::HOME][homeStart->getHomeIdNum()][timeStart];
		ArcGraph *nextArch;

		// 1^ Fase looking for a direct path toward the DP
		do {
			nextArch = nullptr;
			for(auto& a : current->arcs) {
				//cout << "Arch type: " << a->arc_t << endl << std::flush;
				if (	(a->arc_t == ArcGraph::FLY_WITH_PACKAGE) &&
						(a->dest->node_t == NodeGraph::DELIVERY_POINT) &&
						(a->dest->node_id == dp->getDpIdNum())) {
					nextArch = a;
					break;
				}
				else if (a->arc_t == ArcGraph::STOP) {
					nextArch = a;
				}
			}
			if (nextArch == nullptr) {
				cerr << "Something goes wrong in getMinimumPathOnlyFly_GoAndBack: nextArch for DP not found" << endl;
				exit(EXIT_FAILURE);
			}

			arcList.push_back(nextArch);

			arcListTimeCost += nextArch->dest->time - nextArch->src->time;
			arcListEnergyCost += nextArch->getEnergyCost();

			current = nextArch->dest;

		} while (nextArch->arc_t != ArcGraph::FLY_WITH_PACKAGE);


		// 2^ Fase looking for a fly back home
		do {
			nextArch = nullptr;
			for(auto& a : current->arcs) {
				if (	(a->arc_t == ArcGraph::FLY_EMPTY) &&
						(a->dest->node_t == NodeGraph::HOME) &&
						(a->dest->node_id == homeStart->getHomeIdNum())) {
					nextArch = a;
					break;
				}
				else if (a->arc_t == ArcGraph::STOP) {
					nextArch = a;

				}
			}
			if (nextArch == nullptr) {
				cerr << "Something goes wrong in getMinimumPathOnlyFly_GoAndBack: nextArch for Home not found" << endl;
				exit(EXIT_FAILURE);
			}

			arcList.push_back(nextArch);
			arcListTimeCost += nextArch->dest->time - nextArch->src->time;
			arcListEnergyCost += nextArch->getEnergyCost();

			current = nextArch->dest;

		} while (nextArch->arc_t != ArcGraph::FLY_EMPTY);


		// PRINT DEBUG
		cout << "getMinimumPathOnlyFly_GoAndBack - Minimum path from HOME" << homeStart->getHomeIdNum() << " and DP" << dp->getDpIdNum() << endl;
		cout << "Path time cost: " << arcListTimeCost << " and energy cost: " << arcListEnergyCost << endl;
		for (auto &a : arcList) {
			cout << "      " << a->src->node_t << "-" << a->src->node_id << "_" << a->src->time << "--" <<
					a->arc_t << "|" << a->getEnergyCost() << "-->" << a->dest->node_t << "-" << a->dest->node_id << "_" << a->dest->time << " | " << endl;
		}
		cout << endl;
	}
}

bool FlowGraph::check_pkt_feasibility(double s_lat, double s_lon, Package *p, Battery *b) {
	bool ris = false;
	double requestedEnergy = 0;
	double energy_loss, distance_travel, speed_travel, time_travel;

	//cout << "Checking feasibility to carry the package till the DP" << endl;

	// calculate first part
	energy_loss = sim->getEnergyLossUav(p->weight);
	distance_travel = Simulator::distanceEarth(s_lat, s_lon, p->dest_dp->getDpLatNum(), p->dest_dp->getDpLonNum());
	speed_travel = Uav::getSpeedWithLoad(sim->getUavAvgSpeed(), p->weight);
	time_travel = distance_travel / speed_travel;
	requestedEnergy += energy_loss * time_travel;
	//cout << "Andata: el=" << energy_loss << "W; dist=" << distance_travel << "m; speed=" << speed_travel << "m/s; time="
	//		<< time_travel << "s; reqE=" << requestedEnergy << endl;

	// calculate coming back
	energy_loss = sim->getEnergyLossUav(0);
	distance_travel = Simulator::distanceEarth(p->dest_dp->getDpLatNum(), p->dest_dp->getDpLonNum(), s_lat, s_lon);
	speed_travel = Uav::getSpeedWithLoad(sim->getUavAvgSpeed(), 0);
	time_travel = distance_travel / speed_travel;
	requestedEnergy += energy_loss * time_travel;
	//cout << "Ritorno: el=" << energy_loss << "W; dist=" << distance_travel << "m; speed=" << speed_travel << "m/s; time="
	//		<< time_travel << "s; reqE=" << (energy_loss * time_travel) << endl;
	//cout << "Total req = " << requestedEnergy << "J. Energia disponibile: " << b->getResudualEnergy() << endl << endl;

	if ((b->getResudualEnergy() + requestedEnergy) > (b->getResudualEnergy() * 0.1)) {
		//estimatedEnergyLossOnPackage = requestedEnergy;
		//energyAtPackageLoad = batt->getResudualEnergy();
		ris = true;
	}

	return ris;
}

void FlowGraph::activateUavFlow(unsigned int time, std::list<Uav *> &uavList){
	for (auto& u : uavList){
		NodeGraph *ng = u->getPositionNode();
		Home *waitingHome;
		Battery *bToLoad = nullptr;
		ArcGraph *aToUse_inDP = nullptr;

		switch (u->getState()) {
		case Uav::UAV_FLYING:
			// DO NOTHING... I'M FLYING
			break;

		/*case Uav::UAV_ONBUS:

			break;*/

		case Uav::UAV_WAIT_DP:
			// search for the arc that will send me back to my home
			//cout << "UAV " << u->getId() << " is in UAV_WAIT_DP. Looking for a path to home. Arcs number: " << ng->arcs.size() << endl;
			for (auto a : ng->arcs) {
				if (a->arc_t == ArcGraph::FLY_EMPTY) {
					NodeGraph *dNG = a->dest;
					if ((dNG->node_t == NodeGraph::HOME) && (u->getBelongingHome()->getHomeIdNum() == dNG->node_id)) {
						// I found a direct arc (without payload) toward my home
						aToUse_inDP = a;
						break;
					}
				}
			}
			if (aToUse_inDP != nullptr) {
				aToUse_inDP->uavOnTheArc.push_back(u);
				activeArc.push_back(aToUse_inDP);
				u->setState(Uav::UAV_FLYING);
				u->setActualArch(aToUse_inDP);
				//cout << "UAV " << u->getId() << " Path to home found" << endl;
				//cout << endl << "UAV " << u->getId() << " path found from [DP:" << aToUse_inDP->src->node_id << ":" << aToUse_inDP->src->time <<
				//				"] -> [HOME:" << aToUse_inDP->dest->node_id << ":" << aToUse_inDP->dest->time << "]" << endl;
			}
			else {
				// I didn't find a direct graph (there is not a fly arch for each node).. so I wait here
				for (auto a : ng->arcs) {
					if (a->arc_t == ArcGraph::STOP) {
						a->uavOnTheArc.push_back(u);
						activeArc.push_back(a);
						u->setActualArch(a);
						//cout << "UAV " << u->getId() << " Path to home NOT found. Waiting here" << endl;
						break;
					}
				}
			}
			break;

		case Uav::UAV_WAIT_HOME:
			waitingHome = u->getBelongingHome();
			//cout << "UAV " << u->getId() << " is in UAV_WAIT_HOME" << endl;
			//cout << "UAV " << u->getId() << " bSTATE: " << u->getBatt() << " and pSTATE: " << u->getCarryingPackage() << endl;
			if ((u->getBatt() == nullptr) && (u->getCarryingPackage() == nullptr)) {
				//cout << "UAV " << u->getId() << " has no battery nor a package. Are there available package? " << waitingHome->wa.getWarehousePktNumber() << endl;
				if (waitingHome->wa->getWarehousePktNumber() > 0) {
					Package *pckToCarry = nullptr;
					for (auto& b : waitingHome->bm->batteryList) {
						//for (auto& p : belongingHome->wa.wareHouse) {
						for (auto itP = waitingHome->wa->wareHouse.begin(); itP != waitingHome->wa->wareHouse.end(); itP++) {
							if (check_pkt_feasibility(waitingHome->getHomeLatNum(), waitingHome->getHomeLonNum(), *itP, b)) {
								pckToCarry = *itP;
								waitingHome->wa->wareHouse.erase(itP);
								break;
							}
						}
						if (pckToCarry != nullptr) {
							bToLoad = waitingHome->bm->popBattery(b->id_batt);
							break;
						}
					}
					if ((pckToCarry != nullptr) && (bToLoad != nullptr)) {
						bToLoad->setState(Battery::BATTERY_DISCHARGING_ONUAV);
						u->setBatt(bToLoad);
						u->setCarryingPackage(pckToCarry);
						//cout << "UAV " << u->getId() << " found the Battery"<< bToLoad->id_batt << " and the Package" << pckToCarry->id << endl;
					}
				}
			}

			// did I found/do I have a package to carry?
			if ((u->getBatt() != nullptr) && (u->getCarryingPackage() != nullptr)) {
				Package *actPckToCarry = u->getCarryingPackage();
				//cout << "UAV " << u->getId() << " is in UAV_WAIT_HOME and has a package and battery... looking for a path" << endl;
				ArcGraph *aToUse = nullptr;
				for (auto a : ng->arcs) {
					if (a->arc_t == ArcGraph::FLY_WITH_PACKAGE) {
						NodeGraph *dNG = a->dest;
						if ((dNG->node_t == NodeGraph::DELIVERY_POINT) && (actPckToCarry->dest_dp->getDpIdNum() == dNG->node_id)) {
							// I found a direct arc (with payload) toward the delivery point
							aToUse = a;
							break;
						}
					}
				}
				if (aToUse != nullptr) {
					aToUse->uavOnTheArc.push_back(u);
					activeArc.push_back(aToUse);
					u->setState(Uav::UAV_FLYING);
					u->setActualArch(aToUse);
					//cout << "UAV " << u->getId() << " path found" << endl;
					//cout << endl << "UAV " << u->getId() << " path found from [HOME:" << aToUse->src->node_id << ":" << aToUse->src->time <<
					//		"] -> [DP:" << aToUse->dest->node_id << ":" << aToUse->dest->time << "]" << endl;
				}
				else {
					// I didn't find a direct graph (there is not a fly arch for each node).. so I wait here
					for (auto a : ng->arcs) {
						if (a->arc_t == ArcGraph::STOP) {
							a->uavOnTheArc.push_back(u);
							activeArc.push_back(a);
							u->setActualArch(a);
							//cout << "UAV " << u->getId() << " NO path found... waiting here" << endl;

							// relaase the package and the battery
							waitingHome->bm->addBattery(u->removeBatt());
							waitingHome->wa->addPackage(u->removeCarryingPackage());
							break;
						}
					}
				}
			}

			break;

		default:
			cerr << "Unknown UAV state: " << u->getState() << endl;
			break;
		}
	}

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
	//int cNode = 0;
	//int sOffest = 3;
	//int sSize = 2;

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



