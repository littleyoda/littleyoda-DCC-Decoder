/*
 * DISPLAYHD44780.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef DISPLAYHD44780_H_
#define DISPLAYHD44780_H_
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

#include "DisplayBase.h"
#include "LinkedList.h"

class DisplayHD44780 : public DisplayBase {
public:
	DisplayHD44780(LinkedList<int>* list);
	virtual ~DisplayHD44780();
	virtual void clear() override;
	virtual int columns() override { return 16; };
	virtual int rows() override { return 2; };
	virtual void print(char c) override;
	virtual void setPos(int x, int y) override;
private:
	LCD* lcd;
	int y = 0;
	String empty = "                ";


};

#endif /* DISPLAYHD44780_H_ */
