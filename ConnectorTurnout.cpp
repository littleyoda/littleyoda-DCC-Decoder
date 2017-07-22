/*
 * ConnectorTurnout.cpp
 *
 *  Created on: 21.07.2017
 *      Author: sven
 */

#include "ConnectorTurnout.h"

ConnectorTurnout::ConnectorTurnout(ISettings* a, int addr) {
	action = a;
	id = addr;
}

ConnectorTurnout::~ConnectorTurnout() {
}

void ConnectorTurnout::TurnoutCmd(int id, int direction, int source) {
	if (id != this->id) {
		return;
	}
	action->setSettings(String(id), String(direction));
}
