/*
 * CmdSenderDCC.h

 * Based on the work of Juian Zimmermann
 * http://www.spassbahn.de/forum/index.php?thread/11462-spa%C3%9Flan-topfschlagen-im-minenfeld/&postID=119804&highlight=julian%2Bdcc#post119804
 *
 */

#ifndef ACTIONDCCGENERATION_H_
#define ACTIONDCCGENERATION_H_
#ifdef ESP8266

#include "INotify.h"
#include "ILoop.h"
#include "ISettings.h"
#include "Pin.h"

class ActionDCCGeneration: public INotify, public ILoop, public ISettings {
public:
	ActionDCCGeneration(Pin* gpioenable, int locoaddr, int dccoutput);
	virtual ~ActionDCCGeneration();
	virtual int loop();
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	virtual void DCCFunc(int id, unsigned long int newvalue, int source);
	virtual void getHTMLConfig(String urlprefix, Controller* c);
	virtual void setSettings(String key, String value);
	virtual String getInternalStatus(String key);

private:
	void addToSpi(int i);
	uint8_t STATEtoDCC();
	uint8_t createDCCSequence(uint16_t address, unsigned char framekind);
	void DCCtoSPI(uint8_t idx);
	void send();

	uint8_t FRAME_COUNT = 0;

	static const unsigned char DCC_FRAME_SPEED =0x10;
	static const unsigned char DCC_FRAME_FUNC1 =0x11;
	static const unsigned char DCC_FRAME_FUNC2 =0x12;
	static const unsigned char DCC_FRAME_FUNC3 =0x13;
	static const unsigned char DCC_FRAME_FUNC4 =0x14;
	static const unsigned char DCC_FRAME_FUNC5 =0x15;
	static const unsigned char DCC_FRAME_CV =0x16;

	unsigned DCC_ADRESSE;
	bool FORCE_LONG_ADDR = false;// Erzwingt Lange DCC Adresse. Adresen über 128 werden automatisch als lange Adressen angenommen
	unsigned SPEEDSTEP = 2;// 0: 14 Fahrstufen, 1: 28 Fahrstufen 2: 128 Fahrstufen. Betrifft nur die Ansteuerung des Dekoders.
							// Nur 128 Fahrstufen werden unterstützt!
	uint16_t LOCO_ADR;

	unsigned char DCCBuf[6];

	uint16_t spicache = 0;
	uint8_t spicacheUsed = 0;

	unsigned char SPIBuf[32];
	uint8_t SPIBufUsed = 0;


	unsigned long int  FUNC_STATE = 0;
	boolean DIR_STATE = false;
	boolean trackenabled = false;
	uint8_t SPEED_STATE = 0;

	Pin* enableGpio;
	requestInfo* r;

	uint8_t cv_sendCounter = 0;
	uint8_t cv_value;
	uint16_t cv_cv;
};

#endif

#endif /* ACTIONDCCGENERATION_H_ */
