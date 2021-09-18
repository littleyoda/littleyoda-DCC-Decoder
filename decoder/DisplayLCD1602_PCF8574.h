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
	DisplayLCD1602_PCF8574(int r, int c);
	virtual ~DisplayLCD1602_PCF8574();
	virtual void clear() override;
	virtual void print(char c) override;
	virtual void setPos(int x, int y) override;

private:
	LCDIC2* lcd;
	int x = 0;
	int y = 0;
	String empty = "                ";
};

#endif /* DISPLAYLCD1602_PCF8574_H_ */
