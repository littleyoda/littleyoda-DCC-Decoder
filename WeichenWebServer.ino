#include <ESP8266WiFi.h> 
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>


// WLAN-Config
const char* ssid = "Z21_YYYY";
const char* password = "XXXXXXXXXXXX";

// IP Z21
IPAddress z21Server(192,168,0,111);

WiFiUDP udp;
ESP8266WebServer server(80);

const int pin00 = 4;
const int pin01 = 5;
const int led = 13;

const int turnoutCount = 2;
int turnoutAddr[] = {15, 515}; // Currently only one adresse is really supported
int turnoutStatus[] = {0, 0};

unsigned int localPort = 21105;
const int packetBufferSize = 30;
byte packetBuffer[packetBufferSize];

int loopstatus = 0;

unsigned long lastTime = -1;


/**
 * Webserver-Section
 * 
 * 
 */

void handleRoot() {
  String message = "<html><body><table border=\"1\"><tr><th>Weichen ID (0 based)</th><th>Status</th></tr>";
  for (int i = 0; i < turnoutCount; i++) {
    message += "<tr><td>";
    message += turnoutAddr[i];
    message += "</td><td>";
    int newstatus = 0;
    switch (turnoutStatus[i]) {
       case 0: message += "Unknown";break;
       case 1: message += "Status P=0"; newstatus = 1; break;
       case 2: message += "Status P=1"; break;
       default: message += "Status invalid ("; message += turnoutStatus[i]; message += ")"; break;
    }
    message += "<td><a href=\"change?id=";
    message += turnoutAddr[i];
    message += "&status=";
    message += newstatus;
    message += "\">Flip to ";
    message += newstatus;
    message += "</a>";
    message += "</td></tr>";
    
  }
  server.send(200, "text/html", message);
}


void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void handleChange() {
  Serial.println(server.arg("id"));
  Serial.println(server.arg("status"));
  sendSetTurnout(server.arg("id"), server.arg("status"));
  String msg = "<html><head><META http-equiv=\"refresh\" content=\"1;URL=/\"></head><body>Sending...</body></html>";
  server.send(200, "text/html", msg);
}

void setupWebServer() {
  Serial.println("HTTP server starting...");
  server.on("/", handleRoot);
  server.on("/change", handleChange);
  server.onNotFound(handleNotFound);
  server.begin();
}


/**
 * Z21 Section
 * 
 * 
 */
void requestTurnoutInfo(int addr) {
    if (addr == -1) {
      return;
    }
    Serial.println("Sending Turnout-Info Request");
    memset(packetBuffer, 0, packetBufferSize);
    
    packetBuffer[0] = 0x08;
    packetBuffer[1] = 0x00;
    packetBuffer[2] = 0x40;  
    packetBuffer[3] = 0x00;  
    
    packetBuffer[4] = 0x43;  
    packetBuffer[5] = addr >> 8;  
    packetBuffer[6] = addr & 255;  
    packetBuffer[7] = packetBuffer[4] ^ packetBuffer[5] ^ packetBuffer[6];  

    udp.beginPacket(z21Server, localPort); 
    udp.write(packetBuffer, 8);
    udp.endPacket();
}

void enableBroadcasts() {
    Serial.println("Sending Broadcast Request");
    memset(packetBuffer, 0, packetBufferSize);
    
    // 0x08 0x00 0x50 0x00 0x01 0x00 0x01 0x00 
    // 2.16 LAN_SET_BROADCASTFLAGS
    packetBuffer[0] = 0x08;
    packetBuffer[1] = 0x00;
    packetBuffer[2] = 0x50;  
    packetBuffer[3] = 0x00;  
    
    // Flags = 0x00010001 (Little Endian)
    // 0x01 and 0x010000
    packetBuffer[4] = 0x01;  
    packetBuffer[5] = 0x00;  
    packetBuffer[6] = 0x01;  
    packetBuffer[7] = 0x00;  

    udp.beginPacket(z21Server, localPort); 
    udp.write(packetBuffer, 8);
    udp.endPacket();
}

void sendSetTurnout(String id, String status) {
    Serial.println("Sending Set Turnout");
    int statuscode = 0;
    if (status == "1") {
      statuscode = 1; 
    }
    memset(packetBuffer, 0, packetBufferSize);
    
    // 5.2 LAN_X_SET_TURNOUT
    packetBuffer[0] = 0x09;
    packetBuffer[1] = 0x00;
    packetBuffer[2] = 0x40;  
    packetBuffer[3] = 0x00;  

    packetBuffer[4] = 0x53;  
    packetBuffer[5] = 0x00;  
    packetBuffer[6] = 15;  
    packetBuffer[7] = 0xA8 | statuscode;  
    packetBuffer[8] = packetBuffer[4] ^ packetBuffer[5] ^ packetBuffer[6] ^ packetBuffer[7];

    udp.beginPacket(z21Server, localPort); 
    udp.write(packetBuffer, 9);
    udp.endPacket();
}

int getTurnoutIdx(int id) {
  int i;
  for (i = 0; i < turnoutCount; i++) {
    if (turnoutAddr[i] == id) {
      return i;
    }
  }
  return -1;
}

void doReceive(int cb) {
    Serial.print("packet received, length=");
    Serial.println(cb);
    if (cb  > packetBufferSize) {
      cb = packetBufferSize;
    }
    udp.read(packetBuffer, cb); 
    for (int idx = 0; idx < cb; idx++) {
      Serial.print(String(packetBuffer[idx], HEX));
    }
    Serial.println();

    // Check if this is a TURNOUT_INFO
    boolean turnOutInfoPaket = cb == 9 && 
        packetBuffer[0] == 0x09 &&  packetBuffer[1] == 0x00 && 
        packetBuffer[2] == 0x40 &&  packetBuffer[3] == 0x00 && packetBuffer[4] == 0x43;
    if (!turnOutInfoPaket) {
      return;
    }

    // Check if this is a message for us
    int id = packetBuffer[5] << 8 | packetBuffer[6];
    int status = packetBuffer[7];
    int idx = getTurnoutIdx(id);
    if (idx ==  -1) {
      return;
    }
    turnoutStatus[idx] = status;
    Serial.print("IDX: ");
    Serial.println(stauts);
    // TODO Chnage only if the status has changed
    if (idx == 0) {
      if (status == 1) {
              digitalWrite(pin00, 1);
              digitalWrite(pin01, 0);
      } else {
              digitalWrite(pin00, 0);
              digitalWrite(pin01, 1);
      }
      delay(200);
      digitalWrite(pin00, 0);
      digitalWrite(pin01, 0);
    }
}


/***
 * 
 * Setup & Loop
 * 
 * 
 */


void setup(void) {
  pinMode(pin00, OUTPUT);
  digitalWrite(pin00, 0);
  pinMode(pin01, OUTPUT);
  digitalWrite(pin01, 0);

  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("Version 23.Juli.2016 -- Weichenantrieb");
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  setupWebServer();

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

}


void loop(void) {
  server.handleClient();
  
  // Disable L298d
  digitalWrite(pin00, 0);
  digitalWrite(pin01, 0);

  // Check for UDP
  int cb = udp.parsePacket();
  if (cb != 0) {
    doReceive(cb);
  }

  // Scheduler for Requests
  long int time = millis();
  if (time - lastTime > 1000) {
    lastTime = time;
    if (loopstatus < turnoutCount) {
        requestTurnoutInfo(turnoutAddr[loopstatus]);
    } else {
        enableBroadcasts();
    }
    loopstatus++;
    if (loopstatus >= turnoutCount + 1) {
      loopstatus = 0;
    }
  }
}

