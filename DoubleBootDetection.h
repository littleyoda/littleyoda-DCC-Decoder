/*
 * DoubleBootDetection.h
 *
 *  Created on: 15.06.2017
 *      Author: sven
 */

#ifndef DOUBLEBOOTDETECTION_H_
#define DOUBLEBOOTDETECTION_H_

#include <Arduino.h>
#include "interfaceLoop.h"
#include "Controller.h"

class DoubleBootDetection : public interfaceLoop {
public:
	DoubleBootDetection(Controller* c);
	virtual ~DoubleBootDetection();
	virtual int loop();
private:
	int status = 0;
	String filename = "/DoubleBoot";
	Controller* controll;
};

#endif /* DOUBLEBOOTDETECTION_H_ */
