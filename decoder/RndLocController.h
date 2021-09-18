/*
 * RNDLOCCONTROLLER.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef RNDLOCCONTROLLER_H_
#define RNDLOCCONTROLLER_H_

#include "ILoop.h"
#include "Connectors.h"
#include "ISettings.h"


class RndLocController : public ILoop, public Connectors {
public:
	RndLocController(ISettings* a);
	virtual ~RndLocController();
	int loop();
private:
	int speed = 2;
	int dir = 1;

};

#endif /* RNDLOCCONTROLLER_H_ */
