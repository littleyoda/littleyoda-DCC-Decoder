/*
 * ISettings.h
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#ifndef ISETTINGS_H_
#define ISETTINGS_H_

#include "Arduino.h"
class ISettings {
public:
	ISettings();
	virtual ~ISettings();
	virtual void setName(String s);
	virtual String getName();
	virtual void setSettings(String key, String value);
	virtual String getHTMLCfg(String urlprefix) ;
	virtual String getHTMLController(String urlprefix);
private:
	String id;
};

#endif /* ISETTINGS_H_ */
