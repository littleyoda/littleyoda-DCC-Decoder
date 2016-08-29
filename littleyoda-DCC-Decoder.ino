#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "Controller.h"
#include "CmdReceiverDCC.h"
#include "CmdReceiverZ21Wlan.h"

#include "ActionTurnOut.h"
#include "ActionServo.h"
#include "ActionLed.h"
#include "Config.h"

#include "Webserver.h"
#include "WebserviceWifiScanner.h"
#include "WebserviceLog.h"
#include "Logger.h"
#include "Utils.h"
#include "Config.h"
Controller* controller;

const char compile_date[] = "Compiledate: " __DATE__ " " __TIME__;

void initWifi() {
	Serial.println("Starting Wifi...");
	WiFi.begin(ssid, password);
	Serial.println("");
}

/**
 * Auswertung der Configuration (json-Format)
 */
void json() {
	//StaticJsonBuffer<2000> jsonBuffer;
	DynamicJsonBuffer jsonBuffer;
	Serial.println("Starting Parsing");
	Serial.println(configuration);
	Logger::getInstance()->addToLog("Config-File: " + String(configuration));
	JsonObject& root = jsonBuffer.parseObject(configuration);

	if (!root.success()) {
		Logger::getInstance()->addToLog(
				"Config-File konnte nicht geparst werden. Fehlerhafter Syntax? Nicht genug Memory?");
		return;
	}

	JsonArray& r1 = root["action"];
	Serial.println(r1.size());
	for (JsonArray::iterator it = r1.begin(); it != r1.end(); ++it) {
		JsonObject& value = *it;
		const char* art = (const char*) value["m"];
		if (strcmp(art, "led") == 0) {
			controller->registerAction(
					new ActionLed(
							Utils::string2gpio(value["gpio"].as<const char*>()),
							value["locoid"].as<int>(),
							value["func"].as<int>()));
		} else if (strcmp(art, "servo") == 0) {
			controller->registerAction(
					new ActionServo(
							Utils::string2gpio(value["gpio"].as<const char*>()),
							value["locoid"].as<int>()));
		} else if (strcmp(art, "turnout") == 0) {
			controller->registerAction(
					new ActionTurnOut(
							Utils::string2gpio(value["dir1"].as<const char*>()),
							Utils::string2gpio(value["dir2"].as<const char*>()),
							Utils::string2gpio(
									value["enable"].as<const char*>()),
							value["addr"].as<int>()));
		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + String(art));
		}
		loop();
	}
}

void setup() {
	Serial.begin(115200);
	Logger::getInstance()->addToLog("Started!");
	Logger::getInstance()->addToLog(compile_date);
	initWifi();

	controller = new Controller();
	json();

	// Webservices
	Webserver* web = new Webserver(controller);
	web->addServices(new WebserviceWifiScanner());
	web->addServices(new WebserviceLog());
	controller->registerLoop(web);

	// Receiver
	controller->registerCmdReceiver(
			new CmdReceiverZ21Wlan(controller, 192, 168, 0, 111));
	loop();
	//controller->registerCmdReceiver(new CmdReceiverDCC(controller, D5, D5));
	loop();

	Logger::getInstance()->addToLog("Setup finish!");
}

void loop() {
	controller->doLoops();
}
