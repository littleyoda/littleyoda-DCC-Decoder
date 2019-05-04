/*
 * ConnectorLocoSpeed.h
 *
 *  Created on: 18.07.2017
 *      Author: sven
 */

#ifndef CONNECTORLOCOSPEED_H_
#define CONNECTORLOCOSPEED_H_
#include "Connectors.h"
#include "ISettings.h"

class ConnectorLocoSpeed: public Connectors {
public:
	ConnectorLocoSpeed(ISettings* a, int locoaddr);
	virtual ~ConnectorLocoSpeed();
	void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);

private:
	ISettings* action;
	int addr;
};

#endif /* CONNECTORLOCOSPEED_H_ */
