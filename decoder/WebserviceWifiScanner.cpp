/*
 * WebserviceWifiScanner.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#include <WiFiClient.h>
	#ifdef ESP8266
#include <ESP8266WiFi.h>
	#elif ESP32
#include <WiFi.h>
	#endif


#include "WebserviceWifiScanner.h"

WebserviceWifiScanner::WebserviceWifiScanner() {
}

WebserviceWifiScanner::~WebserviceWifiScanner() {
}

char const* WebserviceWifiScanner::getUri() {
	return "/scan";
}

int WebserviceWifiScanner::dbm2qual(int dBm) {
	int quality;
	// dBm to Quality:
	if (dBm <= -100)
		quality = 0;
	else if (dBm >= -50)
		quality = 100;
	else
		quality = 2 * (dBm + 100);
	return quality;
}

void WebserviceWifiScanner::run() {
	int n = WiFi.scanNetworks();
	String message =
			"<html><body><table border=\"1\"><tr><th>SSID</th><th>RSSI</th><th>Encrypted</th></tr>";
	for (int i = 0; i < n; ++i) {
		message += "<tr><td>";
		message += WiFi.SSID(i);
		message += "</td><td>";
		message += dbm2qual(WiFi.RSSI(i));
		message += "%</td><td>";

		#ifdef ESP8266
			message += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "ENC";
		#elif ESP32
			message += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "ENC";
		#endif
		message += "</td></tr>\n";
	}
	message += "</table></body></html>";
	server->send(200, "text/html", message);
}


String WebserviceWifiScanner::getLinkText() {
	return "&#x1F4F6;";
}

