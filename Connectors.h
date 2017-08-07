/*
 * Connectors.h
 *
 *  Created on: 18.07.2017
 *      Author: sven
 */

#ifndef CONNECTORS_H_
#define CONNECTORS_H_

#include <Arduino.h>
#include <LinkedList.h>

#include "INotify.h"

class Connectors: public INotify {
public:
	Connectors();
	virtual ~Connectors();

	virtual void TurnoutCmd(int id, int direction, int source);

	// Reports the Loco Speed and Direction
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);

	// Reports just the changed Bit
	virtual void DCCFunc(int id, int bit, int newvalue, int source);

	// Reports the Status of the all functions (bit 0 => F0, bit 1 => F1, ...)
	virtual void DCCFunc(int id, unsigned long int newvalue, int source);

};


#endif /* CONNECTORS_H_ */
