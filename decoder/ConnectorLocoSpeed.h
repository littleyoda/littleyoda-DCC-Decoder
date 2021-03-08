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
	virtual void DCCSpeed(int id, int speed, int direction, int source) override;

private:
	int addr;
};

#endif /* CONNECTORLOCOSPEED_H_ */
