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

Display::Display(Controller* c, String text, String _model, LinkedList<int>* list, int rows, int cols, int rotation) {
	controller = c;
	display = nullptr;
	model = _model;
	if (model.equalsIgnoreCase("Wemos OLED Shield")) {
		display = new DisplaySSD1306(rotation);

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


/**
 * Loop ist so designt, dass es max. ein Zeichen pro Durchlauf ausgibt, um die Laufzeit möglichst gering zu halten.
 * Dieses funktioniert nur, wenn das genutzte Display diese Funktion unterstützt.
 */
int Display::loop() {
	if (display == nullptr) {
		return 10000;
	}

	if (idx == -1) {
		if (subidx == 0) {
			output.clear();
			fill(pattern);
			count++;
			if (count >= maxcount) {
				count = 1;
			}

		} else if (subidx == 1) {
			//display->clear();
			display->setPos(0, 0);
		} else if (subidx == 2) {
			for (int i = 0; i < output.size(); i++) {
				//Serial.println(output.get(i));
				// if (i > 0) {
				// 	outputtext = outputtext + "\n";
				// }
				String s = output.get(i);
				if (s.length() > display->columns()) {
					// Scrolle durch den Text
					if (s.length() > maxcount) {
						maxcount = s.length();
					}
					s = ">" + s + "<";
					int toolong = (s.length() - display->columns()) + 1;
					int tidx = count % toolong;
					output.set(i, s.substring(tidx, display->columns() + tidx));
				}
			}
		}
		subidx++;
		if (subidx == 3) {
			idx++;
			subidx = 0;
		}
	}
	if (idx >= 0) {
		display->print(output.get(idx)[subidx]);
		subidx++;
		if (subidx >= output.get(idx).length()) {
			idx++;
			subidx = 0;
			display->setPos(0, idx);
		}
		if (idx == display->rows() || idx >= output.size() ) {
			display->show();
			idx = -1;
			subidx = 0;
			return 500;
		}
	}
	return 1;
}

void Display::fill(String s) {
	output.clear();
	char temp[100];
	int status = 0;
	String out = "";
	String var = "";
	for (unsigned int i = 0; i < s.length(); i++) {
		char c = s[i];
		switch (status) {
			case 0: if (c == '$') {
						var = "";
						status = 2; // start of "${key|value}"
					} else if (c == '\\') {
						status = 3;
					} else {
						out += c;
					}
					break;
			// case 1: // Not used
			case 2:
					if (c == '}') {
						status = 0;
						String data = controller->getInternalStatus(Utils::getSubstring(var, '|', 0), Utils::getSubstring(var, '|', 1));
						int idx = 2;
						String format = Utils::getSubstring(var, '|', idx);
						while (!format.isEmpty()) { 
							if (format.startsWith("%")) {
								size_t len = snprintf(temp, sizeof(temp), format.c_str(), data.c_str());
								if (len >=0 && len < sizeof(temp)) {
									data = temp;
								} else {
									data = "<PRINTF-ERROR>";
									break;
								}
							} else if (format.startsWith("s/")) {
								data.replace(Utils::getSubstring(format, '/', 1),  Utils::getSubstring(format, '/', 2));
							} else {
								data = "<FORMAT-ERROR>";
								break;
							}
							idx++;
							format = Utils::getSubstring(var, '|', idx);
						}
						out += data;
					} else if (c == '{') {
						//
					} else {
						var += c;
					}
					break;			
			case 3:
				if (c == 'n') {
					while (out.length() < display->columns()) {
						out = out + " ";
					}
//					Serial.println(">" + out  + "<");
					output.add("" + out);
					out = "";
				}
				status = 0;
				break;
		}
	}
	if (!out.isEmpty()) {
		while (out.length() < display->columns()) {
			out = out + "%";
		}
//		Serial.println(">" + out  + "<");
		output.add("" + out);
	}
}

