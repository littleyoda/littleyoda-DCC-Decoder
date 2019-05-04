/*
 * ConnectorTurnout2Value.h
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#ifndef ConnectorTurnout2Value_H_
#define ConnectorTurnout2Value_H_
#include "ISettings.h"
#include "Connectors.h"

class ConnectorTurnout2Value : public Connectors {
public:
	ConnectorTurnout2Value(ISettings* a, int locoaddr, int *array, int arraylength);
	virtual ~ConnectorTurnout2Value();
	virtual void TurnoutCmd(int id, int direction, int source);

private:
	int *array;
	ISettings* action;
	int addr;
	int arraylength;
};

#endif /* ConnectorTurnout2Value_H_ */
