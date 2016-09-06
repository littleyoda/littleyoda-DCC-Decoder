/*
 * ActionDFPlayerMP3.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef ACTIONDFPLAYERMP3_H_
#define ACTIONDFPLAYERMP3_H_

#include <SoftwareSerial.h>
#include "ActionBase.h"

class ActionDFPlayerMP3: public ActionBase {
public:
	ActionDFPlayerMP3(int s1, int s2);
	virtual void loop();
	virtual ~ActionDFPlayerMP3();
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);

private:
	SoftwareSerial* mp3Serial;
	uint8_t play1[10] =
			{ 0X7E, 0xFF, 0x06, 0X03, 00, 00, 0x01, 0xFE, 0xF7, 0XEF };
};
;

#endif /* ACTIONDFPLAYERMP3_H_ */
