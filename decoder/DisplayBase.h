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
	virtual int columns() { return 0; };
	virtual int rows() { return 0; };
};

#endif /* DISPLAYBASE_H_ */
