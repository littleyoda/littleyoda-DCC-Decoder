
#ifndef ISTATUS_H_
#define ISTATUS_H_

#include "Arduino.h"
#include "IInternalStatusCallback.h"

/**  Name + getInternalStatus(key)
 * 
 */


class IStatus {
public:
	IStatus();
	virtual ~IStatus();
	virtual void setName(String s);
	virtual String getName();
	virtual void getInternalStatus(IInternalStatusCallback* cb, String key);
protected:
	String* id;
};

#endif /* ISTATUS_H_ */
