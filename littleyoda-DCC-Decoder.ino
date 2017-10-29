#include <Arduino.h>

#include "Controller.h"

#include "Logger.h"
#include "Utils.h"
#include "DoubleBootDetection.h"

#include "Config.h"

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


bool scanRunning = false;
void handleSerial() {
	if (Serial.available() > 0) {
		int chr = Serial.read();
		if (chr == 'd') {
			Serial.println("Memory:");
			Serial.println("==================");
			Serial.println("Free start memory: " + String(Logger::getInstance()->startmemory));
			Serial.println("Free memory: " + String(ESP.getFreeHeap()));
			Serial.println("\r\nLogger:");
			Serial.println("==================");
			Serial.println("Logger: " + String(Logger::getInstance()->getMemUsage()));
			if (controller->dccSniffer != NULL) {
				//Serial.println("Sniffer: " + String(controller->dccSniffer->getMemUsage()));
			}
			if (controller->cmdlogger != NULL) {
				Serial.println("Commandlogger: " + String(controller->cmdlogger->getMemUsage()));
			}
			Serial.println("\r\nWifi:");
			Serial.println("==================");
			Serial.println("IP: " + Utils::wifi2String(WiFi.status()) + "  / " + WiFi.localIP().toString());
			WiFi.printDiag(Serial);
			Serial.println(WiFi.softAPIP().toString());
			Serial.println("Qualität (RSSI): " + String(WiFi.RSSI()));
			Serial.printf("Verbunden mit: %s\r\n", WiFi.BSSIDstr().c_str());

			Serial.println("\r\nFlash:");
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
			Serial.println("\n==================");
			Serial.println("Pin-Nutzung:");
			Serial.println(GPIO.getUsage("\r\n"));
		} else if (chr == 'R') {
			ESP.restart();
		} else if (chr == 'D') {
			Serial.println("Configfile deleted");
			SPIFFS.remove("/config.json");
		} else if (chr == 'a') {
			controller->enableAPModus();
		} else if (chr == 'r') {
			WiFi.reconnect();
		} else if (chr == 'w') {
			Serial.println("Scan Started");
			WiFi.scanNetworks(true, true);
			scanRunning = true;
		} else {
			Serial.println("Key: " + String(chr));
		}
	}
	if (scanRunning) {
		if (WiFi.scanComplete() == WIFI_SCAN_FAILED) {
			Serial.println("Scan Failed!");
			scanRunning = false;
		}
		if (WiFi.scanComplete() >= 0) {
			int n = WiFi.scanComplete();
			for (int i = 0; i < n; ++i) {
				String message = "";
				String ssid_scan;
				int32_t rssi_scan;
				uint8_t sec_scan;
				uint8_t* BSSID_scan;
				int32_t chan_scan;
				bool hidden_scan;

				WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
				message +="[";
				for (int i = 0; i < 6; i++) {
					if (BSSID_scan[i] < 16) {
						message += "0";
					}
					message += String(BSSID_scan[i], HEX);
					if (i < 5) {
						message +=":";
					}

				}
				message +="] CH:";
				if (chan_scan < 10) {
					message += "0";
				}
				message += String(chan_scan);
				message += " ";
				message += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "OPEN" : "ENC ";
				message += "  Q:";
				message += String(WiFi.RSSI(i));
				message += "  --- ";
				message += ssid_scan;
				message += "  ";
				if (hidden_scan) {
					message += " (HIDDEN)";
				}
				message += "\r\n";
				Serial.print(message);
			}
			Serial.printf("BSSID: %s\r\n", WiFi.BSSIDstr().c_str());
			WiFi.scanDelete();
			scanRunning = false;
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
	Serial.println(GPIO.getUsage("\r\n"));
	Logger::getInstance()->addToLog("Setup finish!");

}

void loop() {
	controller->doLoops();
	handleSerial();
}
