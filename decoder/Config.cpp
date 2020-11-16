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
#include "ActionPWMDirect.h"
#include "ActionDCCGeneration.h"
#include "ActionSUSIGeneration.h"
#include "ActionSendTurnoutCommand.h"
#include "ActionSendSensorCommand.h"
#include "ActionStepperOutput.h"
#include "ActionPWMShieldV1Output.h"
#include "ActionTraktion.h"
//#include "ActionController.h"
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
#include "DCCZentrale.h"

#include "FilterLimitChange.h"
#include "InputRotoryEncoder.h"
#include "InputAnalog.h"
#include "CmdReceiverDCC.h"
#include "CmdZentraleZ21.h"
#include "CmdReceiverZ21Wlan.h"
#include "CmdReceiverRocnetOverMQTT.h"

#include "WebserviceCommandLogger.h"
#include "WebserviceLog.h"
#include "WebserviceWifiScanner.h"

#include "ISettings.h"
#include "Display.h"
#include "LocDataController.h"
#include "WifiCheck.h"

#include "CmdReceiverESPNOW.h"

#ifdef LY_FEATURE_AUDIO
	#include "ActionAudioI2S.h"
#endif


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
	if (!parser->isValid()) {
		return false;
	}
	String version = parser->getValueByKey(0, "version");
	if (!version.equals("3")) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Ungültige Version des Konfig-Files: " + version);
		return false;
	}
	if (dryrun) {
		delete(parser);
		return true;
	}

	parseCfg(controller, web, "cfg");
	parseOut(controller, web, "out");
	parseFilter(controller, web, "filter");
	parseIn(controller, web, "in");
	parseConnector(controller, web, "connector");

	delete(parser);
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Cfg-End");
	return true;
}


void Config::parseOut(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog(LogLevel::INFO, "Einträge in Out-Sektion: 0");
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
			controller->registerSettings(a);
			idx = parser->getNextSiblings(idx);
			continue;
		}

		if (m.equals("dcczentrale")) {
			Pin* gpioenable = new Pin(parser->getValueByKey(idx, "enable"));
			DCCZentrale* a = new DCCZentrale(gpioenable, controller);
			controller->registerNotify(a);
			controller->registerLoop(a);
			controller->registerSettings(a);
			idx = parser->getNextSiblings(idx);
			continue;
		}

		if (m.equals("traktion")) {
			int normal = parser->getValueByKey(idx, "normalAddr").toInt();
			int traktion = parser->getValueByKey(idx, "traktionAddr").toInt();
			int virtuell = parser->getValueByKey(idx, "virtuelleAddr").toInt();
			int func = parser->getValueByKey(idx, "func").toInt();
			ActionTraktion* a = new ActionTraktion(controller, normal, traktion, virtuell, func);
			controller->registerNotify(a);
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
			Logger::getInstance()->addToLog(LogLevel::ERROR, "ID is null");
			idx = parser->getNextSiblings(idx);
			continue;
		}
		if (m.equals("led")) {
			Pin* ledgpio = new Pin(parser->getValueByKey(idx, "gpio"));
			ActionLed* l = new ActionLed(ledgpio);
			l->setName(id);
			controller->registerSettings(l);

		} else if (m.equals("pwm")) {
			String type = parser->getValueByKey(idx, "type");
			ActionPWMOutput* a = NULL;
			if (type.equals("") || type.equals("direct")) {
				 int gpiopwm = GPIOobj.string2gpio(parser->getValueByKey(idx, "pwm"));
				 int gpiof = GPIOobj.string2gpio(parser->getValueByKey(idx, "forward"));
				 int gpior = GPIOobj.string2gpio(parser->getValueByKey(idx, "reverse"));
				 a = new ActionPWMDirect(gpiopwm, gpiof, gpior);
			} else if (type.equals("shieldv1")) {
				int addr = parser->getValueByKey(idx, "i2caddr", "48").toInt();
				int midx = parser->getValueByKey(idx, "motoridx", "0").toInt();
				a = new ActionPWMSchieldV1Output(addr, midx);
			} else {
				Logger::log(LogLevel::ERROR, "Unbekannter Type: " + type);
			}
			if (a != NULL) {
				if (parser->keyExists(idx, "freq")) {
					a->setFreq(parser->getValueByKey(idx, "freq").toInt());
				}
				a->setName(id);
				controller->registerSettings(a);
				int addridx = parser->getFirstChild(parser->getIdxByKey(idx, "values"));
				if (addridx > 0) {
					if (!parser->isArray(addridx)) {
						Logger::log(LogLevel::ERROR, "Values Format im Bereich PWM falsch!");
						idx = parser->getNextSiblings(idx);
						continue;

					}
					uint8_t* arr;
					arr = new uint8_t[128];
					for (int i = 0; i < 127; i++) {
						arr[i] = 255;
					}
					// Array mit den Werten aus der Config-Datei füllen
					arr[0] = 0;
					arr[1] = 0;
					arr[127] = 127;
					int child = parser->getFirstChild(addridx);
					while (child != -1) {
						int idx = parser->getFirstChild(child);
						int pos  = parser->getString(idx).toInt();
						int pwmvalue  = parser->getString(parser->getNextSiblings(idx)).toInt();
						if (pos < 1 || pos > 127 || pwmvalue < 0 || pwmvalue > 127) {
							Logger::getInstance()->addToLog(LogLevel::ERROR,"Ungültiger Wert " + String(pos) + "/" + String(pwmvalue));
							child = parser->getNextSiblings(child);
							continue;
						}
						arr[pos] = (uint8_t) pwmvalue;
						child = parser->getNextSiblings(child);
					}
					// Werte linear interpolieren
					int start = 0;
					int akt = 0;
					while (akt < 127) {
						akt++;
						if (arr[akt] == 255) {
							continue;
						}
						int laenge = akt - start;
						if (laenge == 1) {
							start = akt;
							continue;
						}
						float veraenderung = ((float) (arr[akt] - arr[start]) / laenge );
						float wert = arr[start];
						for (int i = start + 1; i < akt; i++) {
							wert += veraenderung;
							arr[i] = wert;
						}
						start = akt;
					}
					a->setPwmValues(arr);
				}
			}


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

		} else if (m.equals("sensor")) {
			// By Petr Osipov
			// Block for sensor type
			// Example of use in out section:
			//{
			//      "id":"sensor0",
			//      "m":"sensor",
			//      "addr":"10",
			//      "gpio":["D0","D3"]
			//  }
			// Assigned pins must be capable for input.
			// Address is the starting address. It is assigned to the first pin used. 
			// Following pins get the address increment by 1.
      
      		Serial.println("Processing sensor block");
			int addr = parser->getValueByKey(idx, "addr").toInt();
//      		int gpioarray = parser->getIdxByKey(idx, "gpio");
      		int element = parser->getFirstChildOfArrayByKey(idx, "gpio");

		    LinkedList<int> *list = new LinkedList<int>();
      		while (element!=-1) {
        		list->add(GPIOobj.string2gpio(parser->getString(element)));
        		element = parser->getNextSiblings(element);
      		}
      
 			ActionSendSensorCommand* a = new ActionSendSensorCommand(controller, addr, list);
			a->setName(id);      
			controller->registerSettings(a);      
			controller->registerNotify(a);      
      		controller->registerLoop(a);
		} else if (m.equals("sendturnout")) {
			int addr = parser->getValueByKey(idx, "addr").toInt();
      		ActionSendTurnoutCommand* a = new ActionSendTurnoutCommand(controller, addr);
      		a->setName(id);
      		controller->registerSettings(a);
      		controller->registerNotify(a);
	    } else if (m.equals("stepper")) {
			int child = parser->getIdxByKey(idx, "gpio");
			boolean persistent = parser->getBooleanByKey(idx, "persistent");
			child = parser->getFirstChild(child);
			ActionStepperOutput* a = new ActionStepperOutput(
								new Pin(parser->getString(parser->getChildAt(child, 0))),
								new Pin(parser->getString(parser->getChildAt(child, 1))),
								new Pin(parser->getString(parser->getChildAt(child, 2))),
								new Pin(parser->getString(parser->getChildAt(child, 3))),
								persistent
			);
			//per·sis·tent
			a->setName(id);
			a->load();
			controller->registerSettings(a);
			controller->registerLoop(a);

		#ifdef LY_FEATURE_AUDIO
		} else if (m.equals("audioi2s")) {
			ActionAudioI2S* a = new ActionAudioI2S();
			a->setName(id);
			controller->registerSettings(a);
			controller->registerLoop(a);

		#endif
		} else if (m.equals("locdatacontroller")) {
			int element = parser->getFirstChildOfArrayByKey(idx, "locaddr");
		    LinkedList<int> *list = new LinkedList<int>();
      		while (element!=-1) {
				list->add(parser->getString(element).toInt());
        		element = parser->getNextSiblings(element);
      		}
			LocDataController* l = new LocDataController(controller, list);
			Serial.println("ID: " + id);
			l->setName(id);
			controller->registerSettings(l);

		} else {
			Logger::getInstance()->addToLog(LogLevel::ERROR, 
					"Config: Unbekannter Eintrag " + m);
		}
		idx = parser->getNextSiblings(idx);
		loop();
	}
}



void Config::parseCfg(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog(LogLevel::INFO, "Einträge in CFG-Sektion: 0");
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
			controller->registerStatus(rec);


		} else if (m.equalsIgnoreCase("simulateZ21")) {
			CmdZentraleZ21* rec = new CmdZentraleZ21(controller);
			controller->registerCmdReceiver(rec);
			controller->registerCmdSender(rec);
			controller->registerStatus(rec);
			controller->registerNotify(rec);

		} else if (m.equals("espnow")) {
			String role = parser->getValueByKey(idx, "role");
			String key = parser->getValueByKey(idx, "key");
			bool err = false;
			if (key.length() != 32) {
				Logger::getInstance()->addToLog(LogLevel::ERROR, "Ungültiger Verschlüsselungskey");
				err = true;
			} 
			int kanal = parser->getValueByKey(idx, "kanal").toInt();
			if (kanal < 1 || kanal > 12) {
				Logger::getInstance()->addToLog(LogLevel::ERROR, "Ungültiger Kanal für espnow");
				err = true;
			}
			if (!err) {
				CmdReceiverESPNOW* rec = new CmdReceiverESPNOW(controller, role, key, kanal);
				controller->registerCmdReceiver(rec);
				controller->registerCmdSender(rec);
				controller->registerStatus(rec);
			}
		} else if (m.equals("rocnetovermqtt")) {
			CmdReceiverRocnetOverMQTT* c = new CmdReceiverRocnetOverMQTT(controller);
			controller->registerCmdReceiver(c);
			controller->registerCmdSender(c);

		} else if (m.equals("webservicewifiscanner")) {
			web->registerWebServices(new WebserviceWifiScanner());

		} else if (m.equals("webservicestatus")) {
			web->setStatus(parser->getValueByKey(idx, "text", ""));
		} else if (m.equals("webservicelog")) {
			web->registerWebServices(new WebserviceLog());

		} else if (m.equals("mp3")) {
						// int  addr = value["addr").toInt();
						// int tx = GPIOobj.string2gpio(value["tx"].as<const char*>());
						// int rx = GPIOobj.string2gpio(value["rx"].as<const char*>());
						// controller->registerNotify(new ActionDFPlayerMP3(addr, tx, rx));

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
					Logger::log(LogLevel::ERROR,"Netzwerkkonfiguration (ip, netmask, gw) unvollständig");
					idx = parser->getNextSiblings(idx);
					continue;
				}
			} else {
				Logger::log(LogLevel::INFO, "Netzwerkkonfiguration per DHCP");
			}
			// int ch = 1;
			// String kanal = parser->getValueByKey(idx, "kanal");
			// if (kanal.length() > 0) {
			// 	ch = kanal.toInt();
			// }
			WiFi.setAutoReconnect(true);
			WiFi.begin(ssid.c_str(), pwd.c_str());
			controller->registerLoop(new WifiCheck(controller));

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
				Logger::log(LogLevel::ERROR, "softAP fehlgeschlagen!");
			}
			WiFi.enableAP(true);
			Serial.println("AP-IP: " + WiFi.softAPIP().toString());
			controller->registerLoop(new WifiCheck(controller));


		} else if (m.equals("i2c")) {
			int sda = GPIOobj.string2gpio(parser->getValueByKey(idx, "sda"));
			int scl = GPIOobj.string2gpio(parser->getValueByKey(idx, "scl"));
			Wire.begin(sda, scl);


		} else if (m.equals("i2cslave")) {
			String d = parser->getValueByKey(idx, "d");
			bool isPca9685 = d.equalsIgnoreCase("pca9685");
			bool isMcp23017 = d.equalsIgnoreCase("mcp23017");
			bool isArduinoExtender = d.equalsIgnoreCase("arduinoextender");
			String variant = "";
			if (isArduinoExtender) {
				variant = parser->getValueByKey(idx, "variant");
			}
      
			if (isPca9685 || isMcp23017 || isArduinoExtender) {
				int addridx = parser->getIdxByKey(idx, "addr");
				addridx = parser->getFirstChild(addridx);
				if (!parser->isArray(addridx)) {
					Logger::log(LogLevel::ERROR, "Format für MCP23017/PC9685/ArduinoExtender Adressen falsch!");
					idx = parser->getNextSiblings(idx);
					continue;
				} 
				int child = parser->getFirstChild(addridx);
				while (child != -1) {
					int addr = parser->getString(child).toInt();
					int i2caddr = 0;
					if (addr >= 0x10) {
						i2caddr = addr;
					} else if (isMcp23017) {
						i2caddr = addr + 0x20; // Base + Offset
					} else if (isPca9685) {
						i2caddr = addr + 0x40; // Base + Offset
					}
      
					Wire.beginTransmission(i2caddr);
					int ret = Wire.endTransmission();
					String tret = "Failed (" + String(ret) + ")";
					if (ret == 0) {
						tret = "OK";
					}
					Logger::getInstance()->addToLog(LogLevel::INFO, "Test MCP23017/PC9685/ArduinoExtender auf I2c/" + String(i2caddr) + ": " + tret);
					if (ret == 0) {
						if (isMcp23017) {
							GPIOobj.addMCP23017(addr);
						} else if (isArduinoExtender) {
            			GPIOobj.addArduinoExtender(i2caddr, variant);
						} else
						{
							GPIOobj.addPCA9685(i2caddr);
						}
					}
					child = parser->getNextSiblings(child);
				}
			} 
      
      
			else {
				Logger::getInstance()->addToLog(LogLevel::ERROR, "Unbekanntes Gerät (I2C): " + String(d));
			}
		// } else if (m.equals("lycontroller")) {
		// 	ActionController* a = new ActionController(controller);
		// 	controller->registerNotify(a);
		// 	controller->registerLoop(a);
		} else if (m.equals("display")) {
      		int element = parser->getFirstChildOfArrayByKey(idx, "gpio");
		    LinkedList<int> *list = new LinkedList<int>();
      		while (element!=-1) {
        		list->add(GPIOobj.string2gpio(parser->getString(element)));
        		element = parser->getNextSiblings(element);
      		}

			Display* d = new Display(controller, 
									parser->getValueByKey(idx, "text", "No Text"),
									parser->getValueByKey(idx, "model", ""),
									list
									);
		 	controller->registerLoop(d);
		} else {
			Logger::getInstance()->addToLog(LogLevel::ERROR, 
					"Config: Unbekannter Eintrag " + m);
		}

		loop();
		idx = parser->getNextSiblings(idx);

	}

}

void Config::parseConnector(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog(LogLevel::INFO, "Einträge in Connector-Sektion: 0");
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
				Logger::log(LogLevel::ERROR, "Format im Bereich Turnout // LED falsch!");
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
				Logger::log(LogLevel::ERROR, "Format GPIO//Sendturnout falsch!");
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
				cin = new ConnectorGPIO(atc, g, 1, 0, "sd");
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
				Logger::log(LogLevel::ERROR, "Format DIRECTION/LEDS falsch! GPIO Array nicht gefunden!");
				idx = parser->getNextSiblings(idx);
				continue;
			}
			while (child != -1) {
				if (ESP.getFreeHeap() < 1200) {
					lowmemory = true;
					break;
				}
				String pin = parser->getString(child);

				Logger::log(LogLevel::DEBUG, "d=>l Pin:" + String(pin) + " Addr:" + String(addr) + " Func:" + String(func));
				ActionLed* g = new ActionLed(new Pin(pin));
				cin = new ConnectorLights(g, addr, func, direction);
				controller->registerNotify(cin);
				child = parser->getNextSiblings(child);
			}
		} else {
			Logger::getInstance()->addToLog(LogLevel::ERROR, 
					"Config: Unbekannter Eintrag In: " + String(in) + " Out: " + String(out));
		}
		loop();
		idx = parser->getNextSiblings(idx);
	}
}


void Config::parseIn(Controller* controller, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog(LogLevel::INFO, "Einträge in In-Sektion: 0");
		return;
	}
	while (idx != -1) {
		String m = parser->getValueByKey(idx, "m");
		Serial.println("MEM "  + String(ESP.getFreeHeap()) + " " + m);
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
				array[pos++] = parser->getString(child).toInt();
				array[pos++] = parser->getString(parser->getFirstChild(child)).toInt();
				child = parser->getNextSiblings(child);
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
				Serial.println("Turnout2Value (" + parser->getString(child) + "/" + parser->getString(parser->getFirstChild(child)) + ")");
				array[pos++] = parser->getString(child).toInt();
				array[pos++] = parser->getString(parser->getFirstChild(child)).toInt();
				child = parser->getNextSiblings(child);
			}

			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorTurnout2Value(a, addr, array, arraysize);
		} else if (m.equals("turnout")) {
			int addr = parser->getValueByKey(idx, "addr").toInt();
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(controller, conn);
			c = new ConnectorTurnout(a, addr);

		} else if (m.equals("lights")) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, 
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
			uint16_t high = parser->getValueByKey(idx, "high", "1").toInt();
			uint16_t low = parser->getValueByKey(idx, "low", "0").toInt();
			ISettings* a = getSettingById(controller, conn);
			String var = parser->getValueByKey(idx, "var", "sd");
			c = new ConnectorGPIO(a, g, high, low, var);

		} else if (m.equals("rotoryencoder") || m.equals("rotaryencoder")) {
      		int stepvalue = parser->getValueByKey(idx, "stepvalue", "1").toInt();
			int element = parser->getFirstChildOfArrayByKey(idx, "gpio");
		    LinkedList<int> *list = new LinkedList<int>();
      		while (element!=-1) {
        		list->add(GPIOobj.string2gpio(parser->getString(element)));
        		element = parser->getNextSiblings(element);
      		}
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(controller, conn);
			String var = parser->getValueByKey(idx, "var", "relSpeed");
			InputRotoryEncoder* ire = new InputRotoryEncoder(a, list, var, stepvalue);
			controller->registerLoop(ire);
			c = ire;

		} else if (m.equals("analoggpio")) {
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			Serial.println("GPIO " + parser->getValueByKey(idx, "gpio", "A0"));

			int gpio = GPIOobj.string2gpio(parser->getValueByKey(idx, "gpio", "A0"));
			ISettings* a = getSettingById(controller, conn);
			InputAnalog* ire = new InputAnalog(a, gpio);
			controller->registerLoop(ire);

      		int element = parser->getFirstChildOfArrayByKey(idx, "value2out");
      		while (element!=-1) {
				if (parser->isArray(element)) {
					int e = parser->getFirstChild(element);
					int s1 = parser->getString(e).toInt();
					e = parser->getNextSiblings(e);
					int s2 = parser->getString(e).toInt();
					e = parser->getNextSiblings(e);
					String key = parser->getString(e);
					e = parser->getNextSiblings(e);
					String value = parser->getString(e);
					ire->addArea(s1, s2, key, value);
				} else {
					Logger::getInstance()->addToLog(LogLevel::ERROR, "Fehlerhafter Aufbau JSON");
				}
        		element = parser->getNextSiblings(element);
      		}

			c = ire;

		} else {
			Logger::getInstance()->addToLog(LogLevel::ERROR, 
					"Config: Unbekannter Eintrag " + m);
		}
		if (c != NULL) {
			controller->registerNotify(c);
			controller->registerConnectors(c);
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
	String out = "";
	for (int idx = 0; idx < c->getSettings()->size(); idx++) {
		ISettings* s = c->getSettings()->get(idx);
		out = out + s->getName() + "; ";
	}
	Logger::getInstance()->addToLog(LogLevel::ERROR, "Config: Unbekannte ID: '" + String(id) + "' Verfügbar: [" + out + "]");
	return NULL;
}

void Config::parseFilter(Controller* c, Webserver* web, String n) {
	int idx = parser->getFirstChildOfArrayByKey(0, n);
	if (idx == -1) {
		Logger::getInstance()->addToLog(LogLevel::INFO, "Einträge in Filter Sektion: 0");
		return;
	}
	while (idx != -1) {
		if (ESP.getFreeHeap() < 1200) {
			lowmemory = true;
			break;
		}
		String m = parser->getValueByKey(idx, "m");
		Serial.println("MEM "  + String(ESP.getFreeHeap()) + " " + m);

		String id = parser->getValueByKey(idx, "id");
		if (id.length() == 0) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "ID is null");
			idx = parser->getNextSiblings(idx);
			continue;
		}
		if (m.equalsIgnoreCase("limitchange")) {
			String rate = parser->getValueByKey(idx, "rate", "100");
			String value = parser->getValueByKey(idx, "value", "sd");
			FilterLimitChange* f = new FilterLimitChange(rate.toInt(), value);
			f->setName(id);
			String conn = parser->getString(parser->getFirstChildOfArrayByKey(idx, "out"));
			ISettings* a = getSettingById(c, conn);
			f->addAction(a);
			c->registerSettings(f);
			c->registerLoop(f);

		} else {
			Logger::getInstance()->addToLog(LogLevel::ERROR, 
					"Config: Unbekannter Eintrag " + m);
		}
		loop();
		idx = parser->getNextSiblings(idx);
	}

}
