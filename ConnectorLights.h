/*
 * ConnectorLights.h
 *
 *  Created on: 23.07.2017
 *      Author: sven
 */

#ifndef CONNECTORLIGHTS_H_
#define CONNECTORLIGHTS_H_

#include "Connectors.h"
#include "ISettings.h"

class ConnectorLights : public Connectors {
public:
	ConnectorLights(ISettings* vorne, ISettings* hinten, int locoaddr, int anaus);
	virtual ~ConnectorLights();
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	virtual void DCCFunc(int id, unsigned long int newvalue, int source);

private:
	void update();
	ISettings *ptr[2];
	int addr;
	int f0;
	unsigned long int fstatus;
	int richtung;

};

#endif /* CONNECTORLIGHTS_H_ */
