/*
 * INAMED.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INAMED_H_
#define INAMED_H_

#include <Arduino.h>

class INamed {
public:
	INamed();
	virtual ~INamed();
	virtual String getModulName() { return modulname; };
	virtual String getConfigDescription() { return cfgDesc; };
	virtual void setName(String s);
	virtual String getName();
	virtual void setModulName(String name);
	virtual void setConfigDescription(String desc);
	virtual String createDebugDiagramm(String parent);
private:	
	String* id;
	String modulname;
	String cfgDesc;

};

#endif /* INAMED_H_ */
