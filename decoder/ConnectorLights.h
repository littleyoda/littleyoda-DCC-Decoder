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
	ConnectorLights(ISettings* x, int locoaddr, int fkey, int richtung);
	virtual ~ConnectorLights();
	virtual void DCCSpeed(int id, int speed, int direction, int source) override;
	virtual void DCCFunc(int id, unsigned long int newvalue, int source) override;

private:
	void update();
	int addr;
	int func;
	int direction;

	unsigned long int currentFStatus;
	int currentDirection;
	requestInfo* r;
};

#endif /* CONNECTORLIGHTS_H_ */
