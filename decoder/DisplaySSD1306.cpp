/*
 * DISPLAYSSD1306.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "DisplaySSD1306.h"
#include <Wire.h>
#include <Adafruit_GFX.h>


DisplaySSD1306::DisplaySSD1306() {
		display = new Adafruit_SSD1306(-1);
		display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
	    display->display();
}

DisplaySSD1306::~DisplaySSD1306() {
	// TODO Auto-generated destructor stub
}

void DisplaySSD1306::clear() {
	display->clearDisplay();
  	display->setTextSize(1);
  	display->setTextColor(WHITE);
	display->setCursor(0,0);
}


void DisplaySSD1306::println(String s) {
	display->println(s);
}

void DisplaySSD1306::show() {
	display->display();
}