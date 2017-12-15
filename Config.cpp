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

#include "Connectors.h"
#include "ConnectorLocoSpeed.h"
#include "ConnectorONOFF.h"
#include "ConnectorFunc2Value.h"
#include "ConnectorTurnout.h"
#include "ConnectorLights.h"
#include "Config.h"

#include "CmdReceiverDCC.h";
#include "CmdZentraleZ21.h";
#include "CmdReceiverZ21Wlan.h"

#include "WebserviceCommandLogger.h"
#include "WebserviceLog.h";
#include "WebserviceWifiScanner.h";

#include "ISettings.h"

Config::Config() {
}

Config::~Config() {
}

boolean Config::parse(Controller* controller, Webserver* web) {
	File configFile = SPIFFS.open("/config.json", "r");
	size_t size = configFile.size();

	Serial.println("Starting Parsing");
	std::unique_ptr<char[]> buf(new char[size]);
	configFile.readBytes(buf.get(), size);


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
		return false;
	}
	int version = root["version"].as<int>();
	if (version != 3) {
		Logger::getInstance()->addToLog("Ungültige Version: " + String(version));
		return false;
	}

	JsonArray& cfg = root["cfg"];
	parseCfg(controller, web, cfg);

	JsonArray& out = root["out"];
	parseOut(controller, web, out);

	JsonArray& in = root["in"];
	parseIn(controller, web, in);

	Logger::getInstance()->addToLog("JSON Parsing finish");

	return true;
}

void Config::parseOut(Controller* controller, Webserver* web, JsonArray& r1) {
	for (JsonArray::iterator it = r1.begin(); it != r1.end(); ++it) {
		JsonObject& value = *it;
		const char* art = (const char*) value["m"];
		if (art == NULL) {
			//Logger::getInstance()->addToLog("Null from json");
			continue;
		}
		if (strcmp(art, "dccout") == 0) {
			Pin* gpioenable = new Pin(value["enable"].as<const char*>());
			int locoaddr = value["addr"].as<int>();
			int dccoutput = value["dccoutputaddr"].as<int>();
			ActionDCCGeneration* a = new ActionDCCGeneration(gpioenable, locoaddr, dccoutput);
			controller->registerNotify(a);
			controller->registerLoop(a);
			continue;
		}

		if (strcmp(art, "susiout") == 0) {
			int locoaddr = value["addr"].as<int>();
			ActionSUSIGeneration* a = new ActionSUSIGeneration(locoaddr);
			controller->registerNotify(a);
			controller->registerLoop(a);
			continue;
		}

		const char* id = (const char*) value["id"];
		if (id == NULL) {
			Logger::getInstance()->addToLog("ID is null");
			continue;
		}
		if (strcmp(art, "led") == 0) {
			Pin* ledgpio = new Pin(value["gpio"].as<const char*>());
			ActionLed* l = new ActionLed(ledgpio);
			l->setName(id);
			controller->registerSettings(l);
			controller->registerLoop(l);
		} else if (strcmp(art, "pwm") == 0) {
			int gpiopwm = GPIO.string2gpio(value["pwm"].as<const char*>());
			int gpiof = GPIO.string2gpio(value["forward"].as<const char*>());
			int gpior = GPIO.string2gpio(value["reverse"].as<const char*>());
			ISettings* a = new ActionPWMOutput(gpiopwm, gpiof, gpior);
			a->setName(id);
			controller->registerSettings(a);


		} else if (strcmp(art, "servo") == 0) {
			int gpioservo = GPIO.string2gpio(value["gpio"].as<const char*>());
			ActionServo* a = new ActionServo(gpioservo);
			a->setName(id);
			controller->registerSettings(a);
			controller->registerLoop(a);

		} else if (strcmp(art, "turnout") == 0) {
			ActionTurnOut* a = new ActionTurnOut(
					GPIO.string2gpio(value["dir1"].as<const char*>()),
					GPIO.string2gpio(value["dir2"].as<const char*>()),
					GPIO.string2gpio(value["enable"].as<const char*>()));
			a->setName(id);
			controller->registerSettings(a);
			controller->registerLoop(a);

		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + String(art));
		}
		loop();

	}
}



void Config::parseCfg(Controller* controller, Webserver* web, JsonArray& r1) {
	for (auto value : r1) {
		const char* art = (const char*) value["m"];
		if (art == NULL) {
			//Logger::getInstance()->addToLog("Null from json");
			continue;
		}
		if ((strcmp(art, "dcclogger") == 0) || (strcmp(art, "cmdlogger") == 0)) {
			controller->cmdlogger = new WebserviceCommandLogger();
			controller->registerNotify(controller->cmdlogger);
			web->registerWebServices(controller->cmdlogger);


		} else if (strcmp(art, "dccsniffer") == 0) {
			controller->dccSniffer = new WebserviceDCCSniffer();
			web->registerWebServices(controller->dccSniffer);


		} else if (strcmp(art, "dcc") == 0) {
			int gpio = GPIO.string2gpio(value["gpio"].as<const char*>());
			controller->registerCmdReceiver(new CmdReceiverDCC(controller, gpio, gpio));


		} else if (strcmp(art, "z21") == 0) {
			CmdReceiverZ21Wlan* rec = new CmdReceiverZ21Wlan(controller, value["ip"].as<const char*>());
			controller->registerCmdReceiver(rec);
			controller->registerCmdSender(rec);


		} else if (strcmp(art, "simulateZ21") == 0) {
			CmdZentraleZ21* rec = new CmdZentraleZ21(controller);
			controller->registerCmdReceiver(rec);


		} else if (strcmp(art, "webservicewifiscanner") == 0) {
			web->registerWebServices(new WebserviceWifiScanner());


		} else if (strcmp(art, "webservicelog") == 0) {
			web->registerWebServices(new WebserviceLog());


			//		} else if (strcmp(art, "mp3") == 0) {
			////			int  addr = value["addr"].as<int>();
			////			int tx = GPIO.string2gpio(value["tx"].as<const char*>());
			////			int rx = GPIO.string2gpio(value["rx"].as<const char*>());
			////			controller->registerNotify(new ActionDFPlayerMP3(addr, tx, rx));

		} else if (strcmp(art, "wlan") == 0) {
			WiFi.enableSTA(true);
			if (value["ip"] != NULL || value["netmask"] != NULL || value["gw"] != NULL) {
				if (value["ip"] != NULL && value["netmask"] != NULL && value["gw"] != NULL) {
					IPAddress ip;
					IPAddress nm;
					IPAddress gw;
					ip.fromString(value["ip"].as<const char*>());
					nm.fromString(value["netmask"].as<const char*>());
					gw.fromString(value["gw"].as<const char*>());
					WiFi.config(ip, gw, nm);
				} else {
					Logger::log("Netzwerkkonfiguration (ip, netmask, gw) unvollständig");
					continue;
				}
			} else {
				Logger::log("Netzwerkkonfiguration per DHCP");
			}
			int ch = 1;
			if (value["kanal"] != NULL) {
				ch = value["kanal"].as<int>();
			}
			WiFi.begin(value["ssid"].as<const char*>(), value["pwd"].as<const char*>(), ch);


		} else if (strcmp(art, "ap") == 0) {
			IPAddress Ip(192, 168, 0, 111);
			IPAddress NMask(255, 255, 255, 0);
			int ch = 1;
			if (value["kanal"] != NULL) {
				ch = value["kanal"].as<int>();
			}
			WiFi.softAPConfig(Ip, Ip, NMask);
			if (!WiFi.softAP(value["ssid"].as<const char*>(), value["pwd"].as<const char*>(), ch)) {
				Logger::log("softAP fehlgeschlagen!");
			}
			WiFi.enableAP(true);
			Serial.println(WiFi.softAPIP().toString());
			// TODO DNS -Server


		} else if (strcmp(art, "i2c") == 0) {
			int sda = GPIO.string2gpio(value["sda"].as<const char*>());
			int scl = GPIO.string2gpio(value["scl"].as<const char*>());
			Wire.begin(sda, scl);


		} else if (strcmp(art, "i2cslave") == 0) {
			const char* d = (const char*) value["d"];
			if (d != NULL && strcmp(d, "MCP23017") == 0) {
				Wire.beginTransmission(0x20);
				int ret = Wire.endTransmission();
				String tret = "Failed (" + String(ret) + ")";
				if (ret == 0) {
					tret = "OK";
				}
				Logger::getInstance()->addToLog("Test MCP23017 auf I2c/0x20: " + tret);
				if (ret == 0) {
					GPIO.enableMCP23017(0);
				}
			} else {
				Logger::getInstance()->addToLog("Unbekanntes Gerät: " + String(d));
			}


		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + String(art));
		}
		loop();
	}
}


void Config::parseIn(Controller* controller, Webserver* web, JsonArray& r1) {
	for (JsonArray::iterator it = r1.begin(); it != r1.end(); ++it) {
		JsonObject& value = *it;
		const char* art = (const char*) value["m"];
		if (art == NULL) {
			//Logger::getInstance()->addToLog("Null from json");
			continue;
		}
		Connectors* c;
		if (strcmp(art, "locospeed") == 0) {
			int l = value["addr"].as<int>();
			ISettings* a = getSettingById(controller, value["out"][0].as<const char*>());
			c = new ConnectorLocoSpeed(a, l);

		} else if (strcmp(art, "funconoff") == 0) {
			int l = value["addr"].as<int>();
			int f = value["func"].as<int>();
			ISettings* a = getSettingById(controller, value["out"][0].as<const char*>());
			c = new ConnectorONOFF(a, l, f);


		} else if (strcmp(art, "func2value") == 0) {
			int l = value["addr"].as<int>();
			JsonObject& fv = value["func2value"];
			int *array = new int[2 * fv.size()];
			int pos = 0;
			for (auto kv : fv) {
				array[pos++] = atoi(kv.key);
				array[pos++] = kv.value.as<int>();
			}
			ISettings* a = getSettingById(controller, value["out"][0].as<const char*>());
			c = new ConnectorFunc2Value(a, l, array, 2 * fv.size());


		} else if (strcmp(art, "turnout") == 0) {
			int addr = value["addr"].as<int>();
			ISettings* a = getSettingById(controller, value["out"][0].as<const char*>());
			c = new ConnectorTurnout(a, addr);


		} else if (strcmp(art, "lights") == 0) {
			int l = value["addr"].as<int>();
			int onoff = value["on"].as<int>();

			JsonArray& out = value["out"];
			if (out.size() != 2) {
				Logger::log("Keine zwei Ausgabe Led angegeben.");
				continue;
			}
			ISettings *ptr[4];
			for (int i = 0; i < 2; i++) {
				ptr[i] = getSettingById(controller, value["out"][i].as<const char*>());
			}
			Serial.println("Lights: " + String(onoff) + " " + " Addr: " + String(l));
			c = new ConnectorLights(ptr[0], ptr[1], l, onoff);


		} else {
			Logger::getInstance()->addToLog(
					"Config: Unbekannter Eintrag " + String(art));
		}
		if (c != NULL) {
			controller->registerNotify(c);
		}
		loop();

	}
}

ISettings* Config::getSettingById(Controller* c, const char* id) {
	for (int idx = 0; idx < c->getSettings()->size(); idx++) {
		ISettings* s = c->getSettings()->get(idx);
		if (s->getName().equals(id)) {
			return s;
		}
	}
	Logger::getInstance()->addToLog("Config: Unbekannte ID " + String(id));
	return NULL;
}
