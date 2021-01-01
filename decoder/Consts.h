/*
 * Consts.h
 *
 *  Created on: 15.12.2016
 *      Author: sven
 */


#ifndef CONSTS_H_
#define CONSTS_H_

#include <Arduino.h>

const char compile_date[] = "Compiledate: "
							__DATE__
							" "
							__TIME__
							" {NONGITVERSION}";
const char gitversion[] = "{NONGITVERSION}";

class Consts {
public:
	static int DEFAULTSPEEDSTEPS;
	const static int SPEED_EMERGENCY = 0;
	const static int SPEED_STOP = 1;
	const static int SPEED_FORWARD = 1;
	const static int SPEED_REVERSE = -1;

	const static int LOCID_ALL = -1;


	const static int SOURCE_DCC = 1;
	const static int SOURCE_WLAN = 0;
	const static int SOURCE_INTERNAL = 2;
	const static int SOURCE_Z21SERVER = 3;
	const static int SOURCE_ROCRAIL = 4;
	const static int SOURCE_RCKP = 5;
	const static int SOURCE_LOCCONTROLLER = 5;

	const static uint16_t DISABLE = 9999;
};



#endif /* CONSTS_H_ */
