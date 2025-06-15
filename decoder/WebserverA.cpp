/*
 * Webserver.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include <WiFiClient.h>
#include <Arduino.h>
#ifdef ESP32
	#include <FS.h>
	#include "SPIFFS.h"
#endif
#include "WebserverA.h"

#include "Controller.h"
#include "Logger.h"
#include "WebserviceLog.h"
#include "Utils.h"
#include "Config.h"
#include "Utils.h"
#include "InternalStatusToFile.h"

#ifdef ESP8266
ESP8266WebServer* Webserver::server = 0;
ESP8266HTTPUpdateServer* Webserver::httpUpdater = 0;
#elif ESP32
WebServer* Webserver::server = 0;
#endif

void Webserver::setStatus(String s) {
	status = s;
}

Webserver::Webserver(Controller* c) {
	status = "";
	controll = c;
#ifdef ESP8266
	server = new ESP8266WebServer(80);
#elif ESP32
	server = new WebServer(80);
#endif

	server->onNotFound(std::bind(&Webserver::handleNotFound, this));
	server->on("/", std::bind(&Webserver::handleRoot, this));
	server->on("/version", std::bind(&Webserver::handleVersion, this));
	server->on("/controll", std::bind(&Webserver::handleController, this));
	server->on("/status", std::bind(&Webserver::handleStatus, this));
	server->on("/cfg", std::bind(&Webserver::handleCfg, this));
	server->on("/set", std::bind(&Webserver::handleSet, this));
	server->on("/list", std::bind(&Webserver::handleFilelist, this));
	server->on("/flow", std::bind(&Webserver::handleFlow, this));
	server->on("/json", std::bind(&Webserver::handleJsonList, this));
	server->on("/pipefilter", std::bind(&Webserver::handlepipefiltercmd, this));
	server->on("/help", std::bind(&Webserver::handleHelp, this));
	server->on("/format", std::bind(&Webserver::handleFormat, this));
	server->on("/doformat", std::bind(&Webserver::handleDoFormat, this));
	server->on("/editconfig", HTTP_GET,std::bind(&Webserver::handleDoConfigGet, this));
	server->on("/editconfig", HTTP_POST, std::bind(&Webserver::handleDoConfigPost, this));
	server->on("/upload", HTTP_POST,
			[]() { server->send(200, "text/html", "<meta http-equiv=\"refresh\" content=\"1; URL=/list\">"); },
			std::bind(&Webserver::handleUpload, this));
	server->on("/del", std::bind(&Webserver::handleDel, this));
#ifdef ESP8266
	httpUpdater = new ESP8266HTTPUpdateServer();
	const char* update_path = "/firmware";
	httpUpdater->setup(server, update_path, "admin", "admin");
	
	server->begin(); // Only for ESP8266; Bug in ESP 32 Framework; see below
#elif ESP32
	server->on("/firmware", HTTP_GET, [](){
    	server->sendHeader("Connection", "close");
    	server->send(200, "text/html", F("<form method='POST' action='/firmware' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>"));
	});
	server->on("/firmware", HTTP_POST, []() {
      server->sendHeader("Connection", "close");
      server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    }, []() {
      HTTPUpload& upload = server->upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin()) { //start with max available size
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      } else {
        Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
      }
	});	
#endif
}

void Webserver::handlepipefiltercmd() {
	String dest = server->arg("dest").isEmpty() ? "" : server->arg("dest");
	String key = server->arg("key").isEmpty() ? "" : server->arg("key");
	String value = server->arg("value").isEmpty() ? "" : server->arg("value");
	if (!dest.isEmpty()) {
		controll->sendPipeFilter(dest,key,value);
	}
	String dataType = "text/html";
	String out = "";
		out += (String() + "<html><body><form action = \"/pipefilter\" method = \"post\">" +
		        "<input type=\"text\" id=\"dest\" name=\"dest\" value=\"" + dest + "\"><br><br>"+
		        "<input type=\"text\" id=\"key\" name=\"key\" value=\"" + key + "\"><br><br>"+
		        "<input type=\"text\" id=\"value\" name=\"value\" value=\"" + value + "\"><br><br>" +
				 "<input type = \"submit\" value = \"submit\" /></form></body></html>");
    server->send(200, "text/html", out);
}

void Webserver::handleHelp() {
	bool download = !server->arg("dl").isEmpty();
	String filename = (download) ? "/help.dat" : "/help.html";
	int status = Utils::getExtWifiStatus();
	bool wifiscan = ((status >> 4) == 1);
	String url = "vis-network.min.js.gz";
	if (download) {
		url = "https://unpkg.com/vis-network/standalone/umd/vis-network.min.js";
	}
	File f = SPIFFS.open(filename, "w");
	f.print( 
	"<!doctype html>\n"
	"<html><head>\n");
	if (!download) {
		f.print("<a href=\"/help?dl=true\">Download</a><br>");
	}
	f.print("<script type=\"text/javascript\" src=\"" + url + "\"></script>\n"
	"<link href=\"vis-network.min.css\" rel=\"stylesheet\" type=\"text/css\" />\n"
	"<style type=\"text/css\">    #mynetwork {      width: 90%;      height: 600px;      border: 1px solid lightgray;    }  </style>\n"
	"</head><body>\n"
	"<div id=\"mynetwork\"></div>\n"
	"<script type=\"text/javascript\">\n");
	f.print("var DOTstring = `dinetwork {" + controll->createDebugDiagramm() +  " }`;");
	f.print("  var container = document.getElementById(\"mynetwork\");");
	f.print("var data = vis.parseDOTNetwork(DOTstring);");
	f.print("var options = {"
"layout: {"
"hierarchical: {"
"direction: \"UD\","
"sortMethod: \"directed\","
"},"
"},"
"physics: {"
"hierarchicalRepulsion: {"
"avoidOverlap: 1,"
"},"
"},"
"};");
	f.print("      var network = new vis.Network(container, data, options);");
	f.print("</script><hr><pre>");

 	InternalStatusToFile callback;
	callback.setFile(&f);
 	controll->collectAllInternalStatus(&callback, "*", "*");
	f.print("</pre><hr/><pre>");
 	File g = SPIFFS.open("/config.json", "r");
	if (g) {
		while (g.available() > 0) {
			String s = g.readStringUntil('\n');
			if (s.indexOf("pwd") >= 0 && s.indexOf(":") >=0) {
				// Mask Password
				for (int i = s.indexOf(":") + 1; i < s.length(); i++) {
					char c = s[i];
					if (c == '"' || c == ' ') {
						continue;
					}
					if (isAlpha(c) && isLowerCase(c)) {
						c = 'x';
					} else if (isAlpha(c) && isUpperCase(c)) {
						c = 'X';
					} else if (isDigit(c)) {
						c = '0';
					} else {
						c = '#';
					}
					s.setCharAt(i, c);
				}
			}
			// }
			f.print(s);
			f.print("\n");
			delay(0);
		}
		g.close();
	} else {
		f.print("file open failed");
	}
	f.print("</pre><hr/>");
	if (wifiscan) {
		f.print("<pre>");
		f.print(Utils::wifiscan());
		f.print("</pre>");
	}
	f.print("</body></html>");

	f.close();
	f.print("</pre><hr/>");
	if (download) {
		server->sendHeader("Content-Disposition", "attachment; filename=help.html");
	}
	loadFromSPIFFS(filename);
	SPIFFS.remove(filename);

}

void Webserver::handleFlow() {
	server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	server->send(200, "text/html", 
"<!doctype html>"
"<html><head>"
"<script type=\"text/javascript\" src=\"vis-network.min.js.gz\"></script>"
"<link href=\"vis-network.min.css\" rel=\"stylesheet\" type=\"text/css\" />"
"<style type=\"text/css\">    #mynetwork {      width: 90%;      height: 600px;      border: 1px solid lightgray;    }  </style>"
"</head><body>"
"<div id=\"mynetwork\"></div>"
"<script type=\"text/javascript\">");
server->sendContent("var DOTstring = `dinetwork {" + controll->createDebugDiagramm() +  " }`;");

server->sendContent("var parsedData = vis.network.convertDot(DOTstring);"
"var data = { nodes: parsedData.nodes, edges: parsedData.edges };"
"var container = document.getElementById('mynetwork');"

//"var options = {  manipulation: false,  height: '90%',  layout: {    hierarchical: {      enabled: true,      levelSeparation: 150    }  },  physics: {    hierarchicalRepulsion: {      nodeDistance: 30    }  }};"
"var options = {"
"layout: {"
"hierarchical: {"
"direction: \"UD\","
"sortMethod: \"directed\","
"},"
"},"
"physics: {"
"hierarchicalRepulsion: {"
"avoidOverlap: 1,"
"},"
"},"
"};"


"var network = new vis.Network(container, data, options);"
"</script>"
"</body>"
"</html>");
}

void Webserver::handleUpload() {
	HTTPUpload& upload = server->upload();
	String configfile = "/config.json";
	String filename = upload.filename;
	if(!filename.startsWith("/")) {
		filename = "/" + filename;
	}
	if(upload.status == UPLOAD_FILE_START){
		Serial.println("File_Start");
		// Backup old config.json
		if (filename.equals(configfile)) {
			SPIFFS.remove(filename + ".old");
			SPIFFS.rename(filename, filename + ".old");
		}
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	} else if(upload.status == UPLOAD_FILE_WRITE) {
		Serial.println("File_Write");
		if(fsUploadFile) {
			fsUploadFile.write(upload.buf, upload.currentSize);
		}
	} else if(upload.status == UPLOAD_FILE_END){
		Serial.println("File_END");
		if(fsUploadFile) {
			fsUploadFile.close();
//			if (configfile.equals(filename)) {
//				if (!Config::parse(NULL, NULL, configfile, false)) {
//					SPIFFS.remove("/config.fehlerhaft");
//					SPIFFS.rename(configfile, "/config.fehlerhaft");
//					Serial.println("Config-Check failed");
//				} else {
//					Serial.println("Config-Check ok");
//				}
//			}
		}
	}
	yield();
}

void Webserver::handleFormat() {
	String output = "" + Utils::getHTMLHeader();
	output += F("<form action=\"/doformat\" method=\"get\" onsubmit=\"return confirm('Wirklich alles löschen?');\"><button type=\"submit\">Formatieren</button></form>");
	output += Utils::getHTMLFooter();
	server->send(200, "text/html", output);
}

void Webserver::handleDoFormat() {
	#ifdef ESP8266
	ESP.eraseConfig();
	#endif
	bool b = SPIFFS.format();
	String output = "" + Utils::getHTMLHeader();
	output += "Fertig, Status: " + String(b);
	output += Utils::getHTMLFooter();
	server->send(200, "text/html", output);
}


void Webserver::handleDoConfigPost() {
	Serial.println("PostConfig: " + String(server->arg("content").length()));
	if (String(server->arg("content").length())) {
		server->send(500, "text/html", "Fehler bei der Übertragung. Keine Daten erhalten.");
	}
	SPIFFS.remove("/config.json.old");
	SPIFFS.rename("/config.json", "/config.json.old");
	File dataFile = SPIFFS.open("/config.json", "w");
	dataFile.print(server->arg("content"));
	dataFile.close();
	dataFile.flush();
	server->sendHeader("Location", String("http://") + server->client().localIP().toString() + "/list", true);
	server->send ( 302, "text/plain", "");
	if (!server->arg("reboot").isEmpty()) {
			delay(2000);
			ESP.restart();
	}
}

void Webserver::handleDoConfigGet() {
	String dataType = "text/html";
	File dataFile = SPIFFS.open("/config.json", "r");
	String out = "";
		out += "<html><body><form action = \"/editconfig\" method = \"post\"><textarea rows = \"30\" cols = \"70\" name = \"content\">"
				 + dataFile.readString()
				 + "</textarea><br><input type=\"checkbox\" id=\"reboot\" name=\"reboot\"><label for=\"reboot\">Reboot</label><br><input type = \"submit\" value = \"submit\" /></form></body></html>";
    server->send(200, "text/html", out);
}

void Webserver::handleFilelist() {
	String output = "" + Utils::getHTMLHeader() + F("<table><thead><tr><th>Name</th><th>Size</th></thead><tbody>");
#ifdef ESP8266
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()){
		File entry = dir.openFile("r");
		output += "<tr><td>";
		output += "<a href=\"" + dir.fileName() + "\">";
		output += String(dir.fileName()).substring(1);
		output += "</a>";
		output += "</td><td>";
		output += String(dir.fileSize());
		output += "</td><td>";
		output += "<a href=\"/del?file=" + dir.fileName() + "\">";
		output += "&#x2421";
		output += "</a>";
		output += "</td></tr>";
	}
#elif ESP32
	fs::File root = SPIFFS.open("/");
	if (root && root.isDirectory()) {
		File file = root.openNextFile();
		while (file) {
			if (!file.isDirectory()) {
				output += "<tr><td>";
				output += "<a href=\"" + String(file.name()) + "\">";
				output += file.name();
				output += "</a>";
				output += "</td><td>";
				output += String(file.size());
				output += "</td><td>";
				output += "<a href=\"/del?file=" + String(file.name()) + "\">";
				output += "&#x2421";
				output += "</a>";
				output += "</td></tr>";
			}
			file = root.openNextFile();
		  }
	}
#endif
	output += F("</tbody></table><hr>");
	output += F("<form action=\"/upload\" method=\"post\" enctype=\"multipart/form-data\"><fieldset> <input name=\"Datei\" type=\"file\" size=\"50\"> ");
	output += F("<input class=\"button-primary\" value=\"Send\" type=\"submit\"> </fieldset></form><p/>");
	output += F("<a style=\"font-size: 4rem;\"  href=\"/editconfig\">");
	output += F("&#x270E;");
	output += F("</a>");
	output += Utils::getHTMLFooter();
	server->send(200, "text/html", output);
}

void Webserver::handleRoot() {
	String message = Utils::getHTMLHeader();
	message += F("<div class=\"row\">"

			"<div class=\"column\">"
			"<a style=\"font-size: 4rem;\" class=\"button\" href=\"/list\">&#128194;</a>"
			"</div>");

	for (int i = 0; i < services.size(); i++) {
		WebserviceBase* s = services.get(i);
		String linktext = s->getLinkText();
		if (linktext.equals("")) {
			continue;
		}
		message += F("<div class=\"column\">");
		message += "<a style=\"font-size: 4rem;\" class=\"button\" href=\"" + String(s->getUri()) + "\">" + linktext + "</a>";
		message += "</div>";
	}
	message += "<hr>";
	message += Utils::format(status, controll);
	message += "</div>";
	message += Utils::getHTMLFooter();
	server->send(200, "text/html", message);

}



void Webserver::handleVersion() {
	server->send(200, "text", compile_date);

}

int Webserver::loop() {
	#ifdef ESP32
//		if (lastWifiStatus != WiFi.status() && lastWifiStatus ==  WL_DISCONNECTED) {
		if (!init && 
					 ((lastWifiStatus != WiFi.status() && lastWifiStatus ==  WL_DISCONNECTED)
					 || WiFi.getMode() == WIFI_AP
					 || WiFi.getMode() == WIFI_AP_STA)
			 		) {
			Serial.println("Server begin");
			server->begin(); // Bug in Framework
			init = true;
		}
	#endif
	server->handleClient();
	return 2;
}

Webserver::~Webserver() {
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
	else if (path.endsWith(".dat"))
		dataType = "application/octet-stream";
	File dataFile = SPIFFS.open(path.c_str(), "r");
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
	if (server->hostHeader().equals(server->client().localIP().toString())) {
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
	} else {
		server->sendHeader("Location", String("http://") + server->client().localIP().toString(), true);
		server->send ( 302, "text/plain", "");
		server->client().stop();
	}
}


void Webserver::handleController() {
	server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	server->send(200, "text/html", Utils::getHTMLHeader());
	controll->getHTMLController();
	server->sendContent(Utils::getHTMLFooter());
}

void Webserver::sendContent(String s) {
	server->sendContent(s);
}

void Webserver::registerWebServices(WebserviceBase* base) {
	services.add(base);
	base->setServer(server);
	server->on(base->getUri(), std::bind(&WebserviceBase::run, base));
}

void Webserver::handleDel() {
	SPIFFS.remove(server->arg("file"));
	server->send(200, "text/html", F("<html><head><META http-equiv=\"refresh\" content=\"1;URL=/list\"></head><body>Deleting...</body></html>"));
}
void Webserver::handleSet() {
	Serial.println("Webserver");
	controll->setRequest(server->arg("id"), server->arg("key"), server->arg("value"));
	server->send(200, "text/html", F("<html><head><META http-equiv=\"refresh\" content=\"1;URL=/controll\"></head><body>Sending...</body></html>"));
}

void Webserver::handleCfg() {
	server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	server->send(200, "text/html", Utils::getHTMLHeader());
	controll->getHTMLCfg();
	server->sendContent(Utils::getHTMLFooter());
}

void Webserver::handleJsonList() {
	server->setContentLength(CONTENT_LENGTH_UNKNOWN);
	String m = server->arg("m").isEmpty() ? "*" : server->arg("m");
	String k = server->arg("k").isEmpty() ? "*" : server->arg("k");
	server->send(200, "application/json", controll->getInternalStatusAsJon(m, k));
}

void Webserver::handleStatus() {
//	controll->setRequest(server->arg("id"), server->arg("key"), server->arg("value"));
	server->send(200, "text/plain", 
		controll->getInternalStatus(server->arg("m"), server->arg("k"))
	);
}
