/*
 * SPEEDKONVERTER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "SpeedKonverter.h"
#include "Logger.h"
#include "Consts.h"
SpeedKonverter::SpeedKonverter() {
	// TODO Auto-generated constructor stub

}

SpeedKonverter::~SpeedKonverter() {
	// TODO Auto-generated destructor stub
}


int SpeedKonverter::fromInternal(representations rep, int value) {
	if (value < 0 || value >127) {
		Logger::log(LogLevel::ERROR, "CONV," "Ungültiger Wert " + String(value));
		return 0;
	}
	switch (rep) {
		case repInternal:
			return value; 
		case repDCC128:
			return dcc128[value];
		case repDCC28:
			return dcc28[value];
		case repDCC14:
			return dcc14[value];
		case repPercent:
			return prozent[value];
		default:
			Logger::log(LogLevel::ERROR, "CONV", "Ungültige Rep (fi) " + String(rep));
			return 0;
	}
}



int SpeedKonverter::fromInternal(int  speedSteps, int value) {
	if (value < 0 || value >127) {
		Logger::log(LogLevel::ERROR, "CONV," "Ungültiger Wert " + String(value));
		return 0;
	}
	switch (speedSteps) {
		case 128:
			return dcc128[value];
		case 28:
			return dcc28[value];
		case 14:
			return dcc14[value];
		case 100:
			return prozent[value];
		default:
			Logger::log(LogLevel::ERROR, "CONV", "Ungültige Speedsteps (fi) " + String(speedSteps));
			return 0;
	}
}

String SpeedKonverter::fromInternal(int value) {
	if (value < 0 || value >127) {
		Logger::log(LogLevel::ERROR, "CONV," "Ungültiger Wert " + String(value));
		return "XXX";
	}
	return desc[value];
}


int SpeedKonverter::fromExternal(int speedSteps, int value) {
	switch (speedSteps) {
		case 128:
		case 127:
			for (int idx = 0; idx < 128; idx++) {
				if (dcc128[idx] == value) {
					return idx;
				}
			}
			break;
		case 100:
			for (int idx = 0; idx < 128; idx++) {
				if (prozent[idx] == value) {
					return idx;
				}
			}
			break;
		case 28:
		case 29:
			for (int idx = 0; idx < 128; idx++) {
				if (dcc28[idx] == value) {
					return idx;
				}
			}
		default:
			break;
	}
	Logger::log(LogLevel::ERROR, "CONV," "ext2int: Keine Umrechnung möglich. Value: " + String(value) + " / Speedsteps: " + String(speedSteps));
	return Consts::SPEED_STOP;
}

const  int   SpeedKonverter::internal[128] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127};
const char  * SpeedKonverter::desc[128] = {"ERR","0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","45","46","47","48","49","50","51","52","53","54","55","56","57","58","59","60","61","62","63","64","65","66","67","68","69","70","71","72","73","74","75","76","77","78","79","80","81","82","83","84","85","86","87","88","89","90","91","92","93","94","95","96","97","98","99","100","101","102","103","104","105","106","107","108","109","110","111","112","113","114","115","116","117","118","119","120","121","122","123","124","125","126"};
const int SpeedKonverter::dcc128[128] = {1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127};
const int  SpeedKonverter::dcc28[128] = {1,0,2,2,2,2,18,18,18,18,18,3,3,3,3,19,19,19,19,19,4,4,4,4,20,20,20,20,20,5,5,5,5,21,21,21,21,21,6,6,6,6,22,22,22,22,22,7,7,7,7,23,23,23,23,23,8,8,8,8,24,24,24,24,24,9,9,9,9,9,25,25,25,25,10,10,10,10,10,26,26,26,26,11,11,11,11,11,27,27,27,27,12,12,12,12,12,28,28,28,28,13,13,13,13,13,29,29,29,29,14,14,14,14,14,30,30,30,30,15,15,15,15,15,31,31,31,31};
const int  SpeedKonverter::dcc14[128] = {1,0,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15};
const int  SpeedKonverter::prozent[128] = {-1,0,1,2,3,4,4,5,6,7,8,8,9,10,11,12,12,13,14,15,15,16,17,18,19,19,20,21,22,23,23,24,25,26,26,27,28,29,30,30,31,32,33,34,34,35,36,37,38,38,39,40,41,41,42,43,44,45,45,46,47,48,49,49,50,51,52,52,53,54,55,56,56,57,58,59,60,60,61,62,63,63,64,65,66,67,67,68,69,70,71,71,72,73,74,75,75,76,77,78,78,79,80,81,82,82,83,84,85,86,86,87,88,89,89,90,91,92,93,93,94,95,96,97,97,98,99,100};
