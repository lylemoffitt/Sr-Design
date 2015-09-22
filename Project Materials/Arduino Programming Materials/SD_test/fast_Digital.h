/*
 * fast_Digital.h
 *
 *  Created on: Feb 1, 2015
 *      Author: Lyle
 */

#ifndef FAST_DIGITAL_H_
#define FAST_DIGITAL_H_

#include <Arduino.h>

#define DIGITAL_READ_REGISTER PIND
#define DIGITAL_WRITE_REGISTER PORTD
#define DIGITAL_ENABLE_REGISTER DDRD

typedef union {
	uint8_t value;
	struct{
		unsigned D0 : 1;
		unsigned D1 : 1;
		unsigned D2 : 1;
		unsigned D3 : 1;

		unsigned D4 : 1;
		unsigned D5 : 1;
		unsigned D6 : 1;
		unsigned D7 : 1;
	};
}__attribute__((packed)) Byte_Mask;

//static_assert(Byte_Mask(0x10000000).D7 && Byte_Mask(0x00000001).D0 , "Bit order mismatch");


void inline
fast_digitalWrite(unsigned PinNum, bool Val)
{
	bitWrite( DIGITAL_WRITE_REGISTER,PinNum,Val);
}

bool inline
fast_digitalRead(unsigned PinNum, bool Val)
{
	return bitRead( DIGITAL_READ_REGISTER,PinNum);
}


void inline
fast_digitalWrite(Byte_Mask mask)
{
	DIGITAL_WRITE_REGISTER = mask.value;
}

Byte_Mask inline
fast_digitalRead()
{
	Byte_Mask mask;
	mask.value = DIGITAL_READ_REGISTER;
	return mask;
}

inline
void fast_pinMode(uint8_t pin, uint8_t mode)
{
	uint8_t bit = digitalPinToBitMask(pin);
	uint8_t port = digitalPinToPort(pin);
	volatile uint8_t *reg, *out;

	if (port == NOT_A_PIN) return;

	// JWS: can I let the optimizer do this?
	reg = portModeRegister(port);
	out = portOutputRegister(port);

	if (mode == INPUT) {
		uint8_t oldSREG = SREG;
                cli();
		*reg &= ~bit;
		*out &= ~bit;
		SREG = oldSREG;
	} else if (mode == INPUT_PULLUP) {
		uint8_t oldSREG = SREG;
                cli();
		*reg &= ~bit;
		*out |= bit;
		SREG = oldSREG;
	} else {
		uint8_t oldSREG = SREG;
                cli();
		*reg |= bit;
		SREG = oldSREG;
	}
}


#endif /* FAST_DIGITAL_H_ */
