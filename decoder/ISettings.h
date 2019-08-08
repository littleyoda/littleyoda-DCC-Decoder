/*
 * ISettings.h
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#ifndef ISETTINGS_H_
#define ISETTINGS_H_

#include "IStatus.h"
class Controller;

class ISettings : public IStatus {
public:
	ISettings();
	virtual ~ISettings();
	virtual void setSettings(String key, String value);
	virtual String getHTMLCfg(String urlprefix);
	virtual void getHTMLConfig(String urlprefix, Controller* c);
	virtual String getHTMLController(String urlprefix);
};

#endif /* ISETTINGS_H_ */
