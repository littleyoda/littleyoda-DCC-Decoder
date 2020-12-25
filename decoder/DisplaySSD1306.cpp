/*
 * DISPLAYSSD1306.cpp
 * 
 * I²C Interface
 * 
 * D1 mini 	GPIO 	Shield
 * D1 	    5 	    SCL
 * D2 	    4 	    SDA
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "DisplaySSD1306.h"
#include <Wire.h>
#include <Adafruit_GFX.h>


DisplaySSD1306::DisplaySSD1306(int rotation) {
		display = new Adafruit_SSD1306(-1);
		display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
		switch (rotation) {
			case 0: display->setRotation(0); break;
			case 90: display->setRotation(1); rotate=true; break;
			case 180: display->setRotation(2); break;
			case 270: display->setRotation(3); rotate=true; break;

		}
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