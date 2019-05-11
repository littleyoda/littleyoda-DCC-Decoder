// Defines to save heap space
#define NO_GLOBAL_SERIAL1
#include <Arduino.h>
#include "lwip/init.h"
#include "Controller.h"

#include "Logger.h"
#include "Utils.h"
#include "DoubleBootDetection.h"

#include "Config.h"

Controller* controller;

#include "ActionLed.h"
#include "ActionSendTurnoutCommand.h"
#include "ConnectorFunc2Value.h"
#include "ConnectorGPIO.h"
#include "ConnectorLights.h"
#include "ConnectorLocoSpeed.h"
#include "ConnectorONOFF.h"
#include "Connectors.h"
#include "ConnectorTurnout.h"
#include "FS.h"
#ifdef ESP32
	#include "SPIFFS.h"
#endif

void initWifi() {
	Serial.println("Starting Wifi...");
	WiFi.persistent(false);
	#ifdef ESP8266
		WiFi.hostname(controller->getHostname());
	#elif ESP32
		WiFi.setHostname(controller->getHostname().c_str());
	#endif
	WiFi.mode(WIFI_OFF);
	WiFi.disconnect();
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
	if (size > 5024) {
		Logger::getInstance()->addToLog("Konfig-File ist größer als 5024 bytes");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}
	Config* c = new Config();
	bool b = c->parse(controller, web, "/config.json", false);
	delete(c);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " Post Config");
	if (!b) {
		Logger::getInstance()->addToLog(
				"Config-File konnte nicht geparst werden. Fehlerhafter Syntax? Nicht genug Memory?");
		controller->registerLoop(web);
		controller->registerLoop(Logger::getInstance());
		controller->updateRequestList();
		controller->enableAPModus();
		return;
	}
	controller->registerLoop(&GPIOobj);
	controller->registerLoop(web);
	controller->registerLoop(Logger::getInstance());
	controller->updateRequestList();
}


bool scanRunning = false;
char* debugmodus="debug";
int debugmodusPos = 0;
void handleSerial() {
	if (Serial.available() > 0) {
		int chr = Serial.read();
		if (debugmodusPos < 5) {
			if (debugmodus[debugmodusPos] == (char) chr) {
				debugmodusPos++;
				if (debugmodusPos == 5) {
					Serial.println("Debugmodus aktiviert");
				}
			} else {
				debugmodusPos = 0;
			}
		} else if (chr == 'd') {
			// Taken from https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/CheckFlashConfig/CheckFlashConfig.ino
			uint32_t realSize = -1;
			#ifdef ESP8266
			realSize = ESP.getFlashChipRealSize();
			#endif
			uint32_t ideSize = ESP.getFlashChipSize();
			FlashMode_t ideMode = ESP.getFlashChipMode();

			Serial.printf_P(
					PSTR("Version:\r\n"
							"==================\r\n"
							"%s\r\n"
							"LWIP %d.%d.%d.%d\r\n"
							"\r\n"
							"Memory:\r\n"
							"==================\r\n"
							"Free start memory: %d \r\n"
							"Free (Heap)memory: %d \r\n"
							"Free Sketch Space: %d \r\n"
							"\r\n"
							"Logger:\r\n"
							"==================\r\n"
							"Logger-Memory: %d \r\n"
							"\r\n"
							"Wifi:\r\n"
							"==================\r\n"
							"IP: %s/%s\r\n"
					),
					compile_date,
					LWIP_VERSION_MAJOR, LWIP_VERSION_MINOR, LWIP_VERSION_REVISION, LWIP_VERSION_RC,
					Logger::getInstance()->startmemory,
					ESP.getFreeHeap(),
					ESP.getFreeSketchSpace(),
					Logger::getInstance()->getMemUsage(),
					Utils::wifi2String(WiFi.status()).c_str(),
					WiFi.localIP().toString().c_str()
			);
			//			if (controller->cmdlogger != NULL) {
			//				Serial.println("Commandlogger: " + String(controller->cmdlogger->getMemUsage()));
			//			}
			WiFi.printDiag(Serial);
			Serial.printf_P(
					PSTR("%s\r\n"
							"Qualität (RSSI) %d\r\n"
							"Verbunden mit: %s\r\n"
							"\r\n"
							"Flash:\r\n"
							"==================\r\n"
							"Size (real/config): %u %u\r\n"
							"Speed/Mode: %u %s\r\n"
					)
					,WiFi.softAPIP().toString().c_str()
					,WiFi.RSSI()
					,WiFi.BSSIDstr().c_str()
					,realSize, ideSize
					,ESP.getFlashChipSpeed() ,(ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN")
			);

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

		} else if (chr == 'c') {
			File f = SPIFFS.open("/config.json", "r");
			if (f) {
				while (f.available() > 0) {
					Serial.println(f.readStringUntil('\n'));
				}
			} else {
				Serial.println("file open failed");
			}

		} else if (chr == 'x') {
			Serial.println("Debugmodus deaktiviert");
			debugmodusPos = 0;

		} else if (chr == 'm') {
			Serial.printf("Pointer %d\r\n", sizeof(String*));
			Serial.printf("String (without Buffer) %d\r\n", sizeof(String));
			Serial.printf("int %d\r\n", sizeof(int));
			Serial.printf("sint16 %d\r\n", sizeof(int16_t));
			Serial.printf("bool %d\r\n", sizeof(bool));
			Serial.println();
			Serial.printf("RequestInfo %d\r\n", sizeof(INotify::requestInfo));
			Serial.printf("ActionLed %d\r\n", sizeof(ActionLed));
			Serial.printf("ActionSendTurnoutCommand (incl. Inotify, Iloop) %d\r\n", sizeof(ActionSendTurnoutCommand));
			Serial.printf("PIN %d\r\n", sizeof(Pin));
			Serial.printf("ISettings %d\r\n", sizeof(ISettings));
			Serial.printf("INotify %d\r\n", sizeof(INotify));
			Serial.printf("ILoop %d\r\n", sizeof(ILoop));
			Serial.println();
			Serial.printf("ConnectorFunc2Value %d\r\n", sizeof(ConnectorFunc2Value));
			Serial.printf("ConnectorGPIO %d\r\n", sizeof(ConnectorGPIO));
			Serial.printf("ConnectorLights %d\r\n", sizeof(ConnectorLights));
			Serial.printf("ConnectorLocoSpeed %d\r\n", sizeof(ConnectorLocoSpeed));
			Serial.printf("ConnectorONOFF %d\r\n", sizeof(ConnectorONOFF));
			Serial.printf("Connectors %d\r\n", sizeof(Connectors));
			Serial.printf("ConnectorTurnout %d\r\n", sizeof(ConnectorTurnout));
			Serial.println();

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
				bool hidden_scan = false;

				#ifdef ESP8266
					WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
				#elif ESP32
					WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);
				#endif

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
				#ifdef ESP8266
					message += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "OPEN" : "ENC ";
				#elif ESP32
					message += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "ENC ";
				#endif
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
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Setup");
	Logger::getInstance()->addToLog("Started!");
	Logger::getInstance()->addToLog(compile_date);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Controller");
	controller = new Controller();
	controller->registerLoop(new DoubleBootDetection(controller));
	GPIOobj.setController(controller);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Wifi");
	initWifi();
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Webserver");
	Webserver* web = new Webserver(controller);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Serial");
	handleSerial();
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Cfg");
	loadCFG(web);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Setup Finish");
	Serial.println(GPIOobj.getUsage("\r\n"));
	Logger::getInstance()->addToLog("Setup finish!");
	controller->longestLoop = 0;

}

void loop() {
	controller->doLoops();
	handleSerial();
}