/*
 * BestPoiGraph.cpp
 *
 *  Created on: 27 lug 2017
 *      Author: angelo
 */

#include "BestPoiGraph.h"
#include "Simulator.h"

BestPoiGraph::~BestPoiGraph() {
	// TODO Auto-generated destructor stub
}

void BestPoiGraph::initPoi(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName) {
	backup_list_poi.clear();
	for (auto& poi: poiMap) {
		backup_list_poi.push_back(&(poi.second));
	}
}

int BestPoiGraph::updateUavPathBestPoi(Uav *uav, unsigned int t, bool reserve) {
	std::map<unsigned int, std::list<ArcGraph *> > arcMapList;
	std::map<unsigned int, unsigned int > arcMapListCost;
	std::map<unsigned int, std::list<ArcGraph *> > stopArcMapList;
	std::map<unsigned int, unsigned int > stopArcMapListCost;
	Poi *bestPoi = nullptr;

	int minCost = std::numeric_limits<int>::max();
	unsigned int minStopCostFull;

	std::vector<Poi *> poisToExclude;
	sim->getNeighPoi(uav->getPositionStopId(), poisToExclude);

	for (auto& p : backup_list_poi) {
		//if ((poiToExclude != nullptr) && (p->getPoiIdNum() == poiToExclude->getPoiIdNum())) continue;
		bool excludeThisPoi = false;

		for (auto& p2e : poisToExclude) {
			if (p->getPoiIdNum() == p2e->getPoiIdNum()) {
				excludeThisPoi = true;
				break;
			}
		}

		if (excludeThisPoi) continue;

		std::list<ArcGraph *> newList = std::list<ArcGraph *>();

		std::vector<Stops *> stopsEnd;
		sim->getNeighStop(p->getPoiIdNum(), stopsEnd);

		getMinimumPathToFew(arcMapList, arcMapListCost, uav->getPositionStopId(), t, stopsEnd);

		unsigned int minStopID;
		unsigned int minStopCost = std::numeric_limits<unsigned int>::max();
		for (auto& al : arcMapListCost) {
			if (al.second < minStopCost) {
				minStopID = al.first;
				minStopCost = al.second;
			}
		}
		newList.clear();
		int risCost = 0;
		for (auto itAL = arcMapList[minStopID].begin(); itAL != arcMapList[minStopID].end(); itAL++) {
			//for (auto& al : arcMapList[minStopID]) {
			newList.push_back(*itAL);
			risCost += (*itAL)->dest->time - (*itAL)->src->time;
		}

		stopArcMapList[p->getPoiIdNum()] = newList;
		stopArcMapListCost[p->getPoiIdNum()] = risCost;

		if ((minCost > risCost) && (risCost > 0)) {
			minCost = risCost;
			minStopCostFull = p->getPoiIdNum();
			bestPoi = p;
		}
	}

	if (minCost != std::numeric_limits<int>::max()) {
		uavArcMapList[uav->getId()].clear();

		for (auto itAL = stopArcMapList[minStopCostFull].begin(); itAL != stopArcMapList[minStopCostFull].end(); itAL++) {
			uavArcMapList[uav->getId()].push_back(*itAL);
		}

		if (reserve) {
			for (auto& a : uavArcMapList[uav->getId()]) {
				if (a->arc_t == ArcGraph::BUS) {
					a->reserved = true;
				}
			}
		}

		poiDestMap[uav->getId()] = bestPoi;

		stopArcMapList.clear();
		stopArcMapListCost.clear();

		return minCost;
	}
	else {
		return 0;
	}

}

void BestPoiGraph::setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap) {

	std::vector<Stops *> lStops;
	for (auto& ss : stopsMap) {
		lStops.push_back(&(ss.second));
	}

	FlowGraph::setInitExtraUAV(remainingUAV, time_tm, stopsMap);

	for (auto& uav : remainingUAV) {
		for (int j = 0; j < 50; j++) {
			int ris = updateUavPathBestPoi(uav, day_tm2seconds(time_tm), true);
			if (ris == 0) {
				std::random_shuffle ( lStops.begin(), lStops.end() );
				uav->setPositionStopId(lStops[(std::rand() % lStops.size())]->getStopIdNum());
				setUavPosition(time_tm, uav);
				cout << "Unluky stop, try another one" << endl;
			}
			else {
				break;
			}
		}

		int i = 0;
		cout << "UAV " << uav->getId() << "  ";
		for (auto& a : uavArcMapList[uav->getId()]) {
			cout << a->src->stop_id << "_" << a->src->time <<  " -> " << a->dest->stop_id << "_" << a->dest->time <<  " -- ";
			if (i++ > 7) break;
		}
		cout << endl << endl;
	}

}

void BestPoiGraph::activateUavFlow(unsigned int time, std::list<Uav *> &uavList){
	std::list <Uav *> coveringUAV;
	std::list <Uav *> not_coveringUAV;

	for (auto& u : uavList){
		if (u->getState() == Uav::UAV_FLYING) {
			if (u->isCovering()) {
				coveringUAV.push_back(u);
			}
			else {
				not_coveringUAV.push_back(u);

			}
		}
	}

	while (!not_coveringUAV.empty()) {
		Uav *u = not_coveringUAV.front();
		not_coveringUAV.pop_front();

		NodeGraph *ng = graphMapMap[u->getPositionStopId()][time];

		if (uavArcMapList[u->getId()].size() > 0) {
			ArcGraph *ag = uavArcMapList[u->getId()].front();
			uavArcMapList[u->getId()].pop_front();

			//cout << time << " UAV " << u->getId() << " Percorro il mio percorso verso il POI " << uavTSP[u->getId()]->tspNextHop->dest->poi_id << endl;
			//cout << time << " UAV " << u->getId() << " eseguo l'arco " << ag->src->stop_id << "_" << ag->src->time << " -> " << ag->dest->stop_id << "_" << ag->dest->time << endl;

			if ((ag->src->stop_id != ng->stop_id) || (ag->src->time != ng->time)) {
				cerr << time << " UAV " << u->getId() << " Non sono sul percorso corretto???? perché???" << endl;
				cerr << time << " UAV " << u->getId() << " Matrice: " << ng->stop_id << "_" << ng->time << "; arco: " << ag->src->stop_id << "_" << ag->src->time << endl;
				cerr << time << " UAV " << u->getId() << " Io, secondo me, sono sullo stop: " << u->getPositionStopId() << endl;
				exit(EXIT_FAILURE);
			}
			else {
				switch (ag->arc_t) {
				case ArcGraph::STOP:
					u->setState(Uav::UAV_FLYING);
					break;

				case ArcGraph::BUS:
					//cout << time << " UAV " << u->getId() << " takes the bus from stop: " << ag->src->stop_id << "_" << ag->src->time << " to stop: " << ag->dest->stop_id << "_" << ag->dest->time << endl;
					u->setState(Uav::UAV_ONBUS);
					break;

				default:
					cerr << "Arco COVERAGE???? perché???" << endl;
					exit(EXIT_FAILURE);
					break;
				}

				ag->uavOnTheArc.push_back(u);
				activeArc.push_back(ag);
			}
		}
		else {

			// I've finished the path... I catch the POI
			//cout << "Sono arrivato al POI " << uavTSP[u->getId()]->tspNextHop->dest->poi_id << endl;

			cout << time << " UAV " << u->getId() << " arrived at POI " << poiDestMap[u->getId()]->getPoiIdNum() <<
					" on stop " << u->getPositionStopId() << endl;

			for (auto a : ng->arcs) {
				if ( (a->arc_t == ArcGraph::COVER) && (a->p != nullptr) && (a->p->getPoiIdNum() == poiDestMap[u->getId()]->getPoiIdNum()) ) {
					Poi *pp = poiDestMap[u->getId()];

					swapCount++;


					u->setCoveringPoiId(pp->getPoiIdNum());

					//swappo col nodo che stava sul POI
					Uav *uavSwap = pp->getCoveringUav();
					pp->setCoveringUav(u);
					coveringUAV.push_back(u);

					cout << time << " UAV " << u->getId() << " swapping with UAV " << uavSwap->getId() <<
							" that now is going going from POI " << pp->getPoiIdNum() << " -> BOH!" << endl;

					std::vector<Stops *> neighPoiStops;
					sim->getNeighStop(uavSwap->getCoveringPoiId(), neighPoiStops);

					uavSwap->unsetCovering();

					cout << "Looking for the new path of UAV " << uavSwap->getId() << endl; fflush(stdout);

					int risSearch = 0;
					for (int jj = 0; jj < 10; jj++) {
						risSearch = updateUavPathBestPoi(uavSwap, time, false);

						if (risSearch == 0) {
							std::random_shuffle(neighPoiStops.begin(), neighPoiStops.end());
							uavSwap->setPositionStopId(neighPoiStops[(std::rand() % neighPoiStops.size())]->getStopIdNum());
							setUavPosition(day_seconds2tm(time), uavSwap);
							cout << "Unluky stop, try another one" << endl;
						}
						else {
							for (auto& a : uavArcMapList[uavSwap->getId()]) {
								if (a->arc_t == ArcGraph::BUS) {
									a->reserved = true;
								}
							}
							break;
						}
					}

					if (risSearch == 0) {
						cerr << time << " UAV " << uavSwap->getId() << " The UAV is blocked!!!! Killing him..." << endl; fflush(stderr);
						//exit (EXIT_FAILURE);
						//kill the UAV--- setting all stops arcs
						unsigned int act_time = time+1;
						unsigned int act_stop = uavSwap->getPositionStopId();
						NodeGraph *act_ng = graphMapMap[act_stop][act_time];
						uavArcMapList[uavSwap->getId()].clear();
						while (!act_ng->arcs.empty()) {
							for (auto& a : act_ng->arcs) {
								if (a->arc_t == ArcGraph::STOP) {
									uavArcMapList[uavSwap->getId()].push_back(a);
									break;
								}
							}

							act_time++;
							act_ng = graphMapMap[act_stop][act_time];
						}
					}
					else {
						cout << "Found for the new path of UAV " << uavSwap->getId() << endl; fflush(stdout);
						auto itU = coveringUAV.begin();
						while (itU != coveringUAV.end()) {
							Uav *actU = *itU;
							if (actU->getId() == uavSwap->getId()) {
								coveringUAV.erase(itU);
								break;
							}
							itU++;
						}
						if (itU != coveringUAV.end()) {
							not_coveringUAV.push_back(uavSwap);
						}
						else {
							cerr << "UAV" << uavSwap->getId() << " Errore nello swapping nel POI: " << pp->getPoiIdNum() <<
									" - swapping the UAV " << u->getId() << " with UAV " << uavSwap->getId() << endl;
							exit(EXIT_FAILURE);
						}

						cout << endl;

						break;
					}
				}
			}
		}

	}

	// per gli uav coprenti non devo fare molto... solo farli restare sugli archi COVER
	while (!coveringUAV.empty()) {
		Uav *u = coveringUAV.front();
		coveringUAV.pop_front();

		NodeGraph *ng = graphMapMap[u->getPositionStopId()][time];

		for (auto a : ng->arcs) {
			if ( (a->arc_t == ArcGraph::COVER) && (a->p != nullptr) && (a->p->getPoiIdNum() == u->getCoveringPoiId()) ) {
				a->uavOnTheArc.push_back(u);
				activeArc.push_back(a);

				break;
			}
		}
	}

	//cout << "End activate flow" << endl; fflush(stdout);
}
