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
#include "ActionTesting.h"
#include "WebserviceCommandLogger.h"
#include "WebserviceDCCSniffer.h"

Controller* controller;

const char compile_date[] = "Compiledate: " __DATE__ " " __TIME__;

WebserviceCommandLogger* cmdlogger;
WebserviceDCCSniffer* dccSniffer;

void initWifi() {
	Serial.println("Starting Wifi...");
	WiFi.begin(ssid, password);
	Serial.println("");
}

/**
 * Auswertung der Configuration (json-Format)
 */
void loadCFG(Webserver* web) {
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
		if (art == NULL) {
			Logger::getInstance()->addToLog("Null from json");
			continue;
		}
		if (strcmp(art, "led") == 0) {
			ActionLed* l = 					new ActionLed(
					Utils::string2gpio(value["gpio"].as<const char*>()),
					value["locoid"].as<int>(),
					value["func"].as<int>());
			const char* pattern = value["pattern"].as<const char*>();
			if (pattern != NULL) {
				l->setPattern(pattern);
			}
			controller->registerAction(l);
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
		} else if (strcmp(art, "dcclogger") == 0) {
			cmdlogger = new WebserviceCommandLogger();
			controller->registerAction(cmdlogger);
			web->addServices(cmdlogger);
		} else if (strcmp(art, "dccsniffer") == 0) {
			dccSniffer = new WebserviceDCCSniffer();
			web->addServices(dccSniffer);
		} else if (strcmp(art, "dcc") == 0) {
			int gpio = Utils::string2gpio(value["gpio"].as<const char*>());
			controller->registerCmdReceiver(new CmdReceiverDCC(controller, gpio, gpio));
		} else if (strcmp(art, "z21") == 0) {
			controller->registerCmdReceiver(new CmdReceiverZ21Wlan(controller, 192, 168, 0, 111));
		} else if (strcmp(art, "webservicewifiscanner") == 0) {
			web->addServices(new WebserviceWifiScanner());
//		} else if (strcmp(art, "mp3") == 0) {
//			controller->registerAction(new ActionDFPlayerMP3(
//					Utils::string2gpio(value["rx"].as<const char*>()),
//					Utils::string2gpio(value["tx"].as<const char*>())
//					));
		} else if (strcmp(art, "webservicelog") == 0) {
			web->addServices(new WebserviceLog());
		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + String(art));
		}
		loop();
	}
	controller->registerLoop(web);
	Logger::getInstance()->addToLog("JSON Parsing finish");
}

void setup() {
	Serial.begin(115200);
	Logger::getInstance()->addToLog("Started!");
	Logger::getInstance()->addToLog(compile_date);
	initWifi();
	delay(3000);

	controller = new Controller();
	Webserver* web = new Webserver(controller);
	loadCFG(web);

	Logger::getInstance()->addToLog("Setup finish!");
}

void loop() {
	controller->doLoops();
	if (Serial.available() > 0) {
		int chr = Serial.read();
		if (chr == 'd') {
			Serial.println("IP: " + Utils::wifi2String(WiFi.status()) + "  / " + WiFi.localIP().toString());
			Serial.println("Free start memory: " + String(Logger::getInstance()->startmemory));
			Serial.println("Free memory: " + String(ESP.getFreeHeap()));
			Serial.println("Logger: " + String(Logger::getInstance()->getMemUsage()));
			Serial.println("Sniffer: " + String(dccSniffer->getMemUsage()));
			Serial.println("Commandlogger: " + String(cmdlogger->getMemUsage()));
		} else if (chr == 'r') {
			ESP.restart();
		} else if (chr == '#') {
			for (int i = 0; i < 20; i++) {
				Logger::getInstance()->addToLog(String("Hallo World ") + String(millis() + String("log")));
			}
		} else if (chr == '+') {
			for (int i = 0; i < 20; i++) {
				WebserviceDCCSniffer::_instance->addToLog(String("Hallo World ") + String(millis() + String(" DCC")));
			}
		} else {
			Serial.println("Key: " + String(chr));
		}
	}
}
