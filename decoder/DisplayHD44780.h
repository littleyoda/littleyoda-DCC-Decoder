/*
 * DISPLAYHD44780.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef DISPLAYHD44780_H_
#define DISPLAYHD44780_H_
#include <LiquidCrystal.h>

#include "DisplayBase.h"
#include "LinkedList.h"

class DisplayHD44780 : public DisplayBase {
public:
	DisplayHD44780(LinkedList<int>* list);
	virtual ~DisplayHD44780();
	virtual void clear();
	virtual void println(String s);
	virtual void show();
	virtual int columns() { return 16; };
	virtual int rows() { return 2; };
private:
	LiquidCrystal* lcd;
	int y = 0;
	String empty = "                ";


};

#endif /* DISPLAYHD44780_H_ */
