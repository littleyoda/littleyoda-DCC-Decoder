/*
 based on the SPI.cpp - SPI library for esp8266
 Modifiered for non blocking.


 original Licence:

 Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef ESP8266

#include "SPINonBlocking.h"

#include "HardwareSerial.h"
#include "GPIO.h"

typedef union {
	uint32_t regValue;
	struct {
		unsigned regL :6;
		unsigned regH :6;
		unsigned regN :6;
		unsigned regPre :13;
		unsigned regEQU :1;
	};
} spiClk_t;

SPINonClockingClass mySPI;

SPINonClockingClass::SPINonClockingClass() {
}


void SPINonClockingClass::begin(SPISettings settings, String funktion) {
	GPIOobj.pinMode(MOSI, SPECIAL, funktion + " SPI"); ///< GPIO13
	if (settings._SCK) {
		GPIOobj.pinMode(SCK, SPECIAL, funktion + " SPI/CLOCK"); ///< GPIO14
	}
	SPI1C = 0;
	setFrequency(1000000); ///< 1MHz
	SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
	SPI1U1 = (7 << SPILMOSI) | (7 << SPILMISO);
	SPI1C1 = 0;
}

void SPINonClockingClass::end() {
	GPIOobj.pinMode(MOSI, OUTPUT, "SPI");
}


void SPINonClockingClass::beginTransaction(SPISettings settings) {
	while(SPI1CMD & SPIBUSY) {}
	setFrequency(settings._clock);
	setBitOrder(settings._bitOrder);
	setDataMode(settings._dataMode);
}

void SPINonClockingClass::endTransaction() {
	SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
}

void SPINonClockingClass::setDataMode(uint8_t dataMode) {

	/**
     SPI_MODE0 0x00 - CPOL: 0  CPHA: 0
     SPI_MODE1 0x01 - CPOL: 0  CPHA: 1
     SPI_MODE2 0x10 - CPOL: 1  CPHA: 0
     SPI_MODE3 0x11 - CPOL: 1  CPHA: 1
	 */

	bool CPOL = (dataMode & 0x10); ///< CPOL (Clock Polarity)
	bool CPHA = (dataMode & 0x01); ///< CPHA (Clock Phase)

	if(CPHA) {
		SPI1U |= (SPIUSME);
	} else {
		SPI1U &= ~(SPIUSME);
	}

	if(CPOL) {
		SPI1P |= 1<<29;
	} else {
		SPI1P &= ~(1<<29);
		//todo test whether it is correct to set CPOL like this.
	}

}

void SPINonClockingClass::setBitOrder(uint8_t bitOrder) {
	if(bitOrder == MSBFIRST) {
		SPI1C &= ~(SPICWBO | SPICRBO);
	} else {
		SPI1C |= (SPICWBO | SPICRBO);
	}
}

/**
 * calculate the Frequency based on the register value
 * @param reg
 * @return
 */
static uint32_t ClkRegToFreq(spiClk_t * reg) {
	return (ESP8266_CLOCK / ((reg->regPre + 1) * (reg->regN + 1)));
}

void SPINonClockingClass::setFrequency(uint32_t freq) {
	static uint32_t lastSetFrequency = 0;
	static uint32_t lastSetRegister = 0;

	if(freq >= ESP8266_CLOCK) {
		setClockDivider(0x80000000);
		return;
	}

	if(lastSetFrequency == freq && lastSetRegister == SPI1CLK) {
		// do nothing (speed optimization)
		return;
	}

	const spiClk_t minFreqReg = { 0x7FFFF000 };
	uint32_t minFreq = ClkRegToFreq((spiClk_t*) &minFreqReg);
	if(freq < minFreq) {
		// use minimum possible clock
		setClockDivider(minFreqReg.regValue);
		lastSetRegister = SPI1CLK;
		lastSetFrequency = freq;
		return;
	}

	uint8_t calN = 1;

	spiClk_t bestReg = { 0 };
	int32_t bestFreq = 0;

	// find the best match
	while(calN <= 0x3F) { // 0x3F max for N

		spiClk_t reg = { 0 };
		int32_t calFreq;
		int32_t calPre;
		int8_t calPreVari = -2;

		reg.regN = calN;

		while(calPreVari++ <= 1) { // test different variants for Pre (we calculate in int so we miss the decimals, testing is the easyest and fastest way)
			calPre = (((ESP8266_CLOCK / (reg.regN + 1)) / freq) - 1) + calPreVari;
			if(calPre > 0x1FFF) {
				reg.regPre = 0x1FFF; // 8191
			} else if(calPre <= 0) {
				reg.regPre = 0;
			} else {
				reg.regPre = calPre;
			}

			reg.regL = ((reg.regN + 1) / 2);
			// reg.regH = (reg.regN - reg.regL);

			// test calculation
			calFreq = ClkRegToFreq(&reg);
			//os_printf("-----[0x%08X][%d]\t EQU: %d\t Pre: %d\t N: %d\t H: %d\t L: %d = %d\n", reg.regValue, freq, reg.regEQU, reg.regPre, reg.regN, reg.regH, reg.regL, calFreq);

			if(calFreq == (int32_t) freq) {
				// accurate match use it!
				memcpy(&bestReg, &reg, sizeof(bestReg));
				break;
			} else if(calFreq < (int32_t) freq) {
				// never go over the requested frequency
				if(abs(freq - calFreq) < abs(freq - bestFreq)) {
					bestFreq = calFreq;
					memcpy(&bestReg, &reg, sizeof(bestReg));
				}
			}
		}
		if(calFreq == (int32_t) freq) {
			// accurate match use it!
			break;
		}
		calN++;
	}

	// os_printf("[0x%08X][%d]\t EQU: %d\t Pre: %d\t N: %d\t H: %d\t L: %d\t - Real Frequency: %d\n", bestReg.regValue, freq, bestReg.regEQU, bestReg.regPre, bestReg.regN, bestReg.regH, bestReg.regL, ClkRegToFreq(&bestReg));

	setClockDivider(bestReg.regValue);
	lastSetRegister = SPI1CLK;
	lastSetFrequency = freq;

}

void SPINonClockingClass::setClockDivider(uint32_t clockDiv) {
	if(clockDiv == 0x80000000) {
		GPMUX |= (1 << 9); // Set bit 9 if sysclock required
	} else {
		GPMUX &= ~(1 << 9);
	}
	SPI1CLK = clockDiv;
}

inline void SPINonClockingClass::setDataBits(uint16_t bits) {
	const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
	bits--;
	SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}




/**
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
void SPINonClockingClass::send(uint8_t * data, uint8_t size) {
	if(size > 64) return; //max Hardware FIFO

	while(SPI1CMD & SPIBUSY) {}

	uint32_t buffer[16];
	uint8_t *bufferPtr=(uint8_t *)&buffer;
	uint8_t *dataPtr = data;
	volatile uint32_t * fifoPtr = &SPI1W0;
	uint8_t r;
	uint32_t repeatRem;
	uint8_t i;

	dataPtr = data;
	for(i=0; i<size; i++){
		*bufferPtr = *dataPtr;
		bufferPtr++;
		dataPtr++;
	}

	// Anzahl der 4 Bytes Chunks bestimmen.
	repeatRem = size;
	r = repeatRem;

	if (r & 3) {
		r = r / 4 + 1;
	} else {
		r = r / 4;
	}
	for(i=0; i<r; i++){
		*fifoPtr = buffer[i];
		fifoPtr++;
	}
	SPI1U = SPIUMOSI | SPIUSSE;
	//End orig
	setDataBits(repeatRem * 8);
	SPI1CMD |= SPIBUSY;
	//while(SPI1CMD & SPIBUSY) {}

	//	SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
}


bool SPINonClockingClass::busy() {
	return SPI1CMD & SPIBUSY;
}

int SPINonClockingClass::getUsedPin() {
	return MOSI;
}

int SPINonClockingClass::getSCKPin() {
	return SCK;
}

#endif
