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
	r = new requestInfo();
	r->art = requestInfo::ART::TURNOUT;
	r->id = addr;
}

ConnectorTurnout::~ConnectorTurnout() {
}

void ConnectorTurnout::TurnoutCmd(int id, int direction, int source) {
	if (id != this->id) {
		return;
	}
	action->setSettings(String(id), String(direction));
}

