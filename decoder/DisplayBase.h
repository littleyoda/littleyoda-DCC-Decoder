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
	virtual void println(String s) {};
	virtual void show() {};
	virtual int rows() { return _rows; };
	virtual int columns() { return _cols; };
public:
	int _rows = 0;
	int _cols = 0;
};

#endif /* DISPLAYBASE_H_ */
