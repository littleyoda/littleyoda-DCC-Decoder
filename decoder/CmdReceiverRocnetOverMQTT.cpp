/*
 * RocnetOverMQTT.cpp
 *
 *
 *  Created on: 17.06.2018
 *      Author: sven
 *
 *
 * Rocnet over MQTT
 *
 * Rocrail => MQTT => EPS8266
 *
 * Protocoll:
 *      see http://wiki.rocrail.net/doku.php?id=cs-protocol-en
 *      see http://www.rocrail.net/doc/rocrail-wrappers/wrapper-en-index.html
 *
 *      For microprocessors a horrible format because of the size and xml
 *
 *		Loc speed must be between 0 and 100
 */

#include "CmdReceiverRocnetOverMQTT.h"
#include "Controller.h"
#include "Consts.h"
#include "Logger.h"

CmdReceiverRocnetOverMQTT::CmdReceiverRocnetOverMQTT(Controller* c) : CmdReceiverBase(c) {
	client = new PubSubClient(espClient);
	client->setCallback(rocnetovermqttcallback);
	_instance = this;
	Udp.begin(8051);
}

CmdReceiverRocnetOverMQTT::~CmdReceiverRocnetOverMQTT() {
}


CmdReceiverRocnetOverMQTT *CmdReceiverRocnetOverMQTT::_instance = NULL;

int CmdReceiverRocnetOverMQTT::loop() {
	// Phase 2: Try to connect to mqtt
	if  (discoveryModus == 1) {
		if (!client->connected()) {
			Logger::log(LogLevel::INFO, "Connecting to MQTT Server...");
			String clientId = "ESP8266Client-";
			clientId += String(random(0xffff), HEX);
			if (client->connect(clientId.c_str())) {
				Logger::log(LogLevel::INFO, "Connected to MQTT Server");
				client->subscribe("rocrail/service/info/lc", 1);
				client->subscribe("rocrail/service/info/fn", 1);
			} else {
				Logger::log(LogLevel::INFO, "Connection to MQTT Server failed! "  + String(client->state()));
			}
		} else {
			client->loop();
			return 20;
		}
	}

	// Phase 1: Request MTQQ Data from rocrail
	if  (discoveryModus == 0) {
		int packetSize = Udp.parsePacket();
		if (packetSize) {
			char incomingPacket[255];
			incomingPacket[0] = 0;
			int len = Udp.read(incomingPacket, 254);
			if (len > 0) {
				incomingPacket[len] = 0;
			}
			String out = "";
			out = out + incomingPacket + "\n";
			String host = extractString(out, "BROKER-HOST:", "\n" );
			String port = extractString(out, "BROKER-PORT:", "\n" );

			// Fix Localhost with IP from sender
			if (host.equals("localhost") || host.equals("127.0.0.1")) {
				Logger::getInstance()->addToLog(LogLevel::WARNING, "Rocrail MTQQ localhost oder 127.0.0.1! Benutze stattdessen " + Udp.remoteIP().toString() + ":" + port);
				host = Udp.remoteIP().toString();
			}
			if (host.length() > 0  && port.length() > 0) {
				Logger::log(LogLevel::INFO, "Rocnet-Server detected: " + host + " " + String(port.toInt()));
				client->setServer(host.c_str(), port.toInt());
				discoveryModus = 1;
				return 0;
			}
		}
		Udp.beginPacket("224.0.0.1", 8051);
		#ifdef ESP8266
			Udp.write("BROKER-GET");
    	#elif ESP32
			Udp.write((const uint8_t *) "BROKER-GET", 10);
		#else
			#error "This Arch is not supported"
		#endif
		Udp.endPacket();
		return 200;
	}
	return 500;
}



void rocnetovermqttcallback(char* topic, byte* payload, unsigned int length) {
	payload[length - 1] = 0;
	String payloads = String((char*)payload);
	Logger::log(LogLevel::TRACE, payloads);
	if (payloads.startsWith("<lc ") || payloads.startsWith("<fn ")) {
		CmdReceiverRocnetOverMQTT::_instance->parse(payloads);
	}
}

void CmdReceiverRocnetOverMQTT::parse(String s) {
	if (s.startsWith("<lc ")) {
		int addr = extractXMLAttribute(s, "addr").toInt();
		controller->notifyDCCSpeed(addr,
				extractXMLAttribute(s, "V").toInt(),
				extractXMLAttribute(s, "dir").equals("true") ? 1 : -1,
						100, Consts::SOURCE_ROCRAIL);
		if (extractXMLAttribute(s, "fn").equals("false")) {
			controller->notifyDCCFun(addr, 0, 31, 0, Consts::SOURCE_ROCRAIL);
		}
	} else if (s.startsWith("<fn ")) {
		int addr = extractXMLAttribute(s, "addr").toInt();
		unsigned long f = 0;
		long  count = extractXMLAttribute(s, "fncnt").toInt();
		if (count > 31) {
			count = 31;
		}
		for (long i = count; i >= 0; i--) {
			String code = "f" + String(i);
			f = (f << 1) + (extractXMLAttribute(s, code).equals("true") ? 1 : 0);
		}
		controller->notifyDCCFun(addr, 0, count, f, Consts::SOURCE_ROCRAIL);
	}

}

String CmdReceiverRocnetOverMQTT::extractString(String payloads, String prefix, String postfix) {
	int len = prefix.length();
	int start = payloads.indexOf(prefix);
	if (start == -1) {
		return "";
	}
	int stop = payloads.indexOf(postfix, start + len);
	if (stop == -1) {
		return "";
	}
	String s = payloads.substring(start + len, stop);
	return s;

}

String CmdReceiverRocnetOverMQTT::extractXMLAttribute(String payloads, String attrname) {
	return extractString(payloads, attrname + "=\"", "\"");
}
