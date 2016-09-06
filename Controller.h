/*
 * ActionReceiverMediator.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <ESP8266WebServer.h>
#include <LinkedList.h>
#include <map>

#include "CmdReceiverBase.h"
#include "ActionBase.h"
#include "interfaceLoop.h"

class CmdReceiverBase;
// forward declaration

class Controller {
public:
	Controller();
	virtual ~Controller();
	void registerCmdReceiver(CmdReceiverBase* base);
	void registerAction(ActionBase* base);
	void registerLoop(interfaceLoop* loop);
	void doLoops();

	// Notifications from Outside (via DCC, WLAN, ..)
	void notifyTurnout(int id, int direction);
	void notifyDCCSpeed(int id, int speed, int direction, int SpeedSteps);
	void notifyDCCFun(int id, int startbit, int stopbit, unsigned long value);

	String getHTMLController();
	void setRequest(String id, String key, String value);

private:
	struct LocData {
		int speed;
		int direction;
		int speedsteps;
	};
	typedef std::map<int, LocData*> Items;
	Items items;

	struct FuncData {
		unsigned long int status;
	};
	typedef std::map<int, FuncData*> FuncDatas;
	FuncDatas funcdatas;

	LinkedList<CmdReceiverBase*> receiver = LinkedList<CmdReceiverBase*>();
	LinkedList<ActionBase*> actions = LinkedList<ActionBase*>();
	LinkedList<interfaceLoop*> loops = LinkedList<interfaceLoop*>();
	long int lastTurnoutCmd[3];

};

#endif /* CONTROLLER_H_ */
