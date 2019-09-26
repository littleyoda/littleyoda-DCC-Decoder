
#ifndef CONNECTORS_H_
#define CONNECTORS_H_

#include <Arduino.h>
#include <LinkedList.h>
#include <ISettings.h>

#include "INotify.h"
#include "INamed.h"


/** Connector Klasse (Input: INotify Output: Send to ISettings)
 *  
 * Reagiert auf Änderungen (Inotify), 
 * leidet aus den Änderungen Werte ab und übergibt
 * die Werte an die ISettings-Klassen
 * 
 */


class Connectors: public INotify, public INamed {
public:
	Connectors();
	virtual ~Connectors();
	virtual LinkedList<ISettings*>* getActions();
	virtual String createDebugDiagramm(String parent);

protected:
	LinkedList<ISettings*> actions;
	void addAction(ISettings* a);
	void send(String key, String value);
};


#endif /* CONNECTORS_H_ */
