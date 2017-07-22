#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <Wire.h>


#include "Controller.h"
#include "CmdReceiverDCC.h"
#include "CmdReceiverZ21Wlan.h"
#include "CmdZentraleZ21.h"


#include "Webserver.h"
#include "WebserviceWifiScanner.h"
#include "WebserviceLog.h"
#include "WebserviceCommandLogger.h"
#include "WebserviceDCCSniffer.h"

#include "Logger.h"
#include "Utils.h"
#include "DoubleBootDetection.h"
#include "Adafruit_MCP23017.h"

#include "Config.h"
#include "Arduino.h"

Controller* controller;


void initWifi() {
	Serial.println("Starting Wifi...");
	WiFi.persistent(false);
	WiFi.hostname(controller->getHostname());
	WiFi.mode(WIFI_OFF);
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
	if (size > 2024) {
		Logger::getInstance()->addToLog("Konfig-File ist größer als 2024 bytes");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}
	if (!Config::parse(controller, web)) {
		Logger::getInstance()->addToLog(
				"Config-File konnte nicht geparst werden. Fehlerhafter Syntax? Nicht genug Memory?");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}
	controller->registerLoop(web);
	controller->registerLoop(Logger::getInstance());
	controller->updateRequestList();
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
			if (controller->dccSniffer != NULL) {
				Serial.println("Sniffer: " + String(controller->dccSniffer->getMemUsage()));
			}
			if (controller->cmdlogger != NULL) {
				Serial.println("Commandlogger: " + String(controller->cmdlogger->getMemUsage()));
			}
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
	handleSerial();
	loadCFG(web);
	Logger::getInstance()->addToLog("Setup finish!");

}

void loop() {
	controller->doLoops();
	handleSerial();
}
