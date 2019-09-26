
#ifndef ISTATUS_H_
#define ISTATUS_H_

#include "Arduino.h"
#include "IInternalStatusCallback.h"
#include "INamed.h"

/**  Name + getInternalStatus(key)
 * 
 */


class IStatus : public INamed{
public:
	IStatus();
	virtual ~IStatus();
	virtual void getInternalStatus(IInternalStatusCallback* cb, String key);
};

#endif /* ISTATUS_H_ */
