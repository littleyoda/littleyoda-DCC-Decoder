/*
 * interfaceLoop.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef INTERFACELOOP_H_
#define INTERFACELOOP_H_

class interfaceLoop {
public:
	interfaceLoop();
	virtual ~interfaceLoop() = 0;
	virtual void loop() = 0;
};

#endif /* INTERFACELOOP_H_ */
