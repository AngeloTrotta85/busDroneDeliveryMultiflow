//============================================================================
// Name        : busDroneDeliveryMultiflow.cpp
// Author      : Angelo Trotta
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <algorithm>    // std::find
#include <vector>       // std::vector
#include <list>       // std::list
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

#include <boost/range/irange.hpp>

#include "MyCoord.h"
#include "Simulator.h"

using namespace std;

int main(int argc, char **argv) {

	Simulator sim;

	InputParser input(argc, argv);
	sim.importSomeParameterFromInputLine(&input);

	const std::string &foldername = input.getCmdOption("-f");
	if (!foldername.empty()) {
		std::string filename = foldername + "/" + STOPS_FILE;
		sim.importStops(filename);

		filename = foldername + "/" + CALENDAR_DATES_FILE;
		sim.importCalendarDates(filename);

		filename = foldername + "/" + TRIPS_FILE;
		sim.importTrips(filename);

		filename = foldername + "/" + STOP_TIMES_FILE;
		sim.importStopTimes(filename);

		filename = foldername + "/" + HOMES_FILE;
		sim.importHomes(filename);

		filename = foldername + "/" + DELIVERYPOINTS_FILE;
		sim.importDeliveryPoints(filename);
	}
	else {
		cerr << "Insert the input files folder" << endl;
		exit (EXIT_FAILURE);
	}

	const std::string &exportfoldername = input.getCmdOption("-e");
	if (!exportfoldername.empty()) {
		std::string filename = exportfoldername + "/" + STOPS_FILE;
		sim.exportStops(filename);

		filename = exportfoldername + "/" + CALENDAR_DATES_FILE;
		sim.exportCalendarDates(filename);

		filename = exportfoldername + "/" + TRIPS_FILE;
		sim.exportTrips(filename);

		filename = exportfoldername + "/" + STOP_TIMES_FILE;
		sim.exportStopTimes(filename);

		filename = exportfoldername + "/" + HOMES_FILE;
		sim.exportHomes(filename);

		filename = exportfoldername + "/" + DELIVERYPOINTS_FILE;
		sim.exportDeliveryPoints(filename);

		exit (EXIT_SUCCESS);
	}

	// generate the bus ruotes
	if (!sim.generateBusRoute()) {
		cout << "Error generating routes" << endl;
		return EXIT_FAILURE;
	}

	//std::srand(100);
	const std::string &seedString = input.getCmdOption("-seed");
	if (!seedString.empty()) {
		std::srand(atol(seedString.c_str()));
	}
	else {
		std::srand(std::time(0)); // use current time as seed for random generator
	}

	// set the maximum simulation time
	//sim.setMaxTime(100);
	sim.setMaxTime(std::numeric_limits<unsigned int>::max());	//DEPRECATED

	//set the number of UAV
	const std::string &nUavString = input.getCmdOption("-nUAV");
	if (!nUavString.empty()) {
		sim.setUav(atol(nUavString.c_str()));
	}
	else {
		sim.setUav(12);
	}

	//const std::string &tpsFileName = input.getCmdOption("-tsp");

	// init the simulation
	//if (!sim.init(tpsFileName)) {
	if (!sim.init()) {
		cout << "Error initializing the simulation" << endl;
		return EXIT_FAILURE;
	}

	if (input.cmdOptionExists("-noRun")) exit (EXIT_SUCCESS);

	// run the simulation
	cout << "Running the simulation" << endl;
	sim.run();

	// make stats
	cout << "Making stats" << endl;
	//sim.stats();
	const std::string &statsFileName = input.getCmdOption("-oSTAT");
	if (!statsFileName.empty()) {
		sim.stats(std::string(statsFileName));
	}
	else {
		sim.stats(std::string(OUTPUT_STATS_FILE));

	}

	const std::string &dotFileName = input.getCmdOption("-d");
	if (!dotFileName.empty()) {
		sim.exportDotResult(dotFileName);
	}

	cout << "End simulation" << endl;
	return EXIT_SUCCESS;
}
