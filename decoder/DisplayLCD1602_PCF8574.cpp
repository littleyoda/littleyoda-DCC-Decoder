/*
 * DISPLAYLCD1602_PCF8574.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "DisplayLCD1602_PCF8574.h"

DisplayLCD1602_PCF8574::DisplayLCD1602_PCF8574(int r, int c) : DisplayBase(c, r) {
	if (_rows == 0) {
		_rows = 2;
	}
	if (_cols == 0) {
		_cols = 16;
	}
	lcd = new  LCDIC2(0x27, _cols, _rows);
	bool ret = lcd->begin();
	String out = (ret ? "connected" : "failed");
	Serial.println("LCD1602 " + String(_rows) + "/" + String(_cols) + " " + out); 
}


DisplayLCD1602_PCF8574::~DisplayLCD1602_PCF8574() {
	// TODO Auto-generated destructor stub
}


void DisplayLCD1602_PCF8574::clear() {
	lcd->clear();
	lcd->setCursor(0, 0);
	y = 0;
	x = 0;
}



void DisplayLCD1602_PCF8574::setPos(int x, int y) {
	lcd->setCursor(x, y);
};


void DisplayLCD1602_PCF8574::print(char c) {
	lcd->print(c);
}
