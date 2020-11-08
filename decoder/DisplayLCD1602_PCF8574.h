/*
 * DISPLAYLCD1602_PCF8574.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef DISPLAYLCD1602_PCF8574_H_
#define DISPLAYLCD1602_PCF8574_H_


#include "LCDIC2.h"

#include "DisplayBase.h"
class DisplayLCD1602_PCF8574 : public DisplayBase {
public:
	DisplayLCD1602_PCF8574();
	virtual ~DisplayLCD1602_PCF8574();
	virtual void clear();
	virtual void println(String s);
	virtual void show();
	virtual int columns() { return 16; };
	virtual int rows() { return 2; };
private:
	LCDIC2* lcd;
	int y = 0;
	String empty = "                ";
};

#endif /* DISPLAYLCD1602_PCF8574_H_ */
