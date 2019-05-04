/*
 * ActionSendTurnoutCommand.h
 *
 *  Created on: 04.01.2018
 *      Author: sven
 */

#ifndef ACTIONSENDTURNOUTCOMMAND_H_
#define ACTIONSENDTURNOUTCOMMAND_H_

#include "ISettings.h"
#include "Controller.h"

class ActionSendTurnoutCommand : public ISettings, public INotify {
public:
	ActionSendTurnoutCommand(Controller* c, int id);
	virtual ~ActionSendTurnoutCommand();
	virtual void setSettings(String key, String value);
	virtual void setSettings(int status);
	virtual void TurnoutCmd(int id, int direction, int source);
private:
	Controller* controller;
	sint16 id;
	uint8 richtung;
};

#endif /* ACTIONSENDTURNOUTCOMMAND_H_ */
