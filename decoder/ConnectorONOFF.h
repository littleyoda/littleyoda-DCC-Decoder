/*
 * ConnectorONOFF.h
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#ifndef CONNECTORONOFF_H_
#define CONNECTORONOFF_H_

#include "Connectors.h"
#include "ISettings.h"

class ConnectorONOFF: public Connectors {
public:
	ConnectorONOFF(ISettings* a, int locoaddr, int f);
	virtual ~ConnectorONOFF();
	virtual void DCCFunc(int id, int bit, int newvalue, int source);

private:
	int addr;
	int funcbit;
	requestInfo* r;


};

#endif /* CONNECTORONOFF_H_ */
