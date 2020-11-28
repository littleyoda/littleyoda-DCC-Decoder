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
#include "GPIO.h"
#include "DisplaySSD1306.h"
#include "DisplayHD44780.h"
#include "DisplayLCD1602_PCF8574.h"

Display::Display(Controller* c, String text, String model, LinkedList<int>* list, int rows, int cols) {
	controller = c;
	display = nullptr;
	if (model.equalsIgnoreCase("Wemos OLED Shield")) {
		Serial.println("Init");
		display = new DisplaySSD1306();
	} else if (model.equalsIgnoreCase("HD44780")) {
		display = new DisplayHD44780(list);

	} else if (model.equalsIgnoreCase("LCD1602_PCF8574")) {
		display = new DisplayLCD1602_PCF8574(rows, cols);
	} else {
		Logger::log(LogLevel::ERROR, "Model " + model + " wurde nicht gefunden!");
	}
	pattern = text;
}

	


Display::~Display() {
}


int Display::loop() {
	if (display == nullptr) {
		return 10000;
	}
	display->clear();
	fill(pattern);
	display->show();		  
	count++;
	if (count >= maxcount) {
		count = 1;
	}
	return 500;
}

String Display::fill(String s) {
	int status = 0;
	String out = "";
	String modul = "";
	String key = "";
	for (unsigned int i = 0; i < s.length(); i++) {
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
	if (s.length() <= display->columns()) {
		display->println(s);
	} else {
		// Scrolle durch den Text
		if (s.length() > maxcount) {
			maxcount = s.length();
		}
		s = ">" + s + "<";
		int toolong = (s.length() - display->columns()) + 1;
		int idx = count % toolong;
		String out;
		out = s.substring(idx, display->columns() + idx);
		display->println(out);
	}
}