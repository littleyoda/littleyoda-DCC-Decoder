/*
 * WebserviceDCCLogger.h
 *
 *  Created on: 07.09.2016
 *      Author: sven
 */

#ifndef WEBSERVICECOMMANDLOGGER_H_
#define WEBSERVICECOMMANDLOGGER_H_

#include "WebserviceBase.h"
#include "ActionBase.h"
#include <LinkedList.h>

class WebserviceCommandLogger : public WebserviceBase, public ActionBase {
public:
	WebserviceCommandLogger();
	virtual ~WebserviceCommandLogger();
	virtual void TurnoutCmd(int id, int direction, int source);
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	virtual void DCCFunc(int id, unsigned long int newvalue, int source);
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);

	virtual char const* getUri();
	virtual void run();
	virtual String  getLinkText();
	unsigned int getMemUsage();
private:
	LinkedList<String> logger = LinkedList<String>();
	 void addToLog(String s);

};

#endif /* WEBSERVICECOMMANDLOGGER_H_ */
