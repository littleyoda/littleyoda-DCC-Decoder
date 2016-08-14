#include <LinkedList.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>


// WLAN-Config
const char* ssid = "Z21_YYYY";
const char* password = "************";

// IP Z21
IPAddress z21Server(192, 168, 0, 111);

WiFiUDP udp;
ESP8266WebServer server(80);


// Logs
LinkedList<String> logger = LinkedList<String>();
int startmemory;
int wifiStatus = -1;

const int led = 16;
int dirPin[] = {D1, D2 };


const int turnoutCount = 2;
int enablePin[] = { D3 , D4};
int turnoutAddr[] = {15, 4}; 
int turnoutStatus[] = {0, 0};

unsigned int localPort = 21105;
const int packetBufferSize = 30;
byte packetBuffer[packetBufferSize];

int loopstatus = 0;
int ledstatus = 1;

unsigned long lastTime = -1;


String wifi2String(int status) {
  switch (status) {
        case WL_IDLE_STATUS: return "Idle";
        case WL_NO_SSID_AVAIL: return "No SSID Avail";
        case WL_SCAN_COMPLETED: return "Scan Completed";
        case WL_CONNECTED: return "Connected";
        case WL_CONNECT_FAILED: return "Connect Failed";
        case WL_CONNECTION_LOST: return "Connection Lost";
        case WL_DISCONNECTED: return "Disconnected";
        default: return "Unknown";
  }
}

void logWifi() {
    if (wifiStatus != WiFi.status()) {
      addToLog("Wifi status changed: " + wifi2String(wifiStatus) + " => " + wifi2String(WiFi.status()));
      wifiStatus = WiFi.status();
    }
}

void addToLog(String s) {
  logger.add(String(millis() / 1000) + ": " + s);
  if (logger.size() > 100) {
    logger.shift();
  }
}
/**
   Webserver-Section


*/

void handleRoot() {
  String message = "<html><body><table border=\"1\"><tr><th>Weichen ID (0 based)</th><th>Status</th></tr>";
  for (int i = 0; i < turnoutCount; i++) {
    message += "<tr><td>";
    message += turnoutAddr[i];
    message += "</td><td>";
    int newstatus = 0;
    switch (turnoutStatus[i]) {
      case 0: message += "Unknown"; break;
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


/**
 * Führt einen Wlan-Scan durch
 */
void handleScan() {
  int n = WiFi.scanNetworks();
  String message = "<html><body><table border=\"1\"><tr><th>SSID</th><th>RSSI</th><th>Encrypted</th></tr>";
    for (int i = 0; i < n; ++i)
    {
      message += "<tr><td>";
      message += WiFi.SSID(i);
      message += "</td><td>";
      message += WiFi.RSSI(i);
      message += "</td><td>";
      message += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"ENC";
      message += "</td></tr>\n";
    }
  message += "</table></body></html>";
  server.send(200, "text/html", message);
}

/**
 * Zeigt die Einstellungen, sowie die Log-Nachrichten an
 */
void handleLog() {
  String message = "<html><body><table border=\"1\"><tr><th>Attribut</th><th>Status</th></tr>";
  message += "<tr><td>Memory</td><td>" + String(ESP.getFreeHeap()) + "/" + String(startmemory) + " Bytes </td></tr>"; 
  message += "<tr><td>Uptime</td><td>" + String(millis() / 1000) + " sek</td></tr>"; 
  message += "<tr><td>Wifi RSSI</td><td>" + String(WiFi.RSSI()) + "</td></tr>"; 
  message += "<tr><td>Wifi Status</td><td>" + wifi2String(WiFi.status()) + "</td></tr>"; 
  message += "<tr><td>Wifi SSID</td><td>" + WiFi.SSID() + "</td></tr>"; 
  for (int i = 0; i < 2; i++) {
    message += "<tr><td>Pin: Dir</td><td>" + String(dirPin[i]) + "</td></tr>"; 
  }
  for (int i = 0; i < turnoutCount; i++) {
    message += "<tr><td>Turnout " + String(i) + "<br>Enable Pin</td><td>" + String(enablePin[i]) + "</td></tr>"; 
    message += "<tr><td>Turnout " + String(i) + "<br>ID (0-based)</td><td>" + String(turnoutAddr[i]) + "</td></tr>"; 
  }
  for (int i = 0; i < logger.size(); i++) {
    message += "<tr><td>Log ";
    message += i;
    message += "</td><td>";
    message += logger.get(i);
    message += "</td></tr>\n";
  }
  message += "</body></html>";
  server.send(200, "text/html", message);
}

void setupWebServer() {
  Serial.println("HTTP server starting...");
  server.on("/", handleRoot);
  server.on("/change", handleChange);
  server.on("/log", handleLog);
  server.on("/scan", handleScan);
  server.onNotFound(handleNotFound);
  server.begin();
}


/**
 * Fragt den Weichenstatus der Weiche bei der Z21 an
*/
void requestTurnoutInfo(int addr) {
  if (addr == -1) {
    return;
  }
  //Serial.println("Sending Turnout-Info Request");
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


/**
 * Sendet der Z21 einen BroadcastRequest
 */
void enableBroadcasts() {
//  Serial.println("Sending Broadcast Request");
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


/**
 * Sendet einen Weichenbefehl, der über das Webinterface ausgelöst wurde, an die Z21
 */
void sendSetTurnout(String id, String status) {
  Serial.println("Sending Set Turnout");
  addToLog("Web-Request ID: " + String(id) + " Status: " + status);
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
  packetBuffer[6] = id.toInt();
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

/**
 * Werte die empfangenen UDP Pakete aus
 */
void doReceive(int cb) {
//  Serial.print("packet received, length=");
  //Serial.println(cb);
  if (cb  > packetBufferSize) {
    cb = packetBufferSize;
  }
  udp.read(packetBuffer, cb);
  for (int idx = 0; idx < cb; idx++) {
    //Serial.print(String(packetBuffer[idx], HEX));
  }
  //Serial.println();

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
  
  // Check if the status has been changed
  if (turnoutStatus[idx] == status) {
    return;
  }
  
  // Change the Position of the Turnout
  turnoutStatus[idx] = status;
  addToLog("Changing: " + String(id) + " to " + status);
  Serial.print("Changing ");
  Serial.print(idx);
  Serial.print(" to P");
  Serial.println(status);
  if (status == 1) {
    digitalWrite(dirPin[0], 0);
    digitalWrite(dirPin[1], 1);
  } else {
    digitalWrite(dirPin[0], 1);
    digitalWrite(dirPin[1], 0);
  }
  digitalWrite(enablePin[idx], 1);
  delay(200);
  off();
}


void off() {
  int i;
  for (i = 0; i < turnoutCount; i++) {
    digitalWrite(enablePin[i], 0);
  }
}


/***

   Setup & Loop


*/


void setup(void) {
  startmemory = ESP.getFreeHeap();
  addToLog("Setup started");
  int i;
  for (i = 0; i < turnoutCount; i++) {
    pinMode(enablePin[i], OUTPUT);
    digitalWrite(enablePin[i], 0);
  }
  for (i = 0; i < 2; i++) {
    pinMode(dirPin[i], OUTPUT);
    digitalWrite(dirPin[i], 0);
  }
  pinMode(led, OUTPUT);
  
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("Version 14.Aug.2016 -- Weichenantrieb");
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  addToLog("Waiting for Connection");
  while (WiFi.status() != WL_CONNECTED) {
    setLedbyTime(100);
    delay(100);
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
  addToLog("Setup finished");
}



void setLedbyTime(int timep) {
  setLed((millis()  / timep) % 2);
}

void setLed(int i) {
    digitalWrite(led, i);
    ledstatus = i;
}

void loop(void) {
  off();
  // Wait for connection
  if (WiFi.status() != WL_CONNECTED) {
    setLedbyTime(100);
    delay(100);
    Serial.print(".");
    return;
  }

  server.handleClient();

  // Check for UDP
  int cb = udp.parsePacket();
  if (cb != 0) {
    doReceive(cb);
  }


  // Scheduler for Requests
  long int time = millis();
  setLedbyTime(1000); 
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

  // logging
  logWifi();
}

