/*
 * ConnectorTurnout.h
 *
 *  Created on: 21.07.2017
 *      Author: sven
 */

#ifndef CONNECTORTURNOUT_H_
#define CONNECTORTURNOUT_H_

#include "ISettings.h"
#include "Connectors.h"

class ConnectorTurnout: public Connectors {
public:
	ConnectorTurnout(ISettings* a, int addr);
	virtual ~ConnectorTurnout();
	virtual void TurnoutCmd(int id, int direction, int source)  override;

private:
	int id;
};

#endif /* CONNECTORTURNOUT_H_ */
