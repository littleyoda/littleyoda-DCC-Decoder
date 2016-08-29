/*
 * Webserver.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>
#include <FS.h>

#include "Webserver.h"
#include "Controller.h"
#include "Logger.h"
#include "WebserviceLog.h"
#include "Utils.h"

Webserver::Webserver(Controller* c) {
	controll = c;
	server = new ESP8266WebServer(80);
	server->onNotFound(std::bind(&Webserver::handleNotFound, this));
	server->on("/", std::bind(&Webserver::handleRoot, this));
	server->on("/controll", std::bind(&Webserver::handleController, this));
	server->on("/set", std::bind(&Webserver::handleSet, this));
	server->begin();
	SPIFFS.begin();
}

void Webserver::handleRoot() {
	server->send(200, "text/plain", String(millis()));
}

void Webserver::loop() {
	server->handleClient();

	if (lastWifiStatus != WiFi.status()) {
		Logger::getInstance()->addToLog(
				"Wifi status changed: " + Utils::wifi2String(lastWifiStatus)
						+ " => " + Utils::wifi2String(WiFi.status()) + " IP:"
						+ WiFi.localIP().toString());
		lastWifiStatus = WiFi.status();
	}

}

Webserver::~Webserver() {
	// TODO Auto-generated destructor stub
}

/**
 * Liefert eine Datei aus dem SPIFFS-Filesystem.
 * Funktioniert zur Zeit noch nicht
 */
bool Webserver::loadFromSPIFFS(String path) {
	Serial.println("Searching for " + path);
	String dataType = "text/plain";
	if (path.endsWith("/"))
		path += "index.htm";
	if (!SPIFFS.exists(path)) {
		return false;
	}
	if (path.endsWith(".src"))
		path = path.substring(0, path.lastIndexOf("."));
	else if (path.endsWith(".htm"))
		dataType = "text/html";
	else if (path.endsWith(".html"))
		dataType = "text/html";
	else if (path.endsWith(".txt"))
		dataType = "text/plain";
	else if (path.endsWith(".css"))
		dataType = "text/css";
	else if (path.endsWith(".js"))
		dataType = "application/javascript";
	else if (path.endsWith(".png"))
		dataType = "image/png";
	else if (path.endsWith(".gif"))
		dataType = "image/gif";
	else if (path.endsWith(".jpg"))
		dataType = "image/jpeg";
	else if (path.endsWith(".ico"))
		dataType = "image/x-icon";
	else if (path.endsWith(".xml"))
		dataType = "text/xml";
	else if (path.endsWith(".pdf"))
		dataType = "application/pdf";
	else if (path.endsWith(".zip"))
		dataType = "application/zip";
	File dataFile = SPIFFS.open(path.c_str(), "rb");
	int transmit = server->streamFile(dataFile, dataType);
	Serial.println(
			"Transmit: " + String(transmit) + " Size: "
					+ String(dataFile.size()));
	dataFile.close();
	return true;
}

void Webserver::handleNotFound() {
	if (loadFromSPIFFS(server->uri())) {
		return;
	}
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server->uri();
	message += "\nMethod: ";
	message += (server->method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server->args();
	message += "\n";
	for (uint8_t i = 0; i < server->args(); i++) {
		message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
	}
	server->send(404, "text/plain", message);
}

ESP8266WebServer* Webserver::server = 0;

void Webserver::handleController() {
	String message = "<html><head></head><body>";
	message += controll->getHTMLController();
	//for (int i = 0; i < actions.size(); i++) {
//		String s = actions.get(i)->getHTMLController();
//		message += s;
//		message += "<br>";
//	}
	server->send(200, "text/html", message);
}

void Webserver::addServices(WebserviceBase* base) {
	base->setServer(server);
	server->on(base->getUri(), std::bind(&WebserviceBase::run, base));
}

void Webserver::handleSet() {
	Serial.println("Webserver");
	controll->setRequest(server->arg("id"), server->arg("status").toInt());
	server->send(200, "text/html",
			"<html><head><META http-equiv=\"refresh\" content=\"1;URL=/controll\"></head><body>Sending...</body></html>");
}
