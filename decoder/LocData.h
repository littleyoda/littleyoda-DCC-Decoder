/*
 * LOCDATA.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef LOCDATA_H_
#define LOCDATA_H_

	struct LocData {
		int16_t speed; // Intern wird immer mit 0..127 gerechnet!
		int8_t direction;
		int16_t speedsteps; // Nur für die z21 relevant
		unsigned long int status;
	} __attribute__ ((packed));

	struct TurnOutData {
		int8_t direction;
	};

#endif /* LOCDATA_H_ */
