/*
 * Utils.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <ESP8266WiFi.h>
#include "Consts.h"
#include "Logger.h"
#include "GPIO.h"

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


	static String getMAC() {
		byte mac[6];
		WiFi.macAddress(mac);
		return String(mac[0],HEX) + "-"
					 + String(mac[1],HEX) + "-"
		         	 + String(mac[2],HEX) + "-"
		         	 + String(mac[3],HEX) + "-"
		         	 + String(mac[4],HEX) + "-"
		         	 + String(mac[5],HEX);
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

	static String getHex(unsigned char c) {
		String out = "0x";
		out.reserve(4);
		if (c < 16) {
			out += "0";
		}
		out += String(c, HEX);
		return out;
	}

	static String getBin(unsigned char c) {
		String out = String(c, BIN);
		while (out.length() < 8) {
			out = "0" + out;
		}
		return out;
	}
};

#endif /* UTILS_H_ */
