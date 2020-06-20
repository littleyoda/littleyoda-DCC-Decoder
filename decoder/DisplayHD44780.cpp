/*
 * DISPLAYHD44780.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "DisplayHD44780.h"

DisplayHD44780::DisplayHD44780(LinkedList<int>* list) {
	lcd = nullptr;
	if (list->size() == 6) {
		//lcd = new LiquidCrystal(D6, D5, D3, D2, D1, D0);
		lcd = new LiquidCrystal(list->get(0), list->get(1), list->get(2), list->get(3), list->get(4), list->get(5));
		lcd->begin(16, 2);
		lcd->clear();
	} else {
		Serial.println("Fehlgeschlagen");
	}
}

DisplayHD44780::~DisplayHD44780() {
}


void DisplayHD44780::clear() {
	if (lcd == nullptr) {
		return;
	}
	lcd->setCursor(0, 0);
	y = 0;
}

void DisplayHD44780::println(String s) {
	if (lcd == nullptr) {
		return;
	}
	lcd->setCursor(0, y);
	String out = s + empty.substring(0, 16 - s.length());
	lcd->print(out);
	y++;
}

void DisplayHD44780::show() {

}
