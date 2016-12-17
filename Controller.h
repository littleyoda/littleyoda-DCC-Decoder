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
	void notifyTurnout(int id, int direction, int source);
	void notifyDCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	void notifyDCCFun(int id, int startbit, int stopbit, unsigned long value, int source);

	String getHTMLController();
	String getHTMLCfg();
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
	LinkedList<unsigned long> nextRun = LinkedList<unsigned long>();
	long int lastTurnoutCmd[3];

};

#endif /* CONTROLLER_H_ */
