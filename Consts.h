/*
 * Consts.h
 *
 *  Created on: 15.12.2016
 *      Author: sven
 */


#ifndef CONSTS_H_
#define CONSTS_H_

#include <Arduino.h>


class Consts {
public:

	const static int SPEED_EMERGENCY = 0;
	const static int SPEED_STOP = 1;
	const static int SPEED_FORWARD = 1;
	const static int SPEED_REVERSE = -1;

	const static int LOCID_ALL = -1;


	const static int SOURCE_DCC = 1;
	const static int SOURCE_WLAN = 0;
	const static int SOURCE_INTERNAL = 2;
	const static int SOURCE_Z21SERVER = 3;

	const static uint8_t DISABLE = 255;

};



#endif /* CONSTS_H_ */
