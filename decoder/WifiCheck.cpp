/*
 * WIFICHECK.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include <Arduino.h>

#include "WifiCheck.h"

#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <ESP8266mDNS.h>
#else
	#include <WiFi.h>
	#include <ESPmDNS.h>
	#include <esp_wifi.h>

#endif

#include "Utils.h"


WifiCheck::WifiCheck(Controller* c) {
	controll = c;
}

WifiCheck::~WifiCheck() {
	// TODO Auto-generated destructor stub
}


int WifiCheck::loop() {
	if (lastWifiStatus != Utils::getExtWifiStatus()) {
		Logger::getInstance()->addToLog(LogLevel::INFO, 
				"Wifi status changed: " + Utils::extwifi2String(lastWifiStatus)
		+ " => " + Utils::extwifi2String(Utils::getExtWifiStatus()) + " IP:"
		+ WiFi.localIP().toString());
		Serial.printf("Connection to: %s (Q:%d)\r\n", WiFi.BSSIDstr().c_str(), WiFi.RSSI());
		lastWifiStatus = Utils::getExtWifiStatus();
		if (WiFi.status() == WL_CONNECTED || WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
			Serial.println("MDNS start");
			bool mdns = MDNS.begin(controll->getHostname().c_str());
			Serial.println("MDNS start: " + String(mdns));
			MDNS.addService("http", "tcp", 80);
			MDNS.addServiceTxt("http", "tcp", "Version", gitversion);
			MDNS.addServiceTxt("http", "tcp", "Source", "github:littleyoda/littleyoda-DCC-Decoder");
			MDNS.addServiceTxt("http", "tcp", "FlashModus", "Arduino_Esp8266_2.6");
			
		}
	}
	#ifdef ESP8266
		MDNS.update();
	#endif
	int status = WiFi.status();
	int todo = 0;
	switch (status) {
	case WL_IDLE_STATUS:
		break;
	case WL_NO_SSID_AVAIL:
		break;
	case WL_SCAN_COMPLETED:
		break;
	case WL_CONNECTED:
		break;
	case WL_CONNECT_FAILED:
		todo = 2;
		break;
	case WL_CONNECTION_LOST:
		todo = 1;
		break;
	case WL_DISCONNECTED:
		todo = 1;
		break;
	default:
		break;
	};
	#ifdef ESP32
	switch (todo) {
		case 1:
			Serial.println("Reconnect");
			WiFi.reconnect();
			return 3000;
		case 2:
		Serial.println("Wifi Begin");
			WiFi.begin();
			return 3000;
		default:
			return 100;
	}
	#endif
}


int WifiCheck::wifiGetOpMode() {
			#ifdef ESP8266
		return wifi_get_opmode();
			#else
		return WiFi.getMode();
			#endif
}

bool WifiCheck::wifiISAPActive() {
	return (wifiGetOpMode() & 0x02) > 0;
}

uint8_t WifiCheck::getAPChannel() {
	#ifdef ESP8266
	struct softap_config conf_compare;
	wifi_softap_get_config(&conf_compare);
	return conf_compare.channel;
	#endif
	#ifdef ESP32
	wifi_config_t conf_current;
	esp_wifi_get_config(WIFI_IF_AP, &conf_current);
	return conf_current.ap.channel;
	#endif
}

int32_t WifiCheck::getWifiChannel() {
	#ifdef ESP8266
	return wifi_get_channel();
	#else
	return  WiFi.channel();
	#endif
}