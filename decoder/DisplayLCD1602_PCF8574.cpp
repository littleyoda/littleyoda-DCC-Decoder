/*
 * DISPLAYLCD1602_PCF8574.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "DisplayLCD1602_PCF8574.h"

DisplayLCD1602_PCF8574::DisplayLCD1602_PCF8574() {
	lcd = new  LCDIC2(0x27, 16, 2);
	Serial.println("LCD1602 " + String(lcd->begin()));
}


DisplayLCD1602_PCF8574::~DisplayLCD1602_PCF8574() {
	// TODO Auto-generated destructor stub
}


void DisplayLCD1602_PCF8574::clear() {
	lcd->setCursor(0, 0);
	y = 0;
}

void DisplayLCD1602_PCF8574::println(String s) {
	lcd->setCursor(0, y);
	String out = s + empty.substring(0, 16 - s.length());
	lcd->print(out);
	y++;
}

void DisplayLCD1602_PCF8574::show() {

}