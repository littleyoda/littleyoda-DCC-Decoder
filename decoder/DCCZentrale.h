/*
 * CmdSenderDCC.h

 * Based on the work of Juian Zimmermann
 * http://www.spassbahn.de/forum/index.php?thread/11462-spa%C3%9Flan-topfschlagen-im-minenfeld/&postID=119804&highlight=julian%2Bdcc#post119804
 *
 */

#ifndef DCCZentrale_H_
#define DCCZentrale_H_
#ifdef ESP8266

#include "INotify.h"
#include "ILoop.h"
#include "ISettings.h"
#include "Pin.h"
#include "IInternalStatusCallback.h"
#include "Controller.h"

class DCCZentrale: public INotify, public ILoop, public ISettings {
public:
	DCCZentrale(Pin* gpioenable, Controller* c);
	virtual ~DCCZentrale();
	virtual int loop();
	virtual void getInternalStatus(IInternalStatusCallback* cb, String key);

private:
	Controller* controller;
	Controller::Items* locdata;
//	int locdatapos = 0;
	void addToSpi(int i);
	uint8_t STATEtoDCC(LocData* data, int id);
	uint8_t createDCCSequence(uint16_t address, LocData* data,unsigned char framekind);
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

	//unsigned DCC_ADRESSE;
	bool FORCE_LONG_ADDR = false;// Erzwingt Lange DCC Adresse. Adresen über 128 werden automatisch als lange Adressen angenommen
	unsigned SPEEDSTEP = 2;// 0: 14 Fahrstufen, 1: 28 Fahrstufen 2: 128 Fahrstufen. Betrifft nur die Ansteuerung des Dekoders.
							// Nur 128 Fahrstufen werden unterstützt!
	//uint16_t LOCO_ADR;

	unsigned char DCCBuf[6];

	uint16_t spicache = 0;
	uint8_t spicacheUsed = 0;

	unsigned char SPIBuf[32];
	uint8_t SPIBufUsed = 0;


	//unsigned long int  FUNC_STATE = 0;
	//boolean DIR_STATE = false;
	boolean trackenabled = false;
	//uint8_t SPEED_STATE = 0;

	Pin* enableGpio;
//	requestInfo* r;

	// uint8_t cv_sendCounter = 0;
	// uint8_t cv_value;
	// uint16_t cv_cv;
	uint16_t itemskeysIdx = 0;
	std::vector<int>* itemskeys;
	bool isEmergency = false;
	void check();
};

#endif

#endif /* DCCZentrale_H_ */
