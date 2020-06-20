/*
 * DISPLAYSSD1306.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef DISPLAYSSD1306_H_
#define DISPLAYSSD1306_H_

#include "DisplayBase.h"
#include <Adafruit_SSD1306.h>

class DisplaySSD1306 : public DisplayBase {
public:
	DisplaySSD1306();
	virtual ~DisplaySSD1306();
	virtual void clear();
	virtual void println(String s);
	virtual void show();
	virtual int columns() { return 10; };
	virtual int rows() { return 5; };

private:
	Adafruit_SSD1306* display;
};

#endif /* DISPLAYSSD1306_H_ */
