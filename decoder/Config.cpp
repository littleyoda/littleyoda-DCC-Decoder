/*
 * Config.cpp
 *
 *  Created on: 21.07.2017
 *      Author: sven
 */
#include "Arduino.h"
#include "Config.h"
#include "FS.h"
#include "Logger.h"
#include "ArduinoJson.h"
#include "GPIO.h"
#include "ActionTurnOut.h"
#include "ActionServo.h"
#include "ActionLed.h"
#include "ActionPWMOutput.h"
#include "ActionDFPlayerMP3.h"
#include "ActionDCCGeneration.h"
#include "ActionSUSIGeneration.h"
#include "ActionSendTurnoutCommand.h"
#include "ActionStepperOutput.h"

#include "Connectors.h"
#include "ConnectorLocoSpeed.h"
#include "ConnectorONOFF.h"
#include "ConnectorFunc2Value.h"
#include "ConnectorTurnout2Value.h"
#include "ConnectorTurnout2Value.h"
#include "ConnectorTurnout.h"
#include "ConnectorLights.h"
#include "ConnectorGPIO.h"
#include "Config.h"

#include "CmdReceiverDCC.h"
#include "CmdZentraleZ21.h"
#include "CmdReceiverZ21Wlan.h"
#include "CmdReceiverRocnetOverMQTT.h"
//#include "CmdReceiverESPNOW.h"

#include "WebserviceCommandLogger.h"
#include "WebserviceLog.h"
#include "WebserviceWifiScanner.h"

#include "ISettings.h"

#ifdef ESP32
	#include "FS.h"
	#include "SPIFFS.h"
#endif


Config::Config() {
	lowmemory = false;
	parser = 0;
}

Config::~Config() {
}


bool Config::parse(Controller* controller, Webserver* web, String filename, boolean dryrun) {
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Cfg Read");
	lowmemory = false;
	File configFile = SPIFFS.open(filename, "r");

	parser = new json(configFile);

	String version = parser->getValueByKey(0, "version");
	if (!version.equals("3")) {
		Logger::getInstance()->addToLog("Ungültige Version des Konfig-Files: " + version);
		return false;
	}
	if (dryrun) {
		delete(parser);
		return true;
	}

	parseCfg(controller, web, "cfg");
	parseOut(controller, web, "out");
	parseIn(controller, web, "in");
	parseConnector(controller, web, "connector");

	delete(parser);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Cfg-End");
	return true;
}


void Config::parseOut(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog("Out-Sektion leer oder fehlerhaft!");
		return;
	}
	while (idx != -1) {
		if (ESP.getFreeHeap() < 1200) {
			lowmemory = true;
			break;
		}
		String m = parser->getValueByKey(idx, "m");
		Serial.println("MEM "  + String(ESP.getFreeHeap()) + " " + m);
		#ifdef ESP8266

		if (m.equals("dccout")) {
			Pin* gpioenable = new Pin(parser->getValueByKey(idx, "enable"));
			int locoaddr = parser->getValueByKey(idx, "addr").toInt();
			int dccoutput = parser->getValueByKey(idx, "dccoutputaddr").toInt();
			ActionDCCGeneration* a = new ActionDCCGeneration(gpioenable, locoaddr, dccoutput);
			controller->registerNotify(a);
			controller->registerLoop(a);
			idx = parser->getNextSiblings(idx);
			continue;
		}

		if (m.equals("susiout")) {
			int locoaddr = parser->getValueByKey(idx, "addr").toInt();
			ActionSUSIGeneration* a = new ActionSUSIGeneration(locoaddr);
			controller->registerNotify(a);
			controller->registerLoop(a);
			idx = parser->getNextSiblings(idx);
			continue;
		}

		#endif

		String id = parser->getValueByKey(idx, "id");
		if (id.length() == 0) {
			Logger::getInstance()->addToLog("ID is null");
			idx = parser->getNextSiblings(idx);
			continue;
		}
		if (m.equals("led")) {
			Pin* ledgpio = new Pin(parser->getValueByKey(idx, "gpio"));
			ActionLed* l = new ActionLed(ledgpio);
			l->setName(id);
			controller->registerSettings(l);

		} else if (m.equals("pwm")) {
			int gpiopwm = GPIOobj.string2gpio(parser->getValueByKey(idx, "pwm"));
			int gpiof = GPIOobj.string2gpio(parser->getValueByKey(idx, "forward"));
			int gpior = GPIOobj.string2gpio(parser->getValueByKey(idx, "reverse"));
			ISettings* a = new ActionPWMOutput(gpiopwm, gpiof, gpior);
			a->setName(id);
			controller->registerSettings(a);


		} else if (m.equals("servo")) {
			#ifdef ESP8266

			int gpioservo = GPIOobj.string2gpio(parser->getValueByKey(idx, "gpio"));
			ActionServo* a = new ActionServo(gpioservo);
			a->setName(id);
			controller->registerSettings(a);
			controller->registerLoop(a);
            #endif

		} else if (m.equals("turnout")) {
			ActionTurnOut* a = new ActionTurnOut(
					GPIOobj.string2gpio(parser->getValueByKey(idx, "dir1")),
					GPIOobj.string2gpio(parser->getValueByKey(idx, "dir2")),
					GPIOobj.string2gpio(parser->getValueByKey(idx, "enable")));
			a->setName(id);
			controller->registerSettings(a);
			controller->registerLoop(a);

		} else if (m.equals("sendturnout")) {
			int addr = parser->getValueByKey(idx, "addr").toInt();
			ActionSendTurnoutCommand* a = new ActionSendTurnoutCommand(controller, addr);
			a->setName(id);
			controller->registerSettings(a);
			controller->registerNotify(a);

		} else if (m.equals("stepper")) {
			int child = parser->getIdxByKey(idx, "gpio");
			child = parser->getFirstChild(child);
			ActionStepperOutput* a = new ActionStepperOutput(
								new Pin(parser->getString(parser->getChildAt(child, 0))),
								new Pin(parser->getString(parser->getChildAt(child, 1))),
								new Pin(parser->getString(parser->getChildAt(child, 2))),
								new Pin(parser->getString(parser->getChildAt(child, 3)))
			);
			a->setName(id);
			controller->registerSettings(a);
			controller->registerLoop(a);

		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + m);
		}
		idx = parser->getNextSiblings(idx);
		loop();
	}
}



void Config::parseCfg(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog("CFG-Sektion leer oder fehlerhaft!");
		return;
	}
	while (idx != -1) {
		if (ESP.getFreeHeap() < 1200) {
			lowmemory = true;
			break;
		}
		String m = parser->getValueByKey(idx, "m");
		Serial.println("MEM "  + String(ESP.getFreeHeap()) + " " + m);
		if (m.equals("dcclogger") || m.equals("cmdlogger")) {
			controller->cmdlogger = new WebserviceCommandLogger();
			controller->registerNotify(controller->cmdlogger);
			web->registerWebServices(controller->cmdlogger);
		} else if (m.equals("dccsniffer")) {
			controller->dccSniffer = new WebserviceDCCSniffer();
			web->registerWebServices(controller->dccSniffer);


		} else if (m.equals("dcc")) {
			int gpio = GPIOobj.string2gpio(parser->getValueByKey(idx, "gpio"));
			controller->registerCmdReceiver(new CmdReceiverDCC(controller, gpio, gpio));


		} else if (m.equals("z21")) {
			CmdReceiverZ21Wlan* rec = new CmdReceiverZ21Wlan(controller, parser->getValueByKey(idx, "ip"));
			controller->registerCmdReceiver(rec);
			controller->registerCmdSender(rec);


		} else if (m.equals("simulateZ21")) {
			CmdZentraleZ21* rec = new CmdZentraleZ21(controller);
			controller->registerCmdReceiver(rec);


		} else if (m.equals("espnow")) {
			//			String rolle = String(value["rolle"].as<const char*>());
			//			Serial.println("Rolle: " + rolle);
			//			CmdReceiverESPNOW* rec = new CmdReceiverESPNOW(controller, rolle);
			//			controller->registerCmdReceiver(rec);
		} else if (m.equals("rocnetovermqtt")) {
			controller->registerCmdReceiver(new CmdReceiverRocnetOverMQTT(controller));

		} else if (m.equals("webservicewifiscanner")) {
			web->registerWebServices(new WebserviceWifiScanner());


		} else if (m.equals("webservicelog")) {
			web->registerWebServices(new WebserviceLog());

		} else if (m.equals("mp3")) {
			//			int  addr = value["addr").toInt();
			//			int tx = GPIOobj.string2gpio(value["tx"].as<const char*>());
			//			int rx = GPIOobj.string2gpio(value["rx"].as<const char*>());
			//			controller->registerNotify(new ActionDFPlayerMP3(addr, tx, rx));

		} else if (m.equals("wlan")) {
			WiFi.enableSTA(true);
			String ip = parser->getValueByKey(idx, "ip");
			String netmask = parser->getValueByKey(idx, "netmask");
			String gw = parser->getValueByKey(idx, "gw");
			String ssid = parser->getValueByKey(idx, "ssid");
			String pwd = parser->getValueByKey(idx, "pwd");
			if (ip.length() != 0 || netmask.length() != 0 || gw.length() != 0) {
				if (ip.length() > 0 &&  netmask.length() > 0 && gw.length() > 0) {
					IPAddress ipx;
					IPAddress nmx;
					IPAddress gwx;
					ipx.fromString(ip);
					nmx.fromString(netmask);
					gwx.fromString(gw);
					WiFi.config(ipx, gwx, nmx);
				} else {
					Logger::log("Netzwerkkonfiguration (ip, netmask, gw) unvollständig");
					idx = parser->getNextSiblings(idx);
					continue;
				}
			} else {
				Logger::log("Netzwerkkonfiguration per DHCP");
			}
			int ch = 1;
			String kanal = parser->getValueByKey(idx, "kanal");
			if (kanal.length() > 0) {
				ch = kanal.toInt();
			}
			WiFi.begin(ssid.c_str(), pwd.c_str(), ch);
		} else if (m.equals("ap")) {
			IPAddress Ip(192, 168, 0, 111);
			IPAddress NMask(255, 255, 255, 0);
			int ch = 1;
			String kanal = parser->getValueByKey(idx, "kanal");
			if (kanal.length() > 0) {
				ch = kanal.toInt();
			}
			String ssid = parser->getValueByKey(idx, "ssid");
			String pwd = parser->getValueByKey(idx, "pwd");
			WiFi.softAPConfig(Ip, Ip, NMask);
			if (!WiFi.softAP(ssid.c_str(), pwd.c_str(), ch)) {
				Logger::log("softAP fehlgeschlagen!");
			}
			WiFi.enableAP(true);
			Serial.println("AP-IP: " + WiFi.softAPIP().toString());
			// TODO DNS -Server


		} else if (m.equals("i2c")) {
			int sda = GPIOobj.string2gpio(parser->getValueByKey(idx, "sda"));
			int scl = GPIOobj.string2gpio(parser->getValueByKey(idx, "scl"));
			Wire.begin(sda, scl);


		} else if (m.equals("i2cslave")) {
			String d = parser->getValueByKey(idx, "d");
			if (d.equalsIgnoreCase("mcp23017")) {
				int addridx = parser->getIdxByKey(idx, "addr");
				addridx = parser->getFirstChild(addridx);
				if (!parser->isArray(addridx)) {
					Logger::log("Format für MCP23017 Adressen falsch!");
					idx = parser->getNextSiblings(idx);
					continue;

				}
				int child = parser->getFirstChild(addridx);
				while (child != -1) {
					int addr = parser->getString(child).toInt();
					Wire.beginTransmission(addr + 0x20);
					int ret = Wire.endTransmission();
					String tret = "Failed (" + String(ret) + ")";
					if (ret == 0) {
						tret = "OK";
					}
					Logger::getInstance()->addToLog("Test MCP23017 auf I2c/" + String(addr + 0x20) + ": " + tret);
					if (ret == 0) {
						GPIOobj.addMCP23017(addr);
					}
					child = parser->getNextSiblings(child);
				}
			} else {
				Logger::getInstance()->addToLog("Unbekanntes Gerät (I2C): " + String(d));
			}


		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + m);
		}

		loop();
		idx = parser->getNextSiblings(idx);

	}

}

void Config::parseConnector(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog("Connector-Sektion leer oder fehlerhaft!");
		return;
	}
	while (idx != -1) {
		String in = parser->getValueByKey(idx, "in");
		String out = parser->getValueByKey(idx, "out");
		Serial.println("MEM "  + String(ESP.getFreeHeap()) + " " + in + "/" + out);
		if (in.equals("turnout") && out.equals("led")) {
			Connectors* cin;
			int addridx = parser->getFirstChild(parser->getIdxByKey(idx, "values"));
			if (!parser->isArray(addridx)) {
				Logger::log("Format falsch!");
				idx = parser->getNextSiblings(idx);
				continue;

			}
			int child = parser->getFirstChild(addridx);
			while (child != -1) {
				if (ESP.getFreeHeap() < 1200) {
					lowmemory = true;
					break;
				}
				int idx = parser->getFirstChild(child);
				int addr = parser->getString(idx).toInt();
				String pin = parser->getString(parser->getNextSiblings(idx));

//				Logger::getInstance()->addToLog("Turnout/Led " + String(addr) + "/" + String(pin));

				Pin* ledgpio = new Pin(pin);
				ActionLed* l = new ActionLed(ledgpio);
				controller->registerSettings(l);
				cin = new ConnectorTurnout(l, addr);
				controller->registerNotify(cin);

				child = parser->getNextSiblings(child);
			}
		} else if (in.equals("gpio") && out.equals("sendturnout")) {
			Connectors* cin;
			int child = parser->getFirstChildOfArrayByKey(idx, "values");
			if (child < 0) {
				Logger::log("Format falsch!");
				idx = parser->getNextSiblings(idx);
				continue;

			}
			while (child != -1) {
				if (ESP.getFreeHeap() < 1200) {
					lowmemory = true;
					break;
				}
				int idx = parser->getFirstChild(child);
				String pin = parser->getString(idx);
				int addr = parser->getString(parser->getNextSiblings(idx)).toInt();
//				Logger::getInstance()->addToLog("GPIO/Sendturnout " + String(pin) + "/" + String(addr));

				ActionSendTurnoutCommand* atc = new ActionSendTurnoutCommand(controller, addr);
				controller->registerSettings(atc);
				controller->registerNotify(atc);

				Pin* g = new Pin(pin);
				cin = new ConnectorGPIO(atc, g);
				controller->registerNotify(cin);

				child = parser->getNextSiblings(child);
			}
		} else if (in.equals("direction") && out.equals("leds")) {

			int addr = parser->getValueByKey(idx, "addr").toInt();
			int func = parser->getValueByKey(idx, "func").toInt();
			int direction = parser->getValueByKey(idx, "direction").toInt();
			Connectors* cin;
			int child = parser->getFirstChildOfArrayByKey(idx, "gpio");
			if (child < 0) {
				Logger::log("Format falsch! GPIO Array nicht gefunden!");
				idx = parser->getNextSiblings(idx);
				continue;
			}
			while (child != -1) {
				if (ESP.getFreeHeap() < 1200) {
					lowmemory = true;
					break;
				}
				String pin = parser->getString(child);

				Logger::log("d=>l Pin:" + String(pin) + " Addr:" + String(addr) + " Func:" + String(func));
				ActionLed* g = new ActionLed(new Pin(pin));
				cin = new ConnectorLights(g, addr, func, direction);
				controller->registerNotify(cin);
				child = parser->getNextSiblings(child);
			}
		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag In: " + String(in) + " Out: " + String(out));
		}
		loop();
		idx = parser->getNextSiblings(idx);
	}
}


void Config::parseIn(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog("In-Sektion leer oder fehlerhaft!");
		return;
	}
	while (idx != -1) {
		String m = parser->getValueByKey(idx, "m");
		Serial.println("S: " +  m);
		Connectors* c = NULL;
		if (m.equals("locospeed")) {
			int l = parser->getValueByKey(idx, "addr").toInt();
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorLocoSpeed(a, l);

		} else  if (m.equals("funconoff")) {
			int l = parser->getValueByKey(idx, "addr").toInt();
			int f = parser->getValueByKey(idx, "func").toInt();
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorONOFF(a, l, f);

		} else if (m.equals("func2value")) {
			/**
                        "m": "func2value",
                        "addr": "4711",
                        "out": [ "servo1" ],
                        "func2value": { "3": "0", "4": "50", "5": "120"
                        }
			 */
			int addr = parser->getValueByKey(idx, "addr").toInt();
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));

			int child = parser->getIdxByKey(idx, "func2value");
			child = parser->getFirstChild(child);
			child = parser->getFirstChild(child);
			int arraysize = 2 * parser->getNumberOfSiblings(child);
			int *array = new int[arraysize];
			int pos = 0;
			while (child != -1) {
				child = parser->getNextSiblings(child);
				array[pos++] = parser->getString(child).toInt();
				array[pos++] = parser->getString(parser->getFirstChild(child)).toInt();
			}

			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorFunc2Value(a, addr, array, arraysize);
		} else if (m.equals("turnout2value")) {
			/**
                        "m": "turnout2value",
                        "addr": "4711",
                        "out": [ "servo1" ],
                        "turnout2value": { "0": "0", "1": "50"}
                        }
			 */
			int addr = parser->getValueByKey(idx, "addr").toInt();
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));

			int child = parser->getIdxByKey(idx, "turnout2value");
			child = parser->getFirstChild(child);
			child = parser->getFirstChild(child);
			int arraysize = 2 * parser->getNumberOfSiblings(child);
			int *array = new int[arraysize];
			int pos = 0;
			while (child != -1) {
				child = parser->getNextSiblings(child);
				array[pos++] = parser->getString(child).toInt();
				array[pos++] = parser->getString(parser->getFirstChild(child)).toInt();
			}

			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorTurnout2Value(a, addr, array, arraysize);
		} else if (m.equals("turnout")) {
			int addr = parser->getValueByKey(idx, "addr").toInt();
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorTurnout(a, addr);

		} else if (m.equals("lights")) {
			Logger::getInstance()->addToLog(
					"Zur Zeit nicht implementiert " + m);
			//	TODO		int l = parser->getValueByKey(idx, "addr").toInt();
			//			int onoff = parser->getValueByKey(idx, "on").toInt();
			//
			//			JsonArray& out = parser->getValueByKey(idx, "out"];
			//			if (out.size() != 2) {
			//				Logger::log("Keine zwei Ausgabe Led angegeben.");
			//				continue;
			//			}
			//			ISettings *ptr[4];
			//			for (int i = 0; i < 2; i++) {
			//				ptr[i] = getSettingById(controller, parser->getValueByKey(idx, "out"][i].as<const char*>());
			//			}
			//			Serial.println("Lights: " + String(onoff) + " " + " Addr: " + String(l));
			//			c = new ConnectorLights(ptr[0], ptr[1], l, onoff);

		} else if (m.equals("gpio")) {
			Pin* g = new Pin(parser->getValueByKey(idx, "gpio"));
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorGPIO(a, g);

		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + m);
		}
		if (c != NULL) {
			controller->registerNotify(c);
		}
		loop();
		idx = parser->getNextSiblings(idx);

	}
}

ISettings* Config::getSettingById(Controller* c, String id) {
	for (int idx = 0; idx < c->getSettings()->size(); idx++) {
		ISettings* s = c->getSettings()->get(idx);
		if (s->getName().equals(id)) {
			return s;
		}
	}
	Logger::getInstance()->addToLog("Config: Unbekannte ID " + String(id));
	return NULL;
}

