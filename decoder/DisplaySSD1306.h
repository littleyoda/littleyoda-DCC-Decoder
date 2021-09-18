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
	DisplaySSD1306(int rotation);
	virtual ~DisplaySSD1306();
	virtual void clear() override;
	virtual void print(char c) override;
	virtual void show() override;
	virtual int columns() override { return rotate?5:10; } ;
	virtual int rows() override { return rotate?10:5; };
	virtual void setPos(int x, int y) override;

private:
	bool rotate = false;
	Adafruit_SSD1306* display;
};

#endif /* DISPLAYSSD1306_H_ */
