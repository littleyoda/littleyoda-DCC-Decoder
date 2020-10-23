/*
 * Utils.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef UTILS_H_
#define UTILS_H_

#ifdef ESP8266
	#include <ESP8266WiFi.h>
#else
	#include <WiFi.h>
#endif

#include "Consts.h"
#include "Logger.h"
#include "GPIO.h"

#define set_bit(var, bitIdx) ((var) |= (1 << (bitIdx)))
#define bit_is_set(var, bitIdx) ((var) & (1 << (bitIdx)))
#define clear_bit(var, bitIdx) ((var) &= (unsigned)~(1 << (bitIdx)))


class Utils {
public:

	static String getHTMLHeader() {
		return "<!DOCTYPE html>\n<html xmlns=\"http://www.w3.org/1999/xhtml\">\n<head><link rel=\"stylesheet\" href=\"milligram.min.css\"><link rel=\"stylesheet\" href=\"css.css\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/></head>\n<body>\n"

"				<script>\n"
"				function send(url) {\n"
"				  var xhttp = new XMLHttpRequest();\n"
// "				  xhttp.onreadystatechange = function() {\n"
// "				  }\n"
"				  xhttp.open(\"GET\" , url, true);\n"
"				  xhttp.send();\n"
"				}\n"
"				</script>\n";

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

	static int getExtWifiStatus() {
		int status = WiFi.status();
                #ifdef ESP8266
			status = status | (wifi_get_opmode() << 4);
                #else
			status = status | (WiFi.getMode() << 4);
                #endif
		return status;
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
			return "Unknown (" + String(status) + ")";
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

       static uint32_t  getRnd() {
                #ifdef ESP32
                        return esp_random();
                #endif
                #ifdef ESP8266
                        return (uint32_t) os_random();
                #endif
                

        }

	static String getSubstring(String string, char separator, int index) {
	    int found = 0;
	    int range[] = { 0, -1 };
	    int maxIndex = string.length() - 1;

    	for (int i = 0; i <= maxIndex && found <= index; i++) {
        	if (string.charAt(i) == separator || i == maxIndex) {
            	found++;
            	range[0] = range[1] + 1;
            	range[1] = (i == maxIndex) ? i+1 : i;
        	}
    	}
    	return found > index ? string.substring(range[0], range[1]) : "";
	}

	static String format(String s, Controller* controller) {
		String out = "";
		int status = 0;
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
					// if (c == 'n') {
					// 	show(out);
					// 	out = "";
					// }
					status = 0;
					break;
			}
		}
		return out;
	}
};

#endif /* UTILS_H_ */
