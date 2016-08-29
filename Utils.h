/*
 * Utils.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "Logger.h"

#define set_bit(var, bitIdx) ((var) |= (1 << (bitIdx)))
#define bit_is_set(var, bitIdx) ((var) & (1 << (bitIdx)))
#define clear_bit(var, bitIdx) ((var) &= (unsigned)~(1 << (bitIdx)))


class Utils {
public:

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
					String("Unbekannte PIN in Config: ") + pin);
			return -1;
		}
	}

};

#endif /* UTILS_H_ */
