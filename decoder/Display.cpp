/*
 * DISPLAY.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "Display.h"
#ifdef ESP8266
	#include <ESP8266WiFi.h>
#elif ESP32
    #include <WiFi.h>
#endif
#include "Utils.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GPIO.h"
 
Display::Display(Controller* c, String text, String model) {
	controller = c;
	display = nullptr;
	Serial.println(model);
	if (model.equalsIgnoreCase("Wemos OLED Shield")) {

		Serial.println("Init");
		pinMode(0, OUTPUT);
		display = new Adafruit_SSD1306(-1);
		display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
	    display->display();
		pattern = text;
		Serial.println(pattern);
	} else {
		Logger::log(LogLevel::ERROR, "Model " + model + " wurde nicht gefunden!");
	}
}

	


Display::~Display() {
}


int Display::loop() {
	display->clearDisplay();
  	display->setTextSize(1);
  	display->setTextColor(WHITE);
	display->setCursor(0,0);
	fill(pattern);
	display->display();		  
	count++;
	if (count >= maxcount) {
		count = 1;
	}
	return 700;
}

String Display::fill(String s) {
	int status = 0;
	String out = "";
	String modul = "";
	String key = "";
	for (int i = 0; i < s.length(); i++) {
		char c = s[i];
		switch (status) {
			case 0: if (c == '$') {
						modul = "";
						key = "";
						status = 1;
					} else if (c == '\\') {
						status = 3;
					} else {
						out += c;
					}
					break;
			case 1: if (c == '{') {
						//
					} else if (c == '|') {
						status = 2;
					} else {
						modul += c;
					}
					break;
			case 2:
					if (c == '}') {
						status = 0;
						out += controller->getInternalStatus(modul, key);
					} else {
						key += c;
					}
					break;			
			case 3:
				if (c == 'n') {
					show(out);
					out = "";
				}
				status = 0;
				break;
		}
	}
	show(out);
	return out;
}

void Display::show(String s) {
	if (s.length() <= width) {
		display->println(s);
	} else {
		if (s.length() > maxcount) {
			maxcount = s.length();
		}
		int toolong = (s.length() - width) + 1;
		int idx = count % toolong;
		String out;
		out = s.substring(idx, width + idx);
		display->println(out);
	}
}