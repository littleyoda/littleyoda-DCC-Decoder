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
#include "Controller.h"
#include "LinkedList.h"
#include "DisplayBase.h"

class Display : public ILoop {
public:
	Display(Controller* c, String text, String model, LinkedList<int>* list, int rows, int cols, int rotation);
	virtual ~Display();
	int loop();
private:
	Controller* controller;
	int count = 0;
	void fill(String);
	unsigned int maxcount = 10;
	String pattern;
	String model;
	DisplayBase* display;
	int idx = -1;
	int subidx = 0;
	LinkedList<String> output = LinkedList<String>();
};

#endif /* DISPLAY_H_ */
