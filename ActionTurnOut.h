/*
 * ActionTurnOut.h
 *
 *  Created on: 19.08.2016
 *      Author: sven
 */

#ifndef ACTIONTURNOUT_H_
#define ACTIONTURNOUT_H_

#include <LinkedList.h>
#include "ActionTurnOut.h"
#include "ActionBase.h"

class ActionTurnOut: public ActionBase {
public:
	ActionTurnOut(int dir1, int dir2, int enable, int id);
	virtual void loop();
	virtual void TurnoutCmd(int id, int direction);
	virtual ~ActionTurnOut();
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);

private:
	int dirPin[2];
	int enable;
	int id;
	void off();

};

#endif /* ACTIONTURNOUT_H_ */
