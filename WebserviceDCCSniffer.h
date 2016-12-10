/*
 * WebserviceDCCSniffer.h
 *
 *  Created on: 08.09.2016
 *      Author: sven
 */

#ifndef WEBSERVICEDCCSNIFFER_H_
#define WEBSERVICEDCCSNIFFER_H_

#include "WebserviceBase.h"
#include "LinkedList.h"

class WebserviceDCCSniffer : public WebserviceBase {
public:
	WebserviceDCCSniffer();
	virtual ~WebserviceDCCSniffer();
	static WebserviceDCCSniffer* _instance; // Hack for access from NmraDCC

	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);

	virtual char const* getUri();
	virtual void run();
	virtual String  getLinkText();
	 void addToLog(String s);
		unsigned int getMemUsage();
		int maxLog = 50;

private:
	LinkedList<String> logger = LinkedList<String>();

};

#endif /* WEBSERVICEDCCSNIFFER_H_ */
