#ifndef ISETTINGS_H_
#define ISETTINGS_H_

#include "IStatus.h"


class Controller;

/** setSettings + getHTMLConfig + getHTMLConfig
 * 
 * 
 */

class ISettings : public IStatus {
public:
	ISettings();
	virtual ~ISettings();
	virtual void setSettings(String key, String value);
	virtual String getHTMLCfg(String urlprefix);
	virtual void getHTMLConfig(String urlprefix, Controller* c);
	virtual String getHTMLController(String urlprefix);
	virtual String createDebugDiagramm(String parent);
};

#endif /* ISETTINGS_H_ */
