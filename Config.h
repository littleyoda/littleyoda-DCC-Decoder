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
#include "Webserver.h"

#define JSMN_STRICT
#include "jsmn.h"

class Config {
public:
	Config();
	virtual ~Config();
	static boolean parse(Controller* c, Webserver* web, String filename, boolean dryrun);
private:
	static void parseCfg(Controller* c, Webserver* web, JsonArray& r1);
	static void parseOut(Controller* c, Webserver* web, JsonArray& r1);
	static void parseIn(Controller* c, Webserver* web, JsonArray& r1);
	static void parseConnector(Controller* c, Webserver* web, JsonArray& r1);
	static ISettings* getSettingById(Controller* c, const char* id);
	static boolean lowmemory;
	static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
};
#endif /* CONFIG_H_ */
