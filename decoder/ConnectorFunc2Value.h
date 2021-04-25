/*
 * ConnectorFunc2Value.h
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#ifndef CONNECTORFUNC2VALUE_H_
#define CONNECTORFUNC2VALUE_H_
#include "ISettings.h"
#include "Connectors.h"

class ConnectorFunc2Value : public Connectors {
public:
	ConnectorFunc2Value(ISettings* a, int locoaddr, int *array, int arraylength, String var);
	virtual ~ConnectorFunc2Value();
	virtual void DCCFunc(int id, unsigned long int newvalue, int source) override;

private:
	int *array;
	int addr;
	int arraylength;
	String var;
};

#endif /* CONNECTORFUNC2VALUE_H_ */
