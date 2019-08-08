/*
 * DISPLAY.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "ILoop.h"
	
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Controller.h"

class Display : public ILoop {
public:
	Display(Controller* c, String text, String model);
	virtual ~Display();
	int loop();
private:
	Adafruit_SSD1306* display;
	Controller* controller;
	int count = 0;
	void show(String);
	String fill(String);
	unsigned int width = 10;
	unsigned int maxcount = 10;
	String pattern;
	String model;


};

#endif /* DISPLAY_H_ */
