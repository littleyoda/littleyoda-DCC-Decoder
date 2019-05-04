/*
 * interfaceLoop.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef ILOOP_H_
#define ILOOP_H_

class ILoop {
public:
	ILoop();
	virtual ~ILoop();
	virtual int loop();
};

#endif /* ILOOP_H_ */
