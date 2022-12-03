/*
 * Config.h
 *
 *  Created on: 21.07.2017
 *      Author: sven
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "Controller.h"
#include "ISettings.h"
#include "Connectors.h"
#include "ArduinoJson.h"
#include "WebserverA.h"
#include "json.h"

class Config {
public:
	Config();
	virtual ~Config();
	bool parse(Controller* c, Webserver* web, String filename, boolean dryrun);
private:
	void parseCfg(Controller* c, Webserver* web, String n);
	void parseOut(Controller* c, Webserver* web, String n);
	void parseFilter(Controller* c, Webserver* web, String n);
	void parseIn(Controller* c, Webserver* web, String n);
	void parseConnector(Controller* c, Webserver* web, String n);
	#ifdef ESP32
	void initSD(int gpio);
	#endif 
	ISettings* getSettingById(Controller* c, String id);
	bool lowmemory;
	json* parser;
};

#endif /* CONFIG_H_ */
