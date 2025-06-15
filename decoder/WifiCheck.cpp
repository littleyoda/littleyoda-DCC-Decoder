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

#ifdef ESP32
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
	WifiCheck::connectedClients++;
	Logger::log(LogLevel::INFO,"Client connected: " + String(info.wifi_ap_staconnected.mac[0],HEX) + "-"
					 + String(info.wifi_ap_staconnected.mac[1],HEX) + "-"
		         	 + String(info.wifi_ap_staconnected.mac[2],HEX) + "-"
		         	 + String(info.wifi_ap_staconnected.mac[3],HEX) + "-"
		         	 + String(info.wifi_ap_staconnected.mac[4],HEX) + "-"
		         	 + String(info.wifi_ap_staconnected.mac[5],HEX));
	Logger::log(LogLevel::INFO,String (WifiCheck::connectedClients)  + " Clients connected");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
	WifiCheck::connectedClients--;
	Logger::log(LogLevel::INFO,"Client Disconnected: " + String(info.wifi_ap_staconnected.mac[0],HEX) + "-"
					+ String(info.wifi_ap_staconnected.mac[1],HEX) + "-"
					+ String(info.wifi_ap_staconnected.mac[2],HEX) + "-"
					+ String(info.wifi_ap_staconnected.mac[3],HEX) + "-"
					+ String(info.wifi_ap_staconnected.mac[4],HEX) + "-"
					+ String(info.wifi_ap_staconnected.mac[5],HEX));
	Logger::log(LogLevel::INFO,String (WifiCheck::connectedClients)  + " Clients connected");
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Logger::log(LogLevel::INFO,"WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Logger::log(LogLevel::INFO,"Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Logger::log(LogLevel::INFO,"WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Logger::log(LogLevel::INFO,"WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Logger::log(LogLevel::INFO,"Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Logger::log(LogLevel::INFO,"Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Logger::log(LogLevel::INFO,"Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Logger::log(LogLevel::INFO, "Obtained IP address: ");
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Logger::log(LogLevel::INFO,"Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Logger::log(LogLevel::INFO,"WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Logger::log(LogLevel::INFO,"WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Logger::log(LogLevel::INFO,"WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Logger::log(LogLevel::INFO,"WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Logger::log(LogLevel::INFO,"WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Logger::log(LogLevel::INFO,"WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Logger::log(LogLevel::INFO,"Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Logger::log(LogLevel::INFO,"Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Logger::log(LogLevel::INFO,"Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Logger::log(LogLevel::INFO,"Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Logger::log(LogLevel::INFO,"IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Logger::log(LogLevel::INFO,"Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Logger::log(LogLevel::INFO,"Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Logger::log(LogLevel::INFO,"Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Logger::log(LogLevel::INFO,"Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Logger::log(LogLevel::INFO,"Obtained IP address");
            break;
        default: break;
    }
}


#endif

WifiCheck::WifiCheck(Controller* c) {
	controll = c;
	#ifdef ESP32
	WiFi.onEvent(WiFiEvent);
	WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
 	WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
	#endif 
}

WifiCheck::~WifiCheck() {
	// TODO Auto-generated destructor stub
}

int WifiCheck::connectedClients = 0;

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
	}
	#endif
	return 100;
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