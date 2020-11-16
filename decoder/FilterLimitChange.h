/*
 * FILTERLIMITCHANGE.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef FILTERLIMITCHANGE_H_
#define FILTERLIMITCHANGE_H_

#include "Connectors.h"
#include "IFilter.h"

class FilterLimitChange : public IFilter, public ILoop {
public:
	FilterLimitChange(int rate, String name);
	virtual ~FilterLimitChange();
	virtual int loop();
	virtual void setSettings(String key, String value);

private:
	int16_t value;	
	float current;
	String name;
	float rate;
};

#endif /* FILTERLIMITCHANGE_H_ */
