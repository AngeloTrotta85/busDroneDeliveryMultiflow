/*
 * GreedyFlowGraph.cpp
 *
 *  Created on: 18 dic 2017
 *      Author: angelo
 */

#include "GreedyFlowGraph.h"


GreedyFlowGraph::GreedyFlowGraph(Simulator *s) : FlowGraph(s) {

}

GreedyFlowGraph::~GreedyFlowGraph() {
	// TODO Auto-generated destructor stub
}


void GreedyFlowGraph::generateUavPath(unsigned int time, Uav *u){
	NodeGraph *ng = u->getPositionNode();

	if (u->getState() == Uav::UAV_WAIT_HOME) { // check for the new path only for the UAV waiting at HOME
		Home *waitingHome = u->getBelongingHome();

		if (waitingHome->wa->getWarehousePktNumber() > 0) {
			std::list<ArcGraph *> arcList;
			//unsigned int arcListTimeCost = 0;
			double arcListEnergyCost = 0;

			for (auto itP = waitingHome->wa->wareHouse.begin(); itP != waitingHome->wa->wareHouse.end(); itP++) {
				Package *pckToCarry = *itP;
				Battery *bToLoad = nullptr;

				std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, std::list<ArcGraph *> > > arcMapListGo, arcMapListBack;
				std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, unsigned int > > arcMapListCostGo, arcMapListCostBack;
				std::map<NodeGraph::NODE_TYPE, std::map<unsigned int, double > > arcMapListEnergyCostGo, arcMapListEnergyCostBack;
				std::vector<NodeGraph *> nodesEndGo, nodesEndBack;

				nodesEndGo.push_back(graphMapMapMap[NodeGraph::DELIVERY_POINT][pckToCarry->dest_dp->getDpIdNum()].begin()->second);
				nodesEndBack.push_back(ng);

				cout << endl << "Calculating minimum path for the UAV" << u->getId() << endl;
				cout << "Generating path from HOME" << ng->node_id << " to DeliveryPoint" << pckToCarry->dest_dp->getDpIdNum() << endl;

				getMinimumPathToFew_withEnergy(arcMapListGo, arcMapListCostGo, arcMapListEnergyCostGo, ng, nodesEndGo, false, true);
				exit(EXIT_SUCCESS);
				if (arcMapListGo[NodeGraph::DELIVERY_POINT][pckToCarry->dest_dp->getDpIdNum()].size() > 0) {
					NodeGraph *ngDParrived = arcMapListGo[NodeGraph::DELIVERY_POINT][pckToCarry->dest_dp->getDpIdNum()].back()->dest;
					getMinimumPathToFew_withEnergy(arcMapListBack, arcMapListCostBack, arcMapListEnergyCostBack, ngDParrived, nodesEndBack, true, false);
					if (arcMapListBack[ng->node_t][ng->node_id].size() > 0) {

						arcListEnergyCost = arcMapListEnergyCostGo[NodeGraph::DELIVERY_POINT][pckToCarry->dest_dp->getDpIdNum()] +
								arcMapListEnergyCostBack[ng->node_t][ng->node_id];

						// check if there is a battery with enough battery to carry this package
						for (auto& b : waitingHome->bm->batteryList) {
							//cout << endl << "UAV" << u->getId() << " check battery. Path cost: " << arcListEnergyCost << " - battery available: " << b->getResudualEnergy() << endl << std::flush;
							if ((b->getResudualEnergy() + arcListEnergyCost) > 0) {
								//TODO set: arcList, arcListTimeCost, arcListEnergyCost;
								//arcListTimeCost = arcMapListCostGo[NodeGraph::DELIVERY_POINT][pckToCarry->dest_dp->getDpIdNum()] +
								//		arcMapListCostBack[ng->node_t][ng->node_id];

								for (auto& itGO : arcMapListGo[NodeGraph::DELIVERY_POINT][pckToCarry->dest_dp->getDpIdNum()]) {
									arcList.push_back(itGO);
								}
								for (auto& itBACK : arcMapListBack[ng->node_t][ng->node_id]) {
									arcList.push_back(itBACK);
								}


								waitingHome->wa->wareHouse.erase(itP);

								bToLoad = waitingHome->bm->popBattery(b->id_batt);
								bToLoad->setState(Battery::BATTERY_DISCHARGING_ONUAV);

								u->setBatt(bToLoad);
								u->setCarryingPackage(pckToCarry);

								break;
							}
						}
					}
				}

				cout  << "End calculating minimum path for the UAV" << u->getId() << ", generating path from HOME" << ng->node_id << " to DeliveryPoint" << pckToCarry->dest_dp->getDpIdNum() << endl;

				//getMinimumPathOnlyFly_GoAndBack(arcList, arcListTimeCost, arcListEnergyCost, waitingHome, time, pckToCarry->dest_dp);

				if (arcList.size() > 0) {
					// check if there is a battery with enough battery to carry this package
					//for (auto& b : waitingHome->bm->batteryList) {


					//if (b->getResudualEnergy() > arcListEnergyCost) {
					//if ((b->getResudualEnergy() + arcListEnergyCost) > 0) {

					// set the path
					for (auto itAL = arcList.begin(); itAL != arcList.end(); itAL++) {
						uavArcMapList[u->getId()].push_back(*itAL);
					}

					//cout << "Path found for UAV" << u->getId() << endl << std::flush;

					//break;
					//}
					//}
				}
				else {
					//UNDELIVERABLE PACKAGE
					//waitingHome->wa->undeliverablePackage(pckToCarry);
					itP = waitingHome->wa->wareHouse.erase(itP);
				}

				if (bToLoad != nullptr) {
					break;
				}
			}
			//arcList.clear();
		}

		if (uavArcMapList[u->getId()].size() == 0) {	// no package available or battery to carry it. Stay at home
			//cout << "No Package to carry, stay at home" << endl << std::flush;
			for (auto a : ng->arcs) {
				if (a->arc_t == ArcGraph::STOP) {
					uavArcMapList[u->getId()].push_back(a);
					break;
				}
			}
		}
		else {
			// PRINT DEBUG
			cout << "FINAL PATH for UAV" << u->getId() << endl;
			for (auto a : uavArcMapList[u->getId()]) {
				std::string lt;
				switch (a->arc_t) {
				case ArcGraph::BUS:
					lt = std::string("BUS");
					break;
				case ArcGraph::FLY_EMPTY:
					lt = std::string("FLY_EMPTY");
					break;
				case ArcGraph::FLY_WITH_PACKAGE:
					lt = std::string("FLY_PACK");
					break;
				case ArcGraph::STOP:
					lt = std::string("STOP");
					break;
				default:
					lt = std::string("UNKNOWN");
					break;
				}
				cout << "      " << a->src->node_t << "-" << a->src->node_id << "_" << a->src->time << "--" <<
						lt << "|T" << (a->dest->time - a->src->time) << "|E" << a->getEnergyCost() << "-->" << a->dest->node_t << "-" << a->dest->node_id << "_" << a->dest->time << " | " << endl;
			}
			cout << endl;
			cout << "FINAL PATH - FINISH" << endl;
		}
	}
	else {
		cerr << "Error: UAV" << u->getId() << " has no calculated path but it is not at HOME (act state " << u->getState() << ")" << endl;
		exit (EXIT_FAILURE);
	}
}
