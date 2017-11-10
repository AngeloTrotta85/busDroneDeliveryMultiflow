/*
 * Package.cpp
 *
 *  Created on: 20 ott 2017
 *      Author: angelo
 */

#include "Package.h"

int Package::id_counter = 0;

Package::~Package() {
	// TODO Auto-generated destructor stub
}


Package::Package(double p, double l, double w, double h) {
	weight = p;
	length = l;
	width = w;
	height = h;

	dest_dp = nullptr;

	id = id_counter++;
}
