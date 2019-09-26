/*
 * ConnectorTurnout.cpp
 *
 *  Created on: 21.07.2017
 *      Author: sven
 */

#include "ConnectorTurnout.h"

ConnectorTurnout::ConnectorTurnout(ISettings* a, int addr) {
	addAction(a);
	id = addr;
	r = new requestInfo();
	r->art = requestInfo::ART::TURNOUT;
	r->id = addr;
	setModulName("Turnout");
	setConfigDescription("ID: " + String(addr));

}

ConnectorTurnout::~ConnectorTurnout() {
}

void ConnectorTurnout::TurnoutCmd(int id, int direction, int source) {
	if (id != this->id) {
		return;
	}
	send(String(id), String(direction));
}

