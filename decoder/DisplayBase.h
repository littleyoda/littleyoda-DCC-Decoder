/*
 * DISPLAYBASE.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef DISPLAYBASE_H_
#define DISPLAYBASE_H_

#include <Arduino.h>

class DisplayBase {
public:
	DisplayBase() {};
	virtual ~DisplayBase() {};
	virtual void clear() {};
	virtual void print(char c) {};
	virtual void show() {};
	virtual void setPos(int x, int y) {};
	virtual int rows() { return _rows; };
	virtual int columns() { return _cols; };
public:
	int _rows = 0;
	int _cols = 0;
};

#endif /* DISPLAYBASE_H_ */
