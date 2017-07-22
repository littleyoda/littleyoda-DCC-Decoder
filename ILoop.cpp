/*
 * interfaceLoop.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include "ILoop.h"

#include <climits>

ILoop::ILoop() {
}

int ILoop::loop() {
	return 1000 * 60 * 60 * 24; // One day
}


ILoop::~ILoop() {
}

