#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "Controller.h"
#include "CmdReceiverDCC.h"
#include "CmdReceiverZ21Wlan.h"
#include "CmdZentraleZ21.h"

#include "ActionTurnOut.h"
#include "ActionServo.h"
#include "ActionLed.h"
#include "ActionPWMOutput.h"
#include "ActionDFPlayerMP3.h"
#include "Webserver.h"
#include "WebserviceWifiScanner.h"
#include "WebserviceLog.h"
#include "Logger.h"
#include "Utils.h"
#include "ActionTesting.h"
#include "WebserviceCommandLogger.h"
#include "WebserviceDCCSniffer.h"
#include "DoubleBootDetection.h"
#include "ActionDCCGeneration.h"

Controller* controller;

const char compile_date[] = "Compiledate: " __DATE__ " " __TIME__;
WebserviceCommandLogger* cmdlogger;
WebserviceDCCSniffer* dccSniffer;

void initWifi() {
	Serial.println("Starting Wifi...");
	WiFi.persistent(false);
	WiFi.hostname(controller->getHostname());
	WiFi.mode(WIFI_OFF);
//	WiFi.softAP("Hallo World");
	//	WiFi.mode(WIFI_AP_STA);
//	WiFi.begin(ssid, password);


	//	WiFi.softAP("esp8266_ap", "svensven");
}

/**
 * Auswertung der Configuration (json-Format)
 */
void loadCFG(Webserver* web) {

	File configFile = SPIFFS.open("/config.json", "r");
	if (!configFile) {
		Logger::getInstance()->addToLog("Konnte Konfig-File nicht öffnen: config.json");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}


	size_t size = configFile.size();
	if (size > 1024) {
		Logger::getInstance()->addToLog("Konfig-File ist größer als 1024 bytes");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}
	Serial.println("Starting Parsing");
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);

	Serial.println(buf.get());

	// Replace ' to ""
	int counter = 0;
	while (buf.get()[counter] != '\0') {
		if(buf.get()[counter] == '\'') {
			buf.get()[counter] = '"';
		}
		counter++;
	}


	Logger::getInstance()->addToLog("Config-File: " + String(buf.get()));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(buf.get());

	if (!root.success()) {
		Logger::getInstance()->addToLog(
				"Config-File konnte nicht geparst werden. Fehlerhafter Syntax? Nicht genug Memory?");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}

	// For the Config Section
	//	IPAddress* ip = new IPAddress();
	//	ip->fromString("192.168.2.154");
	//	Logger::getInstance()->setIPAddress(ip);

	JsonArray& r1 = root["cfg"];
	for (JsonArray::iterator it = r1.begin(); it != r1.end(); ++it) {
		JsonObject& value = *it;
		const char* art = (const char*) value["m"];
		if (art == NULL) {
			//Logger::getInstance()->addToLog("Null from json");
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
		} else if ((strcmp(art, "dcclogger") == 0) || (strcmp(art, "cmdlogger") == 0)) {
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
			CmdReceiverZ21Wlan* rec = new CmdReceiverZ21Wlan(controller, value["ip"].as<const char*>());
			controller->registerCmdReceiver(rec);
			controller->registerCmdSender(rec);
		} else if (strcmp(art, "simulateZ21") == 0) {
			CmdZentraleZ21* rec = new CmdZentraleZ21(controller);
			controller->registerCmdReceiver(rec);
		} else if (strcmp(art, "webservicewifiscanner") == 0) {
			web->addServices(new WebserviceWifiScanner());
		} else if (strcmp(art, "webservicelog") == 0) {
			web->addServices(new WebserviceLog());
		} else if (strcmp(art, "pwm") == 0) {
			int gpiopwm = Utils::string2gpio(value["pwm"].as<const char*>());
			int gpiof = Utils::string2gpio(value["forward"].as<const char*>());
			int gpior = Utils::string2gpio(value["reverse"].as<const char*>());
			int  addr = value["addr"].as<int>();
			controller->registerAction(new ActionPWMOutput(addr, gpiopwm, gpiof, gpior));
		} else if (strcmp(art, "mp3") == 0) {
			int  addr = value["addr"].as<int>();
			int tx = Utils::string2gpio(value["tx"].as<const char*>());
			int rx = Utils::string2gpio(value["rx"].as<const char*>());
			controller->registerAction(new ActionDFPlayerMP3(addr, tx, rx));
		} else if (strcmp(art, "wlan") == 0) {
			WiFi.enableSTA(true);
			WiFi.begin(value["ssid"].as<const char*>(), value["pwd"].as<const char*>());
		} else if (strcmp(art, "dccout") == 0) {
			controller->registerAction(new ActionDCCGeneration());
		} else if (strcmp(art, "ap") == 0) {
			IPAddress Ip(192, 168, 0, 111);
			 IPAddress NMask(255, 255, 255, 0);
			  WiFi.softAPConfig(Ip, Ip, NMask);
				WiFi.softAP(value["ssid"].as<const char*>(), value["pwd"].as<const char*>());
			WiFi.enableAP(true);
			Serial.println(WiFi.softAPIP().toString());
			// TODO DNS -Server
		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + String(art));
		}
		loop();
	}
	controller->registerLoop(web);
	controller->registerLoop(Logger::getInstance());
	controller->updateRequestList();
	Logger::getInstance()->addToLog("JSON Parsing finish");
}

void handleSerial() {
	if (Serial.available() > 0) {
		int chr = Serial.read();
		if (chr == 'd') {
			Serial.println("Memory:");
			Serial.println("==================");
			Serial.println("Free start memory: " + String(Logger::getInstance()->startmemory));
			Serial.println("Free memory: " + String(ESP.getFreeHeap()));
			Serial.println("\nLogger:");
			Serial.println("==================");
			Serial.println("Logger: " + String(Logger::getInstance()->getMemUsage()));
			if (dccSniffer != NULL) {
				Serial.println("Sniffer: " + String(dccSniffer->getMemUsage()));
			}
			Serial.println("Commandlogger: " + String(cmdlogger->getMemUsage()));
			Serial.println("\nWifi:");
			Serial.println("==================");
			Serial.println("IP: " + Utils::wifi2String(WiFi.status()) + "  / " + WiFi.localIP().toString());
			WiFi.printDiag(Serial);
			Serial.println(WiFi.softAPIP().toString());

			Serial.println("\nFlash:");
			Serial.println("==================");

			// Taken from https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/CheckFlashConfig/CheckFlashConfig.ino
			uint32_t realSize = ESP.getFlashChipRealSize();
			uint32_t ideSize = ESP.getFlashChipSize();

			Serial.printf("Size (real/config): %u %u", realSize, ideSize);
			Serial.println();
			if(ideSize != realSize) {
				Serial.println("Flash Chip configuration wrong!\n");
			}
			FlashMode_t ideMode = ESP.getFlashChipMode();
			Serial.printf("Speed/Mode: %u %s", ESP.getFlashChipSpeed(), (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
			Serial.println();
		} else if (chr == 'r') {
			ESP.restart();
		} else if (chr == 'D') {
			Serial.println("Configfile deleted");
			SPIFFS.remove("/config.json");
		} else if (chr == 'a') {
			Serial.println("Aktiviere Access Point");
			controller->enableAPModus();
		} else {
			Serial.println("Key: " + String(chr));
		}
	}
}


void setup() {
	Serial.begin(115200);
	Logger::getInstance()->addToLog("Started!");
	Logger::getInstance()->addToLog(compile_date);
	controller = new Controller();
	controller->registerLoop(new DoubleBootDetection(controller));
	initWifi();
	Webserver* web = new Webserver(controller);
	loadCFG(web);
	Logger::getInstance()->addToLog("Setup finish!");

}

void loop() {
	controller->doLoops();
	handleSerial();
}

