/*
 * ActionReceiverMediator.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#ifdef ESP8266
	#include <ESP8266WebServer.h>
#else
	#include <WebServer.h>
#endif

#include <DNSServer.h>
#include <LinkedList.h>
#include <map>

#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include "ILoop.h"
#include "INotify.h"
#include "ISettings.h"
#include "WebserviceCommandLogger.h"
#include "WebserviceDCCSniffer.h"

	struct LocData {
		int16_t speed;
		int8_t direction;
		int8_t speedsteps;
		unsigned long int status;
	} __attribute__ ((packed));

	struct TurnOutData {
		int8_t direction;
	};

class CmdReceiverBase;
// forward declaration

class Controller {
public:

	Controller();
	virtual ~Controller();
	void registerCmdSender(CmdSenderBase* base);
	void registerCmdReceiver(CmdReceiverBase* base);
	void registerNotify(INotify* base);
	void registerLoop(ILoop* loop);
	void registerSettings(ISettings* loop);
	LinkedList<ISettings*>* getSettings();
	void updateRequestList();
	void doLoops();

	// Notifications from Outside (via DCC, WLAN, ..)
	void notifyTurnout(int id, int direction, int source);
	void notifyDCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	void notifyDCCFun(int id, int startbit, int stopbit, unsigned long value, int source);
	void notifyDCCFun(int id, int bit, unsigned int value, int source);
	void notifyGPIOChange(int pin, int newvalue);

	void enableAPModus();
	void getHTMLController();
	void getHTMLCfg();
	void sendContent(String s);
	void setRequest(String id, String key, String value);
	String getInternalStatus(String modul, String key);
	bool isEmergency();
	String getHostname();

	LocData* getLocData(int id);
	TurnOutData* getTurnOutData(int id);

	void emergencyStop(int source);

	LinkedList<CmdSenderBase*>* getSender()  {
		return &sender;
	}

	WebserviceCommandLogger* cmdlogger;
	WebserviceDCCSniffer* dccSniffer;
	unsigned long longestLoop;

private:
	typedef std::map<int, LocData*> Items;
	Items items;

	typedef std::map<int, TurnOutData*> LocTurnOuts;
	LocTurnOuts turnoutinfo;

	bool EMERGENCYActive;
	LinkedList<CmdReceiverBase*> receiver = LinkedList<CmdReceiverBase*>();
	LinkedList<CmdSenderBase*> sender = LinkedList<CmdSenderBase*>();
	LinkedList<INotify*> actions = LinkedList<INotify*>();
	LinkedList<ILoop*> loops = LinkedList<ILoop*>();
	LinkedList<ISettings*> settings = LinkedList<ISettings*>();
	LinkedList<IStatus*> status = LinkedList<IStatus*>();
	LinkedList<unsigned long> nextRun = LinkedList<unsigned long>();
	LinkedList<INotify::requestInfo*> requestList = LinkedList<INotify::requestInfo*>();
	long int lastTurnoutCmd[3];
	std::unique_ptr<DNSServer> dnsServer;
	void logLoop(unsigned long d);
//	unsigned int l[100];
//	unsigned long next;
};

#endif /* CONTROLLER_H_ */
