/*
 * CmdSenderDCC.cpp
 *
 * Based on the work of Juian Zimmermann
 * http://www.spassbahn.de/forum/index.php?thread/11462-spa%C3%9Flan-topfschlagen-im-minenfeld/&postID=119804&highlight=julian%2Bdcc#post119804
 */
#ifdef ESP8266

#include <Arduino.h>
#include "Consts.h"
#include "ActionDCCGeneration.h"
#include "Logger.h"
#include "Utils.h"
#include "GPIO.h"
#include "SPINonBlocking.h"


ActionDCCGeneration::ActionDCCGeneration(Pin* gpio, int locoaddr, int dccoutput) {
	DCC_ADRESSE = dccoutput;
	LOCO_ADR = locoaddr;
	enableGpio = gpio;
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting DCC Generator");
	Logger::getInstance()->addToLog(LogLevel::INFO, "DCC-Output:" + GPIOobj.gpio2string(mySPI.getUsedPin())
									+ " Enabled: " + enableGpio->toString()
									+ " Loko-Adresse: " + String(LOCO_ADR)
									+ " genutzte DCC Adresse: " + String(DCC_ADRESSE)
	);
	SPISettings spi = SPISettings(17241, LSBFIRST, my_SPI_MODE3, false) ;
	mySPI.begin(spi, "DCC");
	mySPI.beginTransaction(spi);
	setName("DCCGEN");
	if (enableGpio->getPin() != Consts::DISABLE) {
		GPIOobj.pinMode(enableGpio, OUTPUT, "DCC Generation");
		GPIOobj.digitalWrite(enableGpio, 0);
	} else {
		// Keine Enable-Pin => also immer aktiv
		trackenabled = true;
	}

	r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = LOCO_ADR;
}

ActionDCCGeneration::~ActionDCCGeneration() {
	mySPI.end();
}


/**
 * Wenn der SPI-Bus frei ist, neues Paket erzeugen und verschicken
 */
int ActionDCCGeneration::loop() {
	if (mySPI.busy()) {
		return 0;
	}
	if (trackenabled) {
		// State => DCC => SPI-Buffer
		uint8_t len = STATEtoDCC();
		DCCtoSPI(len);
	} else {
		// SPI auf LOW halten
		SPIBufUsed = 16;
		for (int i = 0; i < SPIBufUsed; i++) {
			SPIBuf[i] = 0;
		}
	}

	// Übertragungszeit berechnen
	int waiting = (int)((1.0 / 17241.0) * SPIBufUsed * 8.0 * 1000) - 1;

	// Versenden
	send();
	return waiting;
}



void ActionDCCGeneration::DCCFunc(int id, unsigned long int newvalue, int source) {
	if (id == LOCO_ADR || id == Consts::LOCID_ALL) {
		FUNC_STATE = newvalue;
	}
}


void ActionDCCGeneration::DCCSpeed(int id, int speed, int direction, int source) {
	if (id == LOCO_ADR || id == Consts::LOCID_ALL) {
		if (enableGpio->getPin() != Consts::DISABLE) {
			Logger::log(LogLevel::WARNING, "Emergency? " + String(speed == Consts::SPEED_EMERGENCY));
			if (speed == Consts::SPEED_EMERGENCY) {
				trackenabled = false;
				GPIOobj.digitalWrite(enableGpio, 0); // disable Track
			} else {
				trackenabled = true;
				GPIOobj.digitalWrite(enableGpio, 1); // Enable Track
			}
		} else {
			trackenabled = true; // Always true
		}
		if (speed == Consts::SPEED_EMERGENCY || speed == Consts::SPEED_STOP) {
			speed = 0;
		}
		DIR_STATE = (direction == Consts::SPEED_FORWARD);
		SPEED_STATE = speed; // TODO SpeedSteps berücksichtigen
	}
}



uint8_t ActionDCCGeneration::STATEtoDCC(){
	uint8_t idx = 0;
	if (cv_sendCounter > 0) {
		cv_sendCounter--;
		idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_CV);
	} else {
		switch(FRAME_COUNT) {
		case 0:
			idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_SPEED);
			break;
		case 1:
			idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_FUNC1);
			break;
		case 2:
			idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_FUNC2);
			break;
		case 3:
			idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_FUNC3);
			break;
		case 4:
			idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_FUNC4);
			break;
		case 5:
			idx = createDCCSequence(DCC_ADRESSE,DCC_FRAME_FUNC5);
			break;
		}
		FRAME_COUNT++;
		if (FRAME_COUNT == 6) {
			FRAME_COUNT = 0;
		}
	}
	return idx;
}


uint8_t ActionDCCGeneration::createDCCSequence(uint16_t address, unsigned char framekind) {
	uint8_t i = 0;
	uint8_t idx = 0;

	if (FORCE_LONG_ADDR || address > 128){         // Lange Adresse, oder über 128 (2 byte)
		DCCBuf[idx] = address / 256;
		DCCBuf[idx] = DCCBuf[idx] | 192;
		idx++;
		DCCBuf[idx] = address & 0xFF;
		idx++;
	}
	else{                                 // Kurze Adresse
		DCCBuf[idx] = address & 0x7F;
		idx++;
	}

	switch(framekind){

	case DCC_FRAME_SPEED:                   // Geschwindigkeit oder Richtung ändern
		if (SPEEDSTEP == 2){                  // 128 Geschwindigkeitsstufen
			DCCBuf[idx] = 0x3F;                   // Erweitertes Befehlsbyte
			idx++;
			DCCBuf[idx] = SPEED_STATE;
			bitWrite(DCCBuf[idx],7,DIR_STATE);                  // Richtungsbit in Bit7 übernehmen
			idx++;
		}
		break;
	case DCC_FRAME_FUNC1:                                 // F0 bis F4 ändern (DATA1: 1 0 0 F0 F4 F3 F2 F1)
		i = FUNC_STATE & 0x1E;                              // Bit F1..F4 maskieren
		DCCBuf[idx] = i >> 1;                                 // auf Bit0:F1 Bit1:F2 ... verschieben
		bitWrite(DCCBuf[idx],4,bitRead(FUNC_STATE,0));        // Bit4:F0(LV,LH) aus FUNC_STATE lesen
		DCCBuf[idx] = DCCBuf[idx] | 0x80;                       // Optionswert: FRAME_FUNC1
		idx++;
		break;
	case DCC_FRAME_FUNC2:                                 // F5 bis F8 ändern (DATA1: 1 0 1 1 F8 F7 F6 F5)
		i = FUNC_STATE >> 5;                                // F5 bis F8 aus Status laden und
		i = i & 0x0F;                                           // auf 4 bit maskieren
		DCCBuf[idx] = i;
		DCCBuf[idx] = DCCBuf[idx] | 0xB0;                                   // Optionswert: FRAME_FUNC2
		idx++;
		break;
	case DCC_FRAME_FUNC3:                                 // F9 bis F12 ändern (DATA1: 1 0 1 0 F12 F11 F10 F9)
		i = FUNC_STATE >> 9;                                // F9 bis F12 aus Status laden und
		i = i & 0x0F;                                           // auf 4 bit maskieren
		DCCBuf[idx] = i;
		DCCBuf[idx] = DCCBuf[idx] | 0xA0;                                   // Optionswert: FRAME_FUNC3
		idx++;
		break;
	case DCC_FRAME_FUNC4:                                 // F13 bis F20 ändern (DATA1: 1101 111 0 DATA2: F20 F19 F18 F17 F16 F15 F14 F13)
		DCCBuf[idx] = 0xDE;
		idx++;
		i = FUNC_STATE >> 13;                               // F13 bis F20 aus Status laden und
		i = i & 0xFF;                                           // auf 8 bit maskieren (eigentlich überflüssig)
		DCCBuf[idx] = i;
		idx++;
		break;
	case DCC_FRAME_FUNC5:                                 // F21 bis F28 ändern (DATA1: 1101 111 1 DATA2: F28 F27 F26 F25 F24 F23 F22 F21)
		DCCBuf[idx] = 0xDF;
		idx++;
		i = FUNC_STATE >> 21;                               // F21 bis F28 aus Status laden und
		i = i & 0xFF;                                           // auf 8 bit maskieren (eigentlich überflüssig)
		DCCBuf[idx] = i;
		idx++;
		break;
	case DCC_FRAME_CV:  
		// 1110CCVV   0   VVVVVVVV   0   DDDDDDDD	
		// CC 11 => Byte Write
		uint16_t cv = cv_cv - 1;
		DCCBuf[idx] = 0b11101100 | ((cv >> 8) & 0b11);
		idx++;
		DCCBuf[idx] = cv & 0xFF;
		idx++;
		DCCBuf[idx] = cv_value & 0xFF;
		idx++;
		break;
	}
	DCCBuf[idx] = DCCBuf[0]; // Platzhalter für XOR
	for (i = 1; i <= idx - 1; i++){                           //XOR Zusammenstellen
		DCCBuf[idx] = DCCBuf[idx] ^ DCCBuf[i];
	}
	return idx;
}

void ActionDCCGeneration::addToSpi(int i) {
	if (i == 1) {
		// wenn 1, dann 10
		spicache = spicache >> 2;
		spicache = spicache | 0x4000;
		spicacheUsed = spicacheUsed + 2;
	} else if (i == 0) {
		// wenn 0, dann 1100
		spicache = spicache >> 4;
		spicache = spicache | 0x3000;
		spicacheUsed = spicacheUsed + 4;
	}

	// Daten ggf. in den SPI-Buffer schieben
	if (spicacheUsed > 8){                                           // Wenn mehr als 8 Bit im Cache liegen, wird in Buffer übertragen
		uint16_t n16 = 0;
		n16 = spicache << (spicacheUsed - 8);                          // 8 LSBs aus Cache zum Buffer
		n16 = n16 >> 8;
		SPIBuf[SPIBufUsed] = n16 & 0xFF;
		SPIBufUsed++;
		spicacheUsed = spicacheUsed - 8;
	}
}


/**
 * DCC Datenstrom für SPI übersetzen
 */
void ActionDCCGeneration::DCCtoSPI(uint8_t idx) {

	uint8_t i = 0;
	uint8_t k = 0;

	spicache = 0;
	spicacheUsed = 0;

	// DCC Preambel aus 12 * 0x8000 (12 * "10")
	for (i = 0; i < 16; i++){
		addToSpi(1);
	}

	// Packet Start Bit
	addToSpi(0);

	// Convert DCCBuffer to SPI-Data
	for (i = 0; i <= idx; i++){

		// Bits aus DCC Buffer auslesen
		for (k = 0; k <= 7; k++){
			if (bitRead(DCCBuf[i], 7 - k) == 1){
				addToSpi(1);
			} else {
				addToSpi(0);
			}
		}

		//
		if (i < idx){
			// Data Byte Start Bit
			addToSpi(0);
		} else {
			// Packet End bit
			addToSpi(1);
		}
	}
	// Zusätzliches Bit, damit sichergestellt ist, dass das "Packet End Bit" korrekt übertragen wurde
	addToSpi(1);


	// Rest und unvollständiges Byte in den SPI-Buffer schieben und spicache leeren
	if ( spicacheUsed > 0){
		uint16_t n16 = 0;
		n16 = spicache >> (8 - spicacheUsed);
		n16 = n16 >> 8;
		SPIBuf[SPIBufUsed] = n16 & 0xFF;
		SPIBufUsed++;
		spicacheUsed=0;
	}

}

void ActionDCCGeneration::send() {
	if (SPIBufUsed == 0) {
		return;
	}
	mySPI.send(SPIBuf, SPIBufUsed);
	SPIBufUsed = 0;
}


void ActionDCCGeneration::getHTMLConfig(String urlprefix, Controller* c) {
        String message =  "<div class=\"row\"> <div class=\"column column-10\">";
        message += "DCC Konfigurator (DCC-Adresse: " + String(DCC_ADRESSE) + ")";
        message += "</div><div class=\"column column-90\">";


        String action = "send('" + urlprefix + "key=cv&value=' + (Number(document.getElementById('dcc_cv').value) * 1000 + Number(document.getElementById('dcc_value').value)))";
 
		message += "<form id=\"myform\" >";
		message += "CV: <input type=\"number\" min=\"0\" max=\"1000\" id=\"dcc_cv\" /><br> Wert:<input min=\"0\" max=\"255\" type=\"number\" id=\"dcc_value\" /><button type = \"button\" onclick=\"" + action + "\">Befehl abschicken</button> ";
		message += "</form>";
        message += "</div>";
        message += "</div>";
        c->sendContent(message);
}

void ActionDCCGeneration::setSettings(String key, String value) {
	if (key == "cv") {
		long l = value.toInt();
		cv_sendCounter = 2;
		cv_cv = l / 1000;
		cv_value = l % 1000;

	}
}

void ActionDCCGeneration::getInternalStatus(IInternalStatusCallback* cb, String key) {
	if (key.equals("*") || key.equals("addr")) {
		cb->send(getName(), "addr", String(LOCO_ADR));
	}
	if (key.equals("*") || key.equals("dccout")) {
		cb->send(getName(), "dccout", String(DCC_ADRESSE));
	}
	if (key.equals("*") || key.equals("trackenabled")) {
		cb->send(getName(), "trackenabled", String(trackenabled));
	}
}


#endif
