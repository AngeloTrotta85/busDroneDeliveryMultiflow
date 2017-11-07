/*
 * TspGraph.cpp
 *
 *  Created on: 24 lug 2017
 *      Author: angelo
 */

#include "TspGraph.h"
#include "Simulator.h"


void TspGraph::initPoi(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName) {
	cout << "GENERATING POI MAP" << endl; fflush(stdout);
	generatePoiGraph(poiMap, tpsFileName);

	cout << "START MAKING THE TSP" << endl; fflush(stdout);
	calculateTSP();

	if (sim->isToCluster()) {
		cout << "START MAKING THE MST" << endl; fflush(stdout);
		calculateMST();

		cout << "START MAKING THE CLUSTERS" << endl; fflush(stdout);
		makeKruskalClusters();
	}

}

void TspGraph::generatePoiGraph(std::map<unsigned long int, Poi> &poiMap, std::string tpsFileName) {
	std::ifstream filePoiTSP(tpsFileName, std::ifstream::in);
	if(filePoiTSP.is_open()) {
		cout << "Loading the TSP from file" << endl;
		std::string str;
		while (std::getline(filePoiTSP, str)) {
			unsigned int poiID1, poiID2, w;
			if (str.compare(0,2,"P ") == 0) {
				sscanf(str.c_str(), "P %u\n", &poiID1);

				TspNodeGraph *newP = new TspNodeGraph(poiID1);
				if (poiMap.count(poiID1) == 0) {
					cerr << "Error importing POI " << poiID1 << " from file" << endl;
					exit(EXIT_FAILURE);
				}
				newP->poiPtr = &(poiMap[poiID1]);
				sim->getNeighStop(poiID1, newP->neighStops);
				tspGraphMap[newP->poi_id] = newP;
			}
			else if (str.compare(0,2,"L ") == 0) {
				sscanf(str.c_str(), "L %u;%u;%u\n", &poiID1, &poiID2, &w);

				TspArcGraph *newA_p1_p2 = new TspArcGraph();
				newA_p1_p2->src = tspGraphMap[poiID1];
				newA_p1_p2->dest = tspGraphMap[poiID2];
				newA_p1_p2->weight = w;

				tspGraphMap[poiID1]->arcs.push_back(newA_p1_p2);
			}
		}
		filePoiTSP.close();

		for (auto& p : tspGraphMap) {
			cout << "Stops for POI " << p.second->poi_id << ": ";;
			for (auto& s : p.second->neighStops) {
				cout << s->getStopIdNum() << " ";
			}
			cout << endl;
		}
	}
	else {

		std::map<unsigned int, std::list<ArcGraph *> > arcMapList;
		std::map<unsigned int, unsigned int > arcMapListCost;
		std::vector<unsigned int> timeVec;
		int attempt = 20;
		double p1counter, p2counter, s1counter, s2counter, tcounter;

		cout << "Calculating the TSP. I got " << poiMap.size() << " POI." << endl;

		// init timeVec to randmize the tryes
		for (auto& t : graphMapMap.begin()->second ) {
			timeVec.push_back(t.second->time);
		}
		if (attempt > (int)graphMapMap.begin()->second.size()) {
			attempt = graphMapMap.begin()->second.size();
		}

		// generate the points
		for (auto& p : poiMap) {
			TspNodeGraph *newP = new TspNodeGraph(p.second.getPoiIdNum());
			newP->poiPtr = &(p.second);
			sim->getNeighStop(p.second.getPoiIdNum(), newP->neighStops);
			tspGraphMap[newP->poi_id] = newP;

			cout << "Poi " << newP->poi_id << ". Neigh stops: ";
			for (auto ss : newP->neighStops) {
				cout << ss->getStopIdNum() << " ";
			}
			cout << endl;
		}

		// generate the arcs
		p1counter = 0;
		for (auto& p1 : tspGraphMap) {
			p1counter += 1;

			p2counter = 0;
			for (auto& p2 : tspGraphMap) {
				p2counter += 1;
				if (p1.first != p2.first) {
					double sumW = 0;
					double countW = 0;
					TspArcGraph *newA_p1_p2 = new TspArcGraph();
					newA_p1_p2->src = p1.second;
					newA_p1_p2->dest = p2.second;

					tcounter = 0;
					std::random_shuffle(timeVec.begin(), timeVec.end());
					for (int i = 0; i < attempt; i++) {
						unsigned int timeAct = timeVec[i];
						//for (auto& t : graphMapMap.begin()->second ) {
						//unsigned int timeAct = t.second->time;

						tcounter += 1;

						s1counter = 0;
						for (auto& s1 : p1.second->neighStops){
							s1counter += 1;
							//getMinimumPathAll(arcMapList, s1->getStopIdNum(), timeAct);
							getMinimumPathToFew(arcMapList, arcMapListCost, s1->getStopIdNum(), timeAct, p2.second->neighStops);

							s2counter = 0;
							for (auto& s2 : p2.second->neighStops){
								s2counter += 1;

								if (arcMapList.count(s2->getStopIdNum()) > 0) {
									double wAct = 0;
									/*std::list<ArcGraph *> arcList;

								for (auto it = arcMapList[s2->getStopIdNum()].begin(); it != arcMapList[s2->getStopIdNum()].end(); it++) {
									arcList.push_back(*it);
								}

								for (auto& ar : arcList) {
									wAct += ar->dest->time - ar->src->time;
								}*/

									for (auto& ar : arcMapList[s2->getStopIdNum()]) {
										wAct += ar->dest->time - ar->src->time;
									}

									if (wAct > 0) {
										sumW += wAct;
										countW += 1.0;

										//cout << "  Path found with cost: " << wAct << "        ";
									}
								}

								fprintf(stdout, "\rPOI1: %.02f%% [%.0f/%d]; POI2: %.02f%% [%.0f/%d]; Time: %.05f%% [%.0f/%d]; STOP1: %.02f%% [%.0f/%d]; STOP2: %.02f%% [%.0f/%d]        ",
										(p1counter / ((double) tspGraphMap.size())) * 100.0, p1counter, (int)tspGraphMap.size(),
										(p2counter / ((double) tspGraphMap.size())) * 100.0, p2counter, (int)tspGraphMap.size(),
										//tcounter / ((double) graphMapMap.begin()->second.size()),
										(tcounter / ((double) attempt)) * 100.0, tcounter, attempt,
										(s1counter / ((double) p1.second->neighStops.size())) * 100.0, s1counter, (int)p1.second->neighStops.size(),
										(s2counter / ((double) p2.second->neighStops.size())) * 100.0, s2counter, (int)p2.second->neighStops.size()    );fflush(stdout);
							}
						}
					}

					if (countW > 0) {
						newA_p1_p2->weight = sumW / countW;
					}
					else {
						newA_p1_p2->weight = std::numeric_limits<unsigned int>::max();
					}
					p1.second->arcs.push_back(newA_p1_p2);
				}
			}
		}
		cout << endl;

		//export the TSP graph
		if (!tpsFileName.empty()) {
			std::ofstream tpsStream(tpsFileName, std::ofstream::out);
			if (tpsStream.is_open()) {
				for (auto& tn : tspGraphMap) {
					tpsStream << "P " << tn.second->poi_id << endl;
				}
				for (auto& tn : tspGraphMap) {
					for (auto& ta : tn.second->arcs) {
						tpsStream << "L " << tn.second->poi_id << ";" << ta->dest->poi_id << ";" << ta->weight  << endl;
					}
				}
				tpsStream.close();
			}
		}
	}

	// DEBUG
	for (auto& tn : tspGraphMap) {
		cout << "Links for POI: " << tn.second->poi_id << " -> ";
		for (auto& ta : tn.second->arcs) {
			cout << "[" << ta->dest->poi_id << "|" << ta->weight << "] ";
		}
		cout << endl;
	}
	//exit(EXIT_SUCCESS);
}

void TspGraph::calculateMST(void) {
	std::list<TspNodeGraph *> reachedNodes;
	mstRootNode = tspGraphMap.begin()->second;

	for (auto& n : tspGraphMap) {
		n.second->mstReached = false;
	}

	mstRootNode->mstReached = true;
	reachedNodes.push_back(mstRootNode);

	while (reachedNodes.size() < tspGraphMap.size()) {
		TspArcGraph *bestArc = nullptr;

		for (auto& n : reachedNodes) {
			for (auto& a : n->arcs) {
				if (	(!(a->dest->mstReached)) &&
						((bestArc == nullptr) || (a->weight < bestArc->weight))
						) {
					bestArc = a;
				}
			}
		}

		if (bestArc != nullptr) {
			bestArc->src->mstChild.push_back(bestArc);
			bestArc->dest->mstReached = true;
			reachedNodes.push_back(bestArc->dest);
		}
		else {
			cerr << "Errore nel calcolo dell'MST" << endl;
			exit(EXIT_FAILURE);
		}
	}

	// DEBUG PRINT
	for (auto& n : tspGraphMap) {
		cout << "MST for node: " << n.second->poi_id << " -> ";
		for (auto& a : n.second->mstChild) {
			cout << a->dest->poi_id << "[" << a->weight << "] ";
		}
		cout << endl;
	}
}

void TspGraph::makeKruskalClusters(void) {
	// copy the original TSP on the level 0
	for (auto& n : tspGraphMap) {
		n.second->kruskal_tspNextHop.resize(1);
		n.second->kruskal_tspNextHop[0] = n.second->tspNextHop;
		n.second->kruskal_tspPreviousHop.resize(1);
		n.second->kruskal_tspPreviousHop[0] = n.second->tspPreviousHop;
		n.second->kruskal_tspID.resize(1);
		n.second->kruskal_tspID[0] = 0;
		n.second->kruskal_mstChild.resize(1);
		for (auto& a : n.second->mstChild) {
			n.second->kruskal_mstChild[0].push_back(a);
		}

		clusterIndexUav.resize(1);
		clusterIndexUav[0].resize(1);
		clusterIndexUav[0][0] = sim->getUav() - tspGraphMap.size();
	}
	kruskal_mstRootNode.resize(1);
	kruskal_mstRootNode[0].push_back(mstRootNode);

	int actClusteringLevel = 0;
	int oldClusteringLevel = 0;
	bestClusteringLayer = 0; // no clusters
	double ri_old = calculateRI(0);
	cout << "RI at level 0: " << ri_old << endl;



	// TODO make it iteratively... now is only one clustering level

	if((sim->getUav() - sim->getNPoi()) >= 2) {

		// make another level
		actClusteringLevel++;

		//duplica il tutto
		for (auto& n : tspGraphMap) {
			n.second->kruskal_tspNextHop.resize(actClusteringLevel+1);
			n.second->kruskal_tspNextHop[actClusteringLevel] = nullptr;
			n.second->kruskal_tspPreviousHop.resize(actClusteringLevel+1);
			n.second->kruskal_tspPreviousHop[actClusteringLevel] = nullptr;
			n.second->kruskal_tspID.resize(actClusteringLevel+1);
			n.second->kruskal_tspID[actClusteringLevel] = n.second->kruskal_tspID[oldClusteringLevel];
			n.second->kruskal_mstChild.resize(actClusteringLevel+1);
			for (auto& a : n.second->kruskal_mstChild[oldClusteringLevel]) {
				n.second->kruskal_mstChild[actClusteringLevel].push_back(a);
			}

			clusterIndexUav.resize(actClusteringLevel+1);
			clusterIndexUav[actClusteringLevel].resize(actClusteringLevel+1);
			for (unsigned int i = 0; i < clusterIndexUav[oldClusteringLevel].size(); i++) {
				clusterIndexUav[actClusteringLevel][i] = clusterIndexUav[oldClusteringLevel][i];
			}
			clusterIndexUav[actClusteringLevel][actClusteringLevel] = 0;
		}
		kruskal_mstRootNode.resize(actClusteringLevel+1);
		for (auto& roo : kruskal_mstRootNode[oldClusteringLevel]) {
			kruskal_mstRootNode[actClusteringLevel].push_back(roo);
		}

		// search for the worst arc in the mst
		unsigned int worstArcW = 0;
		TspArcGraph *worstArc = nullptr;
		std::list<TspNodeGraph *> qVisit;
		for (auto& r : kruskal_mstRootNode[actClusteringLevel]) {
			qVisit.push_back(r);
		}
		while (!qVisit.empty()) {
			TspNodeGraph *actNode = qVisit.front();
			qVisit.pop_front();

			for (auto& a : actNode->kruskal_mstChild[actClusteringLevel]) {
				cout << "Arch " << a->src->poi_id << " -|" << a->weight << "|-> " << a->dest->poi_id << endl;
				qVisit.push_back(a->dest);

				if (worstArcW < a->weight) {
					//if (733 == a->weight) { //TODO REMOVE!!!!

					// check if the arc makes 1-node-cluter
					// non fare solo se sono una foglia o il root
					TspNodeGraph *srcIfRoot = nullptr;
					for (auto& r : kruskal_mstRootNode[actClusteringLevel]) {
						if (a->src->poi_id == r->poi_id) {
							srcIfRoot = r;
							break;
						}
					}
					if  (	(a->dest->kruskal_mstChild[actClusteringLevel].size() > 0) &&
							((srcIfRoot == nullptr) || (srcIfRoot->kruskal_mstChild[actClusteringLevel].size() > 1)) ) {

						worstArcW = a->weight;
						worstArc = a;
					}
				}
			}
		}

		if (worstArc == nullptr) {
			cerr << "Errore nel calcolo di Kruskal... Addio!" << endl;
			exit (EXIT_FAILURE);
		}

		//cut the tree and update the roots
		for (auto itA = worstArc->src->kruskal_mstChild[actClusteringLevel].begin(); itA != worstArc->src->kruskal_mstChild[actClusteringLevel].end(); itA++){
			//for (auto& a : worstArc->src->kruskal_mstChild[actClusteringLevel]) {
			TspArcGraph *a = *itA;
			if (a->dest->poi_id == worstArc->dest->poi_id) {
				worstArc->src->kruskal_mstChild[actClusteringLevel].erase(itA);
				break;
			}
		}
		kruskal_mstRootNode[actClusteringLevel].push_back(worstArc->dest);

		//reset the clusters IDs
		int actIDX = 0;
		for (auto& root : kruskal_mstRootNode[actClusteringLevel]) {
			std::list<TspNodeGraph *> qVisit;
			qVisit.push_back(root);

			while (!qVisit.empty()) {
				TspNodeGraph *actNode = qVisit.front();
				qVisit.pop_front();

				actNode->kruskal_tspID[actClusteringLevel] = actIDX;

				for (auto& a : actNode->kruskal_mstChild[actClusteringLevel]) {
					qVisit.push_back(a->dest);
				}
			}
			actIDX++;
		}

		//calculate the TSP for each subtree
		for (auto& root : kruskal_mstRootNode[actClusteringLevel]) {
			//cout << "Calculating TSP on Kruscal of level " << actClusteringLevel << " starting from root: " << root->poi_id << endl;
			calculateTSP_kruskal(root, actClusteringLevel);
		}

		//rearrange the UAVs
		rearrangeUavAfterClustering(actClusteringLevel, sim->getUav() - tspGraphMap.size(), kruskal_mstRootNode[actClusteringLevel]);
		double ri_new = calculateRI(actClusteringLevel);
		cout << "RI at level " << actClusteringLevel << ": " << ri_new << endl;

		//DEBUG
		for (unsigned int i = 0; i < clusterIndexUav.size(); i++) {
			cout << "Roots at kruskal level " << i << ": ";
			for (auto& r : kruskal_mstRootNode[i]) {
				cout << r->poi_id << " ";
			}
			cout << endl;
			for (auto& n : tspGraphMap) {
				cout << "MST for node: " << n.second->poi_id << " at kruskal level " << i << " -> ";
				for (auto& a : n.second->kruskal_mstChild[i]) {
					cout << a->dest->poi_id << "[" << a->weight << "] ";
				}
				cout << endl;
			}
		}

		if (true){
		//if (ri_new < ri_old) {
			cout << "Good news! Kruskal says that is better to cluster!" << endl;
			bestClusteringLayer = actClusteringLevel;

			for (auto& n : tspGraphMap) {
				n.second->tspNextHop = n.second->kruskal_tspNextHop[bestClusteringLayer];
				n.second->tspPreviousHop = n.second->kruskal_tspPreviousHop[bestClusteringLayer];
			}

			for (auto& r : kruskal_mstRootNode[bestClusteringLayer]) {
				cout << "TSP Nearest Neighbor: " << r->poi_id << " -|" << r->kruskal_tspNextHop[bestClusteringLayer]->weight << "|-> " << r->kruskal_tspNextHop[bestClusteringLayer]->dest->poi_id;
				TspNodeGraph *actNode = r->kruskal_tspNextHop[bestClusteringLayer]->dest;
				while (actNode->poi_id != r->poi_id) {
					cout << " -|" << actNode->kruskal_tspNextHop[bestClusteringLayer]->weight << "|-> " << actNode->kruskal_tspNextHop[bestClusteringLayer]->dest->poi_id;
					actNode = actNode->kruskal_tspNextHop[bestClusteringLayer]->dest;
				}
				cout << " === Total cost: " << calculateTSPcost_kruskal(r, bestClusteringLayer) << endl;

				cout << "TSP BASIC_PTR Nearest Neighbor: " << r->poi_id << " -|" << r->tspNextHop->weight << "|-> " << r->tspNextHop->dest->poi_id;
				actNode = r->tspNextHop->dest;
				while (actNode->poi_id != r->poi_id) {
					cout << " -|" << actNode->tspNextHop->weight << "|-> " << actNode->tspNextHop->dest->poi_id;
					actNode = actNode->tspNextHop->dest;
				}
				cout << " === Total cost: " << calculateTSPcost(tspGraphMap) << endl;



				cout << "TSP Nearest Neighbor (BACK): " << r->poi_id << " -|" << r->kruskal_tspPreviousHop[bestClusteringLayer]->weight << "|-> " << r->kruskal_tspPreviousHop[bestClusteringLayer]->dest->poi_id;
				actNode = r->kruskal_tspPreviousHop[bestClusteringLayer]->dest;
				while (actNode->poi_id != r->poi_id) {
					cout << " -|" << actNode->kruskal_tspPreviousHop[bestClusteringLayer]->weight << "|-> " << actNode->kruskal_tspPreviousHop[bestClusteringLayer]->dest->poi_id;
					actNode = actNode->kruskal_tspPreviousHop[bestClusteringLayer]->dest;
				}
				cout << " === Total cost: " << calculateTSPcost_kruskal_backward(r, bestClusteringLayer) << endl;

				cout << "TSP BASIC_PTR Nearest Neighbor (BACK): " << r->poi_id << " -|" << r->tspPreviousHop->weight << "|-> " << r->tspPreviousHop->dest->poi_id;
				actNode = r->kruskal_tspPreviousHop[bestClusteringLayer]->dest;
				while (actNode->poi_id != r->poi_id) {
					cout << " -|" << actNode->tspPreviousHop->weight << "|-> " << actNode->tspPreviousHop->dest->poi_id;
					actNode = actNode->tspPreviousHop->dest;
				}
				cout << " === Total cost: " << calculateTSPcost_backward(tspGraphMap) << endl;
			}

			/*auto itUAV = uav_extra_backup.begin();
		unsigned int countCost = 0;

		//for (int i = 0; i < kruskal_mstRootNode[bestClusteringLayer].size(); i++) {
		for (auto& r : kruskal_mstRootNode[bestClusteringLayer]) {
			int availUAV = clusterIndexUav[bestClusteringLayer][r->kruskal_tspID[bestClusteringLayer]];

			TspNodeGraph *actNode = r;
			unsigned int nextLimit = actNode->kruskal_tspNextHop[bestClusteringLayer]->weight;

			cout << "Per il cluster " << r->kruskal_tspID[bestClusteringLayer] << " ho a disposizione" <<
					availUAV << " UAV su un totale di " << uav_extra_backup.size() << endl;
		}*/
		}
		else {
			cout << "No more clustering. The best kruscal layer is: " << (actClusteringLevel - 1) << endl;
		}

	}
	else {
		cout << "No clustering. Not enought UAVs: " << (sim->getUav() - sim->getNPoi()) << endl;
	}

}

void TspGraph::rearrangeUavAfterClustering(unsigned int kruskal_level, int availableUAV, std::list <TspNodeGraph *> &rootList) {
	int residualUAV = availableUAV;
	std::vector<double> clusters_fullW(kruskal_level+1, 0);
	/*for (unsigned int i = 0; i < clusterIndexUav[kruskal_level].size(); i++) {
		clusterIndexUav[kruskal_level][i] = 1;
		residualUAV--;
	}*/

	cout << "Rearranging the UAVs after the clustering. I have " << availableUAV << " UAVs" << endl;

	for (auto& r : rootList) {
		clusters_fullW[r->kruskal_tspID[kruskal_level]] = calculateTSPcost_kruskal(r, kruskal_level);

		cout << "TSP cost of ID " << r->kruskal_tspID[kruskal_level] << " is: " << clusters_fullW[r->kruskal_tspID[kruskal_level]] << endl;

		clusterIndexUav[kruskal_level][r->kruskal_tspID[kruskal_level]] = 1;
		residualUAV--;
	}

	while (residualUAV > 0) {
		double maxRI = 0;
		int maxIDX;
		for (unsigned int i = 0; i <= kruskal_level; i++) {
			double clusterRI = clusters_fullW[i] / clusterIndexUav[kruskal_level][i];

			if (maxRI < clusterRI) {
				maxRI = clusterRI;
				maxIDX = i;
			}
		}

		clusterIndexUav[kruskal_level][maxIDX] += 1;

		residualUAV--;
	}

	//DEBUG
	cout << "Rearranged the UAVs after the clustering:" << endl;
	for (unsigned int i = 0; i <= kruskal_level; i++) {
		cout << "Cluster " << i << " received " << clusterIndexUav[kruskal_level][i] << " UAVs. The TPS cost is: " <<
				clusters_fullW[i] << endl;
	}
}

double TspGraph::calculateRI(int clusteringLevel) {
	std::vector<double> clusters_ri(clusteringLevel+1, 0);
	std::vector<double> clusters_sum(clusteringLevel+1, 0);
	//std::map<unsigned int, bool> checkedPoi;
	//double nRemainingUav = sim->getUav() - tspGraphMap.size();
	double maxRI = 0;

	for (auto& p : tspGraphMap) {
		clusters_sum[p.second->kruskal_tspID[clusteringLevel]] += p.second->kruskal_tspNextHop[clusteringLevel]->weight;
	}

	for (unsigned int i = 0; i < clusters_ri.size(); i++) {
		clusters_ri[i] = clusters_sum[i] / clusterIndexUav[clusteringLevel][i];

		cout << "RI at level " << clusteringLevel << " for cluster " << i << " is: " << clusters_ri[i] << endl;

		if (maxRI < clusters_ri[i]) {
			maxRI = clusters_ri[i];
		}
	}

	return maxRI;
}

unsigned int TspGraph::calculateTSPcost(std::map<unsigned int, TspNodeGraph *> &tsp) {
	unsigned int cost = 0;
	for (auto& n : tsp) {
		cost += n.second->tspNextHop->weight;
	}
	return cost;
}

unsigned int TspGraph::calculateTSPcost_backward(std::map<unsigned int, TspNodeGraph *> &tsp) {
	unsigned int cost = 0;
	for (auto& n : tsp) {
		cost += n.second->tspPreviousHop->weight;
	}
	return cost;
}

void TspGraph::calculateTSP(void) {

	cout << "STARTING CALCULATING THE TSP: Number of nodes: " << tspGraphMap.size() << endl;fflush(stdout);
	TspNodeGraph *firstNode = tspGraphMap.begin()->second;
	TspNodeGraph *actNode = firstNode;
	bool found;

	do {
		found = false;

		actNode->arcs.sort(sortArcs);
		for (auto itA = actNode->arcs.begin(); itA != actNode->arcs.end(); itA++) {
			TspArcGraph *arc = *itA;

			if (arc->dest->tspNextHop == nullptr) {
				actNode->tspNextHop = arc;

				for (auto &nh : arc->dest->arcs) {
					if (nh->dest->poi_id == actNode->poi_id) {
						arc->dest->tspPreviousHop = nh;
						break;
					}
				}

				actNode = arc->dest;
				found = true;
				break;
			}
		}

	} while (found);
	cout << "OK FORWARD TPS" << endl; fflush(stdout);
	for (auto &nh : actNode->arcs) {
		if (nh->dest->poi_id == firstNode->poi_id) {
			actNode->tspNextHop = nh;
			for (auto &nh2 : nh->dest->arcs) {
				if (nh2->dest->poi_id == actNode->poi_id) {
					nh->dest->tspPreviousHop = nh2;
					break;
				}
			}
			break;
		}
	}

	cout << "TSP Nearest Neighbor: " << firstNode->poi_id << " -|" << firstNode->tspNextHop->weight << "|-> " << firstNode->tspNextHop->dest->poi_id;
	actNode = firstNode->tspNextHop->dest;
	while (actNode->poi_id != firstNode->poi_id) {
		cout << " -|" << actNode->tspNextHop->weight << "|-> " << actNode->tspNextHop->dest->poi_id;
		actNode = actNode->tspNextHop->dest;
	}
	cout << " === Total cost: " << calculateTSPcost(tspGraphMap) << endl;

	cout << "TSP Nearest Neighbor (BACK): " << firstNode->poi_id << " -|" << firstNode->tspPreviousHop->weight << "|-> " << firstNode->tspPreviousHop->dest->poi_id;
	actNode = firstNode->tspPreviousHop->dest;
	while (actNode->poi_id != firstNode->poi_id) {
		cout << " -|" << actNode->tspPreviousHop->weight << "|-> " << actNode->tspPreviousHop->dest->poi_id;
		actNode = actNode->tspPreviousHop->dest;
	}
	cout << " === Total cost: " << calculateTSPcost_backward(tspGraphMap) << endl;

	//2-opt enhancement

	cout << "TSP 2-opt: " << firstNode->poi_id << " -|" << firstNode->tspNextHop->weight << "|-> " << firstNode->tspNextHop->dest->poi_id;
	actNode = firstNode->tspNextHop->dest;
	while (actNode->poi_id != firstNode->poi_id) {
		cout << " -|" << actNode->tspNextHop->weight << "|-> " << actNode->tspNextHop->dest->poi_id;
		actNode = actNode->tspNextHop->dest;
	}
	cout << " === Total cost: " << calculateTSPcost(tspGraphMap) << endl;

	cout << "TSP 2-opt (BACK): " << firstNode->poi_id << " -|" << firstNode->tspPreviousHop->weight << "|-> " << firstNode->tspPreviousHop->dest->poi_id;
	actNode = firstNode->tspPreviousHop->dest;
	while (actNode->poi_id != firstNode->poi_id) {
		cout << " -|" << actNode->tspPreviousHop->weight << "|-> " << actNode->tspPreviousHop->dest->poi_id;
		actNode = actNode->tspPreviousHop->dest;
	}
	cout << " === Total cost: " << calculateTSPcost_backward(tspGraphMap) << endl;

}

unsigned int TspGraph::calculateTSPcost_kruskal(TspNodeGraph *root, unsigned int kruskal_level) {
	unsigned int cost = root->kruskal_tspNextHop[kruskal_level]->weight;

	TspNodeGraph *actNode = root->kruskal_tspNextHop[kruskal_level]->dest;
	while (actNode->poi_id != root->poi_id) {
		//cout << " -|" << actNode->kruskal_tspNextHop[kruskal_level]->weight << "|-> " << actNode->kruskal_tspNextHop[kruskal_level]->dest->poi_id;
		cost += actNode->kruskal_tspNextHop[kruskal_level]->weight;
		actNode = actNode->kruskal_tspNextHop[kruskal_level]->dest;
	}

	return cost;
}

unsigned int TspGraph::calculateTSPcost_kruskal_backward(TspNodeGraph *root, unsigned int kruskal_level) {
	unsigned int cost = root->kruskal_tspPreviousHop[kruskal_level]->weight;

	TspNodeGraph *actNode = root->kruskal_tspPreviousHop[kruskal_level]->dest;
	while (actNode->poi_id != root->poi_id) {
		//cout << " -|" << actNode->kruskal_tspNextHop[kruskal_level]->weight << "|-> " << actNode->kruskal_tspNextHop[kruskal_level]->dest->poi_id;
		cost += actNode->kruskal_tspPreviousHop[kruskal_level]->weight;
		actNode = actNode->kruskal_tspPreviousHop[kruskal_level]->dest;
	}

	return cost;
}

void TspGraph::calculateTSP_kruskal(TspNodeGraph *root, unsigned int kruskal_level) {

	TspNodeGraph *firstNode = root;
	TspNodeGraph *actNode = firstNode;
	bool found;

	do {
		found = false;

		actNode->arcs.sort(sortArcs);
		for (auto itA = actNode->arcs.begin(); itA != actNode->arcs.end(); itA++) {
			TspArcGraph *arc = *itA;

			if ((arc->dest->kruskal_tspNextHop[kruskal_level] == nullptr) &&
					(actNode->kruskal_tspID[kruskal_level] == arc->dest->kruskal_tspID[kruskal_level]) ) {
				actNode->kruskal_tspNextHop[kruskal_level] = arc;

				for (auto &nh : arc->dest->arcs) {
					if (nh->dest->poi_id == actNode->poi_id) {
						arc->dest->kruskal_tspPreviousHop[kruskal_level] = nh;
						break;
					}
				}

				actNode = arc->dest;
				found = true;
				break;
			}
		}

	} while (found);
	for (auto &nh : actNode->arcs) {
		if (nh->dest->poi_id == firstNode->poi_id) {
			actNode->kruskal_tspNextHop[kruskal_level] = nh;
			for (auto &nh2 : nh->dest->arcs) {
				if (nh2->dest->poi_id == actNode->poi_id) {
					nh->dest->kruskal_tspPreviousHop[kruskal_level] = nh2;
					break;
				}
			}
			break;
		}
	}

	cout << "TSP Nearest Neighbor at kruskal level " << kruskal_level << ": " << firstNode->poi_id << " -|" << firstNode->kruskal_tspNextHop[kruskal_level]->weight << "|-> " << firstNode->kruskal_tspNextHop[kruskal_level]->dest->poi_id;
	actNode = firstNode->kruskal_tspNextHop[kruskal_level]->dest;
	while (actNode->poi_id != firstNode->poi_id) {
		cout << " -|" << actNode->kruskal_tspNextHop[kruskal_level]->weight << "|-> " << actNode->kruskal_tspNextHop[kruskal_level]->dest->poi_id;
		actNode = actNode->kruskal_tspNextHop[kruskal_level]->dest;
	}
	cout << " === Total cost: " << calculateTSPcost_kruskal(root, kruskal_level) << endl;

}

int TspGraph::updateUavPathToNextPoi_FromStopsSet(unsigned int &risStartStop, std::vector<Stops *> &startPoiNeigh, Uav *uav, unsigned int t, bool reserve) {

	std::map<unsigned int, std::list<ArcGraph *> > arcMapList;
	std::map<unsigned int, unsigned int > arcMapListCost;
	std::map<unsigned int, std::list<ArcGraph *> > stopArcMapList;
	std::map<unsigned int, unsigned int > stopArcMapListCost;

	int minCost = std::numeric_limits<int>::max();
	unsigned int minStopCostFull;

	for (auto &s : startPoiNeigh) {

		std::list<ArcGraph *> newList = std::list<ArcGraph *>();

		getMinimumPathToFew(arcMapList, arcMapListCost, s->getStopIdNum(), t, uavTSP[uav->getId()]->tspNextHop->dest->neighStops);
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

		stopArcMapList[s->getStopIdNum()] = newList;
		stopArcMapListCost[s->getStopIdNum()] = risCost;

		if ((minCost > risCost) && (risCost > 0)) {
			minCost = risCost;
			minStopCostFull = s->getStopIdNum();
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

		risStartStop = minStopCostFull;

		stopArcMapList.clear();
		stopArcMapListCost.clear();

		return minCost;
	}
	else {
		return 0;
	}
}

int TspGraph::updateUavPathToNextPoi(Uav *uav, unsigned int t, bool reserve) {
	std::map<unsigned int, std::list<ArcGraph *> > arcMapList;
	std::map<unsigned int, unsigned int > arcMapListCost;

	//uav_extra_backup.push_back(uav);

	getMinimumPathToFew(arcMapList, arcMapListCost, uav->getPositionStopId(), t, uavTSP[uav->getId()]->tspNextHop->dest->neighStops);
	unsigned int minStopID;
	unsigned int minStopCost = std::numeric_limits<unsigned int>::max();
	for (auto& al : arcMapListCost) {
		if (al.second < minStopCost) {
			minStopID = al.first;
			minStopCost = al.second;
		}
	}
	uavArcMapList[uav->getId()].clear();
	int risCost = 0;
	for (auto itAL = arcMapList[minStopID].begin(); itAL != arcMapList[minStopID].end(); itAL++) {
		//for (auto& al : arcMapList[minStopID]) {
		uavArcMapList[uav->getId()].push_back(*itAL);
		risCost += (*itAL)->dest->time - (*itAL)->src->time;
	}

	if (uavArcMapList[uav->getId()].size() == 0) {
		//cerr << "Non c'è un percorso verso il POI " << uavTSP[uav->getId()]->tspNextHop->dest->poi_id << " per lo UAV " << uav->getId() << endl;
		//exit(EXIT_FAILURE);
		return 0;
	}

	if (reserve) {
		for (auto& a : uavArcMapList[uav->getId()]) {
			if (a->arc_t == ArcGraph::BUS) {
				a->reserved = true;
			}
		}
	}

	/*cout << "ORIGIN: ";
	for (auto itAL = arcMapList[minStopID].begin(); itAL != arcMapList[minStopID].end(); itAL++) {
		cout << (*itAL)->src->stop_id << "_" << (*itAL)->src->time << " -> " << (*itAL)->dest->stop_id << "_" << (*itAL)->dest->time << " ";
	}
	cout << endl << "COPY  : ";
	for (auto itAL = uavArcMapList[uav->getId()].begin(); itAL != uavArcMapList[uav->getId()].end(); itAL++) {
		cout << (*itAL)->src->stop_id << "_" << (*itAL)->src->time << " -> " << (*itAL)->dest->stop_id << "_" << (*itAL)->dest->time << " ";
	}
	cout << endl;
	exit(EXIT_SUCCESS);*/
	return risCost;
}

void TspGraph::setInitExtraUAV(std::list <Uav *> &remainingUAV, struct std::tm time_tm, std::map<unsigned long int, Stops> &stopsMap) {

	if (sim->isToCluster()) {
		auto itU = remainingUAV.begin();

		for (auto& root : kruskal_mstRootNode[bestClusteringLayer]) {

			unsigned int tspCost = calculateTSPcost_kruskal(root, bestClusteringLayer);
			//unsigned int tspSplitSize = tspCost / remainingUAV.size();
			int uavInThisCluster = clusterIndexUav[bestClusteringLayer][root->kruskal_tspID[bestClusteringLayer]];
			unsigned int tspSplitSize = tspCost / uavInThisCluster;
			unsigned int countCost = 0;
			unsigned int nextLimit = 0;
			int uavPutInThisCluster = 0;
			TspNodeGraph *actNode;

			//actNode = tspGraphMap.begin()->second;
			actNode = root;
			nextLimit = actNode->tspNextHop->weight;
			while ((countCost < tspCost) && (itU != remainingUAV.end()) && (uavPutInThisCluster < uavInThisCluster)) {
				Uav *uav = *itU;
				uav->setState(Uav::UAV_FLYING);
				uav->unsetCovering();
				uavPutInThisCluster++;

				uav_extra_backup.push_back(uav);

				//set the tsp for each "free" uav
				uavTSP[uav->getId()] = actNode;

				int minArcPath = std::numeric_limits<int>::max();
				unsigned long int bestStartStop = (*(actNode->neighStops.begin()))->getStopIdNum();
				for (auto& ns : actNode->neighStops) {
					uav->setPositionStopId(ns->getStopIdNum());
					int cost = updateUavPathToNextPoi(uav, day_tm2seconds(time_tm), false);
					if (cost > 0) {
						if (minArcPath > cost) {
							minArcPath = cost;
							bestStartStop = ns->getStopIdNum();
						}
					}
				}

				if (minArcPath < std::numeric_limits<int>::max()) {
					uav->setPositionStopId(bestStartStop);
					updateUavPathToNextPoi(uav, day_tm2seconds(time_tm), true);
					//cout << "Found a path with cost: " << minArcPath << endl;
				}
				else {
					cerr << "Warning!!! No path route from the POI " << actNode->poi_id << " to the POI " << actNode->tspNextHop->dest->poi_id << " for UAV " << uav->getId() << endl;
				}


				setUavPosition(time_tm, uav);

				countCost += tspSplitSize;

				cout << "UAV" << uav->getId() << ". POI: " << actNode->poi_id << " -> " << actNode->tspNextHop->dest->poi_id << " first path starting from: " << uav->getPositionStopId() << endl;

				if (countCost >= nextLimit) {
					actNode = actNode->tspNextHop->dest;
					nextLimit += actNode->tspNextHop->weight;
				}

				int i = 0;
				for (auto& a : uavArcMapList[uav->getId()]) {
					cout << a->src->stop_id << "_" << a->src->time <<  " -> " << a->dest->stop_id << "_" << a->dest->time <<  " -- ";
					if (i++ > 5) break;
				}
				cout << endl << endl;


				itU++;
			}
		}
	}
	else {
		unsigned int tspCost = calculateTSPcost(tspGraphMap);
		unsigned int tspSplitSize = tspCost / remainingUAV.size();
		unsigned int countCost = 0;
		unsigned int nextLimit = 0;
		TspNodeGraph *actNode;
		auto itU = remainingUAV.begin();

		actNode = tspGraphMap.begin()->second;
		nextLimit = actNode->tspNextHop->weight;
		while ((countCost < tspCost) && (itU != remainingUAV.end())) {
			Uav *uav = *itU;
			uav->setState(Uav::UAV_FLYING);
			uav->unsetCovering();

			uav_extra_backup.push_back(uav);

			//set the tsp for each "free" uav
			uavTSP[uav->getId()] = actNode;

			//cout << "Check INIT Sono: UAV" << uav->getId() << endl;
			//cout << "Check INIT. Looking for path route from the POI " << actNode->poi_id << " to the POI " << actNode->tspNextHop->dest->poi_id << " for UAV " << uav->getId() << endl;
			//cout << "The list of the neighStop of POI: " << actNode->poi_id << " is: ";
			//for (auto& ns : actNode->neighStops) {
			//	cout << ns->getStopIdNum() << " ";
			//}
			//cout << endl << "The list of the neighStop of POI: " << actNode->tspNextHop->dest->poi_id << " is: ";
			//for (auto& ns : actNode->tspNextHop->dest->neighStops) {
			//	cout << ns->getStopIdNum() << " ";
			//}
			//cout << endl;


			int minArcPath = std::numeric_limits<int>::max();
			unsigned long int bestStartStop = (*(actNode->neighStops.begin()))->getStopIdNum();
			for (auto& ns : actNode->neighStops) {
				uav->setPositionStopId(ns->getStopIdNum());
				int cost = updateUavPathToNextPoi(uav, day_tm2seconds(time_tm), false);
				if (cost > 0) {
					if (minArcPath > cost) {
						minArcPath = cost;
						bestStartStop = ns->getStopIdNum();
					}
				}
			}

			if (minArcPath < std::numeric_limits<int>::max()) {
				uav->setPositionStopId(bestStartStop);
				updateUavPathToNextPoi(uav, day_tm2seconds(time_tm), true);
				//cout << "Found a path with cost: " << minArcPath << endl;
			}
			else {
				cerr << "Warning!!! No path route from the POI " << actNode->poi_id << " to the POI " << actNode->tspNextHop->dest->poi_id << " for UAV " << uav->getId() << endl;
			}

			/*
				uav->setPositionStopId(actNode->neighStops[(int)(std::rand() % actNode->neighStops.size())]->getStopIdNum());
				//calculate the fastest path to get to the next POI
				bool goodPath = updateUavPathToNextPoi(uav, day_tm2seconds(time_tm));

				if (!goodPath) {
					for (auto& ps : actNode->neighStops) {
						uav->setPositionStopId(ps->getStopIdNum());
						if (updateUavPathToNextPoi(uav, day_tm2seconds(time_tm))) {
							break;
						}
					}
				}
			 */

			setUavPosition(time_tm, uav);


			/*std::map<unsigned int, std::list<ArcGraph *> > arcMapList;
				std::map<unsigned int, unsigned int > arcMapListCost;
				getMinimumPathToFew(arcMapList, arcMapListCost, uav->getPositionStopId(), day_tm2seconds(time_tm), uavTSP[uav->getId()]->tspNextHop->dest->neighStops);
				unsigned int minStopID;
				unsigned int minStopCost = std::numeric_limits<unsigned int>::max();
				for (auto& al : arcMapListCost) {
					if (al.second < minStopCost) {
						minStopID = al.first;
						minStopCost = al.second;
					}
				}
				uavArcMapList[uav->getId()].clear();
				for (auto itAL = arcMapList[minStopID].begin(); itAL != arcMapList[minStopID].end(); itAL++) {
					uavArcMapList[uav->getId()].push_back(*itAL);
				}*/

			countCost += tspSplitSize;

			//cout << "UAV " << uav->getId() << " set on random stop: " << uav->getPositionStopId() << " on POI: " << actNode->poi_id << endl;

			cout << "UAV" << uav->getId() << ". POI: " << actNode->poi_id << " -> " << actNode->tspNextHop->dest->poi_id << " first path starting from: " << uav->getPositionStopId() << endl;

			if (countCost >= nextLimit) {
				actNode = actNode->tspNextHop->dest;
				nextLimit += actNode->tspNextHop->weight;
			}


			//int i = 0;
			//for (auto& a : uavArcMapList[uav->getId()]) {
			//	cout << a->src->stop_id << "_" << a->src->time <<  " -> " << a->dest->stop_id << "_" << a->dest->time <<  " -- ";
			//	if (i++ > 5) break;
			//}
			//cout << endl << endl;

			itU++;
		}
	}




	//exit (EXIT_SUCCESS);
}

void TspGraph::activateUavFlow(unsigned int time, std::list<Uav *> &uavList){
	std::list <Uav *> coveringUAV;
	std::list <Uav *> not_coveringUAV;

	for (auto& u : uavList){
		if (u->getState() == Uav::UAV_FLYING) {
			if (u->isCovering()) {
				//cout << "UAV " << u->getId() << " is covering POI: " << u->getCoveringPoiId() << " and is at stop: " << u->getPositionStopId() << endl;
				coveringUAV.push_back(u);
			}
			else {
				//cout << "UAV " << u->getId() << " is not covering and is at stop: " << u->getPositionStopId() << endl;
				//int i = 0;
				//for (auto& a : uavArcMapList[u->getId()]) {
				//	cout << a->src->stop_id << "_" << a->src->time <<  " -> " << a->dest->stop_id << "_" << a->dest->time <<  " -- ";
				//	if (i++ > 5) break;
				//}
				//cout << endl;
				not_coveringUAV.push_back(u);

				//cout << "UAV" << u->getId() << " is on state " << u->getPositionStopId() << "_" << time << endl;
			}
		}
	}

	while (!not_coveringUAV.empty()) {
		Uav *u = not_coveringUAV.front();
		not_coveringUAV.pop_front();

		NodeGraph *ng = graphMapMap[u->getPositionStopId()][time];

		//cout << time << " UAV " << u->getId() << " Analyzing the UAV " << u->getId() << endl;

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

			cout << endl << time << " UAV " << u->getId() << " arrived at POI " << uavTSP[u->getId()]->tspNextHop->dest->poi_id <<
					" on stop " << u->getPositionStopId() << endl;

			bool inTheRightStop = false;
			for (auto& s : uavTSP[u->getId()]->tspNextHop->dest->neighStops) {
				if (s->getStopIdNum() == u->getPositionStopId()) {
					inTheRightStop = true;
				}
			}

			if (inTheRightStop) {
				// trovo l'arco e me ne approprio
				for (auto a : ng->arcs) {
					if ( (a->arc_t == ArcGraph::COVER) && (a->p != nullptr) && (a->p->getPoiIdNum() == uavTSP[u->getId()]->tspNextHop->dest->poi_id) ) {
						Poi *pp = uavTSP[u->getId()]->tspNextHop->dest->poiPtr;

						//a->uavOnTheArc.push_back(u);
						//activeArc.push_back(a);

						swapCount++;

						u->setCoveringPoiId(pp->getPoiIdNum());

						//swappo col nodo che stava sul POI
						Uav *uavSwap = pp->getCoveringUav();
						pp->setCoveringUav(u);
						coveringUAV.push_back(u);


						//assegno all'uav che era in copertura il percorso di quello che è arrivato
						uavTSP[uavSwap->getId()] = uavTSP[u->getId()]->tspNextHop->dest;
						TspNodeGraph *originTspSrc = uavTSP[uavSwap->getId()]->tspPreviousHop->dest;

						cout << time << " UAV " << u->getId() << " swapping with UAV " << uavSwap->getId() <<
								" that now is going going from POI " << uavTSP[uavSwap->getId()]->poi_id << " -> " << uavTSP[uavSwap->getId()]->tspNextHop->dest->poi_id << endl;

						std::vector<Stops *> neighPoiStops;
						sim->getNeighStop(uavSwap->getCoveringPoiId(), neighPoiStops);

						while (originTspSrc != uavTSP[uavSwap->getId()]) {
							//int costPath = updateUavPathToNextPoi(uavSwap, time, false);

							unsigned int startStopFromPoi;

							int costPath = updateUavPathToNextPoi_FromStopsSet(startStopFromPoi, neighPoiStops, uavSwap, time, false);
							if (costPath == 0) {
								cerr << time << " UAV " << u->getId() << " Warning!!! Not found a path for POI " << uavTSP[uavSwap->getId()]->poi_id << ".";
								uavTSP[uavSwap->getId()] = uavTSP[uavSwap->getId()]->tspNextHop->dest;
								cerr << " Jumping to the next POI " << uavTSP[uavSwap->getId()]->poi_id << endl;
							}
							else {
								//recalculate to reserve the buses
								//updateUavPathToNextPoi_FromStopsSet(startStopFromPoi, neighPoiStops, uavSwap, time, true);
								for (auto& a : uavArcMapList[uavSwap->getId()]) {
									if (a->arc_t == ArcGraph::BUS) {
										a->reserved = true;
									}
								}
								if (startStopFromPoi != uavSwap->getPositionStopId()) {
									//unsigned int old_startStopFromPoi = uavSwap->getPositionStopId();
									NodeGraph *ng_now = graphMapMap[startStopFromPoi][time];
									//NodeGraph *ng_old = graphMapMap[old_startStopFromPoi][time];

									cout << time << " UAV" << uavSwap->getId() << " Changing stop on POI from: " << uavSwap->getPositionStopId() << " to " << startStopFromPoi << endl; fflush(stdout);

									uavSwap->setPositionStopId(startStopFromPoi);
									//setUavPosition(day_seconds2tm(time), uavSwap);
									ng_now->uavs.push_back(uavSwap);

									/*for (auto itOld = ng_old->uavs.begin(); itOld != ng_old->uavs.end(); itOld++) {
										Uav *o = *itOld;
										if (o->getId() == uavSwap->getId()) {
											ng_old->uavs.erase(itOld);
											break;
										}
									}*/

									//cout << time << " UAV " << uavSwap->getId() << " Matrice: " << graphMapMap[startStopFromPoi][time]->stop_id << "_" << graphMapMap[startStopFromPoi][time]->time << endl;
									//cout << time << " UAV " << uavSwap->getId() << " Io, secondo me, sono sullo stop: " << uavSwap->getPositionStopId() << endl;

								}
								//updateUavPathToNextPoi(uavSwap, time, true);
								//cout << time << " UAV" << uavSwap->getId() << " Found a path with cost: " << costPath << " for UAV " << uavSwap->getId() << endl;
								cout << time << " UAV" << uavSwap->getId() << " found path starting from: " << uavSwap->getPositionStopId() << " with cost " << costPath << endl;
								/*int i = 0;
								for (auto& a : uavArcMapList[uavSwap->getId()]) {
										cout << a->src->stop_id << "_" << a->src->time <<  " -> " << a->dest->stop_id << "_" << a->dest->time <<  " -- ";
										f (i++ > 5) break;
								}
								cout << endl << endl;*/
								break;
							}
						}
						if (originTspSrc == uavTSP[uavSwap->getId()]) {
							cerr << time << " UAV " << u->getId() << " The UAV is blocked!!!! Killing him..." << endl;
							//exit (EXIT_FAILURE);
							//kill the UAV--- setting all stops arcs
							unsigned int act_time = time;
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

						uavSwap->unsetCovering();

						/*int goodPath = updateUavPathToNextPoi(uavSwap, time, true);

						if (goodPath == 0) {
							cerr << "Warning: no path toward the next POI" << endl;
						}
						else {
							cout << "Found a path with cost: " << goodPath << " for UAV " << uavSwap->getId() << endl;

							cout << "UAV" << uavSwap->getId() << " found path starting from: " << uavSwap->getPositionStopId() << endl;

						}*/

						// cancello quello vecchio
						uavTSP.erase(u->getId());
						uavArcMapList.erase(u->getId());

						// metto il nuovo nodo tra quelli non coprenti
						// prima controlllo che è tutto corretto
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

							// check if I was already been here
							/*for (auto itArc = activeArc.begin(); itArc != activeArc.end(); itArc++) {
								ArcGraph *aa = *itArc;
								if ((aa->arc_t == ArcGraph::COVER) && (aa->src->time == time)) {
									for (auto itUav = aa->uavOnTheArc.begin(); itUav != a->uavOnTheArc.end(); itUav++) {
										Uav *uu = *itUav;
										if (uu->getId() == uavSwap->getId()) {
											aa->uavOnTheArc.erase(itUav);
											break;
										}
									}
								}
							}*/
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
			else {
				cout << endl << "Dove cazzo sono andato??? Sono: UAV" << u->getId() << endl;
				cout << "The list of the neighStop of POI: " << uavTSP[u->getId()]->tspNextHop->dest->poi_id << " is: ";
				std::vector<Stops *> vs;
				sim->getNeighStop(uavTSP[u->getId()]->tspNextHop->dest->poi_id, vs);
				for (auto& ns : vs) {
					cout << ns->getStopIdNum() << " ";
				}
				cout << endl;
				exit(EXIT_FAILURE);
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
}

