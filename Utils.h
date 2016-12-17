/*
 * Utils.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <ESP8266WiFi.h>
#include "Logger.h"

#define set_bit(var, bitIdx) ((var) |= (1 << (bitIdx)))
#define bit_is_set(var, bitIdx) ((var) & (1 << (bitIdx)))
#define clear_bit(var, bitIdx) ((var) &= (unsigned)~(1 << (bitIdx)))


class Utils {
public:

	static String getHTMLHeader() {
		return "<!DOCTYPE html><html xmlns=\"http://www.w3.org/1999/xhtml\"><head><link rel=\"stylesheet\" href=\"milligram.min.css\"><link rel=\"stylesheet\" href=\"css.css\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/></head><body>\n";
	}

	static String repeatString(String rep, int number) {
		String s;
		s.reserve(rep.length() * number);
		for (int i = 0; i < number; i++) {
			s.concat(rep);
		}
		return s;
	}

	static String getHTMLFooter() {
		String r = "<hr/><div class=\"row\">"

				"<div class=\"column\">"
				"<a style=\"font-size: 4rem;\" class=\"button\" href=\"/\">&#x29CD;</a>"
				"</div>"

				"<div class=\"column\">"
				"<a style=\"font-size: 4rem;\" class=\"button\" href=\"/controll\">Controll</a>"
				"</div>"


				"</div>"
				"</body></html>";

		return r;
	}

	static String wifi2String(int status) {
		switch (status) {
		case WL_IDLE_STATUS:
			return "Idle";
		case WL_NO_SSID_AVAIL:
			return "No SSID Avail";
		case WL_SCAN_COMPLETED:
			return "Scan Completed";
		case WL_CONNECTED:
			return "Connected";
		case WL_CONNECT_FAILED:
			return "Connect Failed";
		case WL_CONNECTION_LOST:
			return "Connection Lost";
		case WL_DISCONNECTED:
			return "Disconnected";
		default:
			return "Unknown";
		};
	}

	static String gpio2string(int gpio) {
		if (D1 == gpio) {
			return "D1";
		} else 	if (D2 == gpio) {
			return "D2";
		} else 	if (D3 == gpio) {
			return "D3";
		} else 	if (D4 == gpio) {
			return "D4";
		} else 	if (D5 == gpio) {
			return "D5";
		} else 	if (D6 == gpio) {
			return "D6";
		} else 	if (D7 == gpio) {
			return "D7";
		} else 	if (D8 == gpio) {
			return "D8";
		} else 	if (D9 == gpio) {
			return "D9";
		} else 	if (D10 == gpio) {
			return "D10";
		} else {
			Logger::getInstance()->addToLog("Unbekannter GPIO: " + String(gpio));
			return "Pin " + String(gpio);
		}
	}

	static int string2gpio(const char* pin) {
		if (strcmp("D1", pin) == 0) {
			return D1;
		} else if (strcmp("D2", pin) == 0) {
			return D2;
		} else if (strcmp("D3", pin) == 0) {
			return D3;
		} else if (strcmp("D4", pin) == 0) {
			return D4;
		} else if (strcmp("D5", pin) == 0) {
			return D5;
		} else if (strcmp("D6", pin) == 0) {
			return D6;
		} else if (strcmp("D7", pin) == 0) {
			return D7;
		} else if (strcmp("D8", pin) == 0) {
			return D8;
		} else if (strcmp("D9", pin) == 0) {
			return D9;
		} else if (strcmp("D10", pin) == 0) {
			return D10;
		} else {
			Logger::getInstance()->addToLog(
					"Unbekannte PIN in Config: " + String(pin));
			return -1;
		}
	}

	static int hextoint(char c) {
		unsigned int value = (unsigned int) c;
		if (isdigit(c)) {
			return value - '0';
		} else if (isalpha(c)) {
			return value - (isupper(c) ? 'A' - 10 : 'a' - 10);
		} else {
			return 0;
		}
	}

};

#endif /* UTILS_H_ */
