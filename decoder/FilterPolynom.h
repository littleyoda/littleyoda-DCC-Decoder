/*
 * FILTERPOLYNOM.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 * 
 * m_i * (x - x_diff_i)^y_i
 */


#ifndef FILTERPOLYNOM_H_
#define FILTERPOLYNOM_H_

#include "IFilter.h"
#include "LinkedList.h"
class FilterPolynom : public IFilter {
public:
	FilterPolynom(String key);
	void addPolynom(String typ, float a, float xdiff, float y);
	virtual ~FilterPolynom();
	virtual void setSettings(String key, String value);
private:
	struct polyValues {
		float a;
		float xdiff;
		float y;
	} __attribute__ ((packed));
	String key;
	LinkedList<polyValues*> polynom = LinkedList<polyValues*>();
};

#endif /* FILTERPOLYNOM_H_ */
