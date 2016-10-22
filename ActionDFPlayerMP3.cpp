/*
 * ActionDFPlayerMP3.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#include "ActionDFPlayerMP3.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include <ESP8266WiFi.h>


#include "DFPlayer_Mini_Mp3.h"

ActionDFPlayerMP3::ActionDFPlayerMP3(int s1, int s2) {
	Serial.print("Pins RX: " + String(s1) + "/ TX:" + String(s2));
	delay(2000);
	mp3Serial = new SoftwareSerial(s1, s2);
	mp3Serial->begin(9600);
	mp3_set_serialZeiger(mp3Serial);
	mp3_set_debug_serial(Serial);
	mp3_set_volume(13);

//	 mp3_set_device(2);
//	 mp3_get_tf_sum ();

//	  for (int i=0; i < 2; i++) {
//		  Serial.println("Seinding");
//	    // Query the total number of microSD card files
//	    mp3_get_folder_sum(i + 1);
//	    int count  = mp3_wait_folder_sum();
//
//	    Serial.print("Find ");
//	    Serial.print(count);
//	    Serial.print(" tracks in folder 0");
//	    Serial.print(i);
//	    Serial.println(".");
//	}
}

ActionDFPlayerMP3::~ActionDFPlayerMP3() {
}

int ActionDFPlayerMP3::loop() {
	int count = 0;
	while (true) {
		int b = mp3Serial->read();
		if (b == -1) {
			if (count > 0) {
				Serial.println();
			}
			return 0;
		}
		Serial.print(b, HEX);
		Serial.print(" ");
	}
	return 0;
}

String ActionDFPlayerMP3::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionDFPlayerMP3::getHTMLController(String urlprefix) {
	String message = "<div class=\"row\">";
	message += "<div class=\"column column-10\">Songs</div>";
	message += "<div class=\"column column-90\">";

	for (int i = 0; i <= 10; i++) {
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

void ActionDFPlayerMP3::setSettings(String key, String value) {
	if (key.equals("play")) {
		Serial.println("Playing " + value);
		mp3_play(value.toInt());
	} else 	if (key.equals("volume")) {
		Serial.println("Volume " + value);
		mp3_set_volume(value.toInt());
	}
//	for (int i = 0; i < 10; i++) {
//		mp3Serial -> write(play1[i]);
//	}
//	mp3Serial -> write(play1);
}

