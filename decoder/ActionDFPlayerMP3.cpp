/*
 * ActionDFPlayerMP3.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */
#ifdef ESP8266

#include "ActionDFPlayerMP3.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "Consts.h"


ActionDFPlayerMP3::ActionDFPlayerMP3(int addr, int rx, int tx) {
	Serial.print("Pins RX: " + String(rx) + "/ TX:" + String(tx));
	locoId = addr;
	delay(2000);
	mp3Serial = new SoftwareSerial(tx, rx);
	mp3Serial->begin(9600);
	initTime = millis() + 2000;
}


ActionDFPlayerMP3::~ActionDFPlayerMP3() {
}

int ActionDFPlayerMP3::loop() {
	if (initTime > 0 && millis() > initTime) {
		initTime = 0;
		cmdSelectSDCard();
	}
	if (mp3Serial->available() < 1) {
		return 50;
	}
	while (mp3Serial->available()> 0) {
		if (millis() - lastReceived > 200) {
			bufferPos = 0; // verwerfe alten Buffer
		}
		int b = mp3Serial->read();
		Serial.print(b, HEX);
		Serial.print(" ");
		ansbuf[bufferPos++] = (uint8_t) b;
		if (bufferPos == 10) {
			checkReceiveBuffer();
			bufferPos = 0;
		}
		lastReceived = millis();
	}
	return 50;
}

String ActionDFPlayerMP3::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionDFPlayerMP3::getHTMLController(String urlprefix) {
	String message = "<div class=\"row\">";
	message += "<div class=\"column column-10\">Songs</div>";
	message += "<div class=\"column column-90\">";

	for (int i = 1; i <= 10; i++) {
		message += " <a class=\"button\" href=\"";
		message += urlprefix;
		message += "key=play&value=";
		message += String(i);
		message += "\">";
		message += String(i);
		message += "</a>";
	}
	message += "</div>";
	message += "</div>";


	message += "<div class=\"row\">";
	message += "<div class=\"column column-10\">Volume</div>";
	message += "<div class=\"column column-90\">";
	for (int i = 0; i <= 29; i++) {
		message += " <a class=\"button\" href=\"";
		message += urlprefix;
		message += "key=volume&value=";
		message += String(i);
		message += "\">";
		message += String(i);
		message += "</a>";
	}
	message += "</div>";
	message += "</div>";
	return message;

}

void ActionDFPlayerMP3::cmdSelectSDCard() {
	sendCommand(0x09, 0x02);
}

void ActionDFPlayerMP3::cmdPlay(int number) {
	sendCommand(0x03, number);
}

void ActionDFPlayerMP3::cmdVolume(int volume) {
	if (volume > 30) {
		volume = 30;
	}
	if (volume < 0) {
		volume = 0;
	}
	sendCommand(0x06, volume);
}

void ActionDFPlayerMP3::sendCommand(uint8_t cmd, uint16_t data) {
  sendBuffer[0] = 0x7e;   // Start Byte
  sendBuffer[1] = 0xff;   // Version information
  sendBuffer[2] = 0x06;   // Length -2 (without Start and End-Byte
  sendBuffer[3] = cmd;//
  sendBuffer[4] = 0x01;   // Feedback
  sendBuffer[5] = (uint8_t)(data >> 8);  // first Databyte
  sendBuffer[6] = (uint8_t)(data);       // second Databyte
  sendBuffer[7] = 0xef;   // End-Byte
  mp3Serial->write(sendBuffer, 8);
}


void ActionDFPlayerMP3::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	if (id == locoId || id == Consts::LOCID_ALL) {
		if (speed == Consts::SPEED_EMERGENCY || speed == Consts::SPEED_STOP) {
			cmdStop();
			return;
		}
		int v = 30 * speed / SpeedSteps;
		cmdVolume(v);
	}
}

void ActionDFPlayerMP3::DCCFunc(int id, int bit, int newvalue, int source) {
	Serial.println("Bit: " + String(bit) + " Newvalue: " + String(newvalue));
	if (bit == 0) {
		cmdStop();
		return;
	}
	if (id == locoId && newvalue > 0) {
		cmdPlay(bit);
	}
}


void ActionDFPlayerMP3::setSettings(String key, String value) {
	if (key.equals("play")) {
		Serial.println("Playing " + String(value));
		cmdPlay(value.toInt());
	} else 	if (key.equals("volume")) {
		Serial.println("Volume " + value);
		cmdVolume(value.toInt());
	}
}

void ActionDFPlayerMP3::cmdStop() {
	sendCommand(0x16, 0x00);
}

void ActionDFPlayerMP3::checkReceiveBuffer() {
		bool valid =  ansbuf[0] == 0x7E
						&& ansbuf[1] == 0xFF
						&& ansbuf[2] == 0x06
						&& ansbuf[9] == 0xEF;
	  if (!valid) {
		  return;
	  }
	  if (ansbuf[3] == 0x3A && ansbuf[6] == 0x02) {
		  Serial.println("SD card plugged in");
	  } else if (ansbuf[3] == 0x3B && ansbuf[6 == 0x02]) {
		  Serial.println("SD card plugged out");
	  } else if (ansbuf[3] == 0x41) {
		  Serial.println("Command received");
	  } else if (ansbuf[3] == 0x3D) {
		  Serial.println("Finish playing! " + String(ansbuf[4] << 16 | ansbuf[5] << 8 | ansbuf[6]));
	  }
}
#endif
