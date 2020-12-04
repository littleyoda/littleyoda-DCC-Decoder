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
#define bit_is_set01(var, bitIdx) (bit_is_set(var, bitIdx) > 0 ? 1 : 0)
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

	static String wifiscan() {
		WiFi.scanNetworks(false, true);
		String out = "";
		int n = WiFi.scanComplete();
		for (int i = 0; i < n; ++i) {
			String message = "";
			String ssid_scan;
			int32_t rssi_scan;
			uint8_t sec_scan;
			uint8_t* BSSID_scan;
			int32_t chan_scan;
			bool hidden_scan = false;

			#ifdef ESP8266
				WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
			#elif ESP32
				WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);
			#endif

			message +="[";
			for (int j = 0; j < 6; j++) {
				if (BSSID_scan[j] < 16) {
					message += "0";
				}
				message += String(BSSID_scan[j], HEX);
				if (i < 5) {
					message +=":";
				}

			}
			message +="] CH:";
			if (chan_scan < 10) {
				message += "0";
			}
			message += String(chan_scan);
			message += " ";
			#ifdef ESP8266
				message += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "OPEN" : "ENC ";
			#elif ESP32
				message += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "ENC ";
			#endif
			message += "  Q:";
			message += String(WiFi.RSSI(i));
			message += "  --- ";
			message += ssid_scan;
			message += "  ";
			if (hidden_scan) {
				message += " (HIDDEN)";
			}
			message += "\r\n";
			out += message;
		}
		WiFi.scanDelete();
		out += "Connected to " + WiFi.BSSIDstr();
		return out;
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

	/**
	 * 
	 * Kombination aus dem Wifi Status (Connected, Disconnected) und Mode (STA, AP, STA_SP)
	 * 
	 */
	static int getExtWifiStatus() {
		int status = WiFi.status();
                #ifdef ESP8266
			status = status | (wifi_get_opmode() << 4);
                #else
			status = status | (WiFi.getMode() << 4);
                #endif
		return status;
	}

	static String extwifi2String(int status) {
		String out = wifi2String(status & 15);
		if (status > 15) {
			status = status >> 4;
			switch (status) {
				case 0x01: // STATION_MODE
					out += " [STA]";
					break;
				case 0x02: // SOFTAP_MODE
					out += " [AP]";
					break;
				case 0x03: // STATIONAP_MODE
					out += " [STA/AÜ]";
					break;
				default:
					out += " [unknown]";
					break;
			}
		}
		return out;
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


	static bool isWifiConnected() {
		return WiFi.status() == WL_CONNECTED || WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA;
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
			Serial.println("=====");
			Serial.println(s.substring(0,i));
			Serial.println(out);
		}
		return out;
	}
};

#endif /* UTILS_H_ */
