/*
 * ActionSendTurnoutCommand.cpp
 *
 *  Created on: 04.01.2018
 *      Author: sven
 */

#include "ActionSendTurnoutCommand.h"
#include "Logger.h"

ActionSendTurnoutCommand::ActionSendTurnoutCommand(Controller* c, int number) {
	richtung = -1;
	controller = c;
	id = number;

	r = new requestInfo();
	r->art = requestInfo::ART::TURNOUT;
	r->id = number;

}

ActionSendTurnoutCommand::~ActionSendTurnoutCommand() {
}



void ActionSendTurnoutCommand::setSettings(String key, String value) {
	int status = value.toInt();
	setSettings(status);
}

void ActionSendTurnoutCommand::TurnoutCmd(int toId, int direction, int source) {
	if (toId != id ) {
		return;
	}
	richtung = direction;
}

void ActionSendTurnoutCommand::setSettings(int status) {
	if (status == 1) {
		return;
	}
	if (richtung == 1) {
		richtung = 0;
	} else {
		richtung = 1;
	}
	String s = String(richtung);
	// TODO Move to Controller
	LinkedList<CmdSenderBase*>* list = controller->getSender();
    for (int i = 0; i < list->size(); i++) {
    	CmdSenderBase* b = list->get(i);
    	if (b == NULL) {
			Logger::log(LogLevel::ERROR, "ActionSendTurnoutCommand: Sender is null");
    		continue;
    	}
    	b->sendSetTurnout(String(id), s);
    }
}

