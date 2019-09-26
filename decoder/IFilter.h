/*
 * IFILTER.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef IFILTER_H_
#define IFILTER_H_

#include "ISettings.h"
#include "LinkedList.h"

class IFilter :public ISettings {
public:
	IFilter();
	virtual ~IFilter();
	void addAction(ISettings* a);
	virtual void setSettings(String key, String value);
	virtual String createDebugDiagramm(String parent);

protected:
	LinkedList<ISettings*> actions;
	void send(String key, String value);

};

#endif /* IFILTER_H_ */
