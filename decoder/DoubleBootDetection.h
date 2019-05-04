/*
 * DoubleBootDetection.h
 *
 *  Created on: 15.06.2017
 *      Author: sven
 */

#ifndef DOUBLEBOOTDETECTION_H_
#define DOUBLEBOOTDETECTION_H_

#include <Arduino.h>
#include "Controller.h"
#include "ILoop.h"

class DoubleBootDetection : public ILoop {
public:
	DoubleBootDetection(Controller* c);
	virtual ~DoubleBootDetection();
	virtual int loop();
private:
	int status = 0;
	String filename = "/DoubleBoot";
	String blockfilename = "/disabledoubleboot";
	Controller* controll;
};

#endif /* DOUBLEBOOTDETECTION_H_ */
