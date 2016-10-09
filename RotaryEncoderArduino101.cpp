// RotaryEncoderArduino101 library example
// IDE: Arduino 1.6.11
// Board support pkg: Intel Curie Boards 1.0.7 (could work on other boards with sufficient IRQs)
// Wiring: rotary encoder 1: common to ground, push to D4, rotary A on D5, rotary B on D6
//         rotary encoder 2: common to ground, push to D7, rotary A on D8, rotary B on D9
// (c) 2016 by Coert Vonk (http://www.coertvonk.com)
// see enclosed LICENSE.txt
// inspired by http://www.embedded.com/design/prototyping-and-development/4023817/Interrupts-in-C-
//         and http://www.instructables.com/id/Improved-Arduino-Rotary-Encoder-Reading/

#include "RotaryEncoderArduino101.h"

typedef void(*isrFnc_t)(void);

void Interrupt::registr(uint8_t const pinNr, Interrupt * intHandlerThis, uint32_t const mode_)
{
	for (uint8_t ii = 0; ii < MAX_INTERRUPTS; ii++) {
		if (isrVectorTable[ii] == NULL) {
			isrVectorTable[ii] = intHandlerThis;
			isrFnc_t fnc;
			switch (ii) {
				case 0: fnc = isr0; break;
				case 1: fnc = isr1; break;
				case 2: fnc = isr2; break;
				case 3: fnc = isr3; break;
				case 4: fnc = isr4; break;
				case 5: fnc = isr5; break; // should match MAX_INTERRUPTS
				default: fnc = NULL;
			}
			if (fnc) {
				attachInterrupt(pinNr, fnc, mode_);
			}
			break;
		}
	}
}

#if 0
void Interrupt::deregistr(uint8_t const pinNr, Interrupt * intHandlerThis)
{
	for (uint8_t ii = 0; ii < MAX_INTERRUPTS; ii++) {
		if (isrVectorTable[ii] == intHandlerThis) {
			isrVectorTable[ii] = NULL;
			detachInterrupt(pinNr);
		}
	}
}
#endif

void Interrupt::isr0(void) { isrVectorTable[0]->isr(); }
void Interrupt::isr1(void) { isrVectorTable[1]->isr(); }
void Interrupt::isr2(void) { isrVectorTable[2]->isr(); }
void Interrupt::isr3(void) { isrVectorTable[3]->isr(); }
void Interrupt::isr4(void) { isrVectorTable[4]->isr(); }
void Interrupt::isr5(void) { isrVectorTable[5]->isr(); }

// definitions for static class variable (if you forget, .. you get odd linker errors)
Interrupt * Interrupt::isrVectorTable[MAX_INTERRUPTS];

RotaryEncoderInterruptA::RotaryEncoderInterruptA(uint8_t const pinNr_, RotaryEncoder* owner_)
{
	owner = owner_;
	pinMode(pinNr_, INPUT_PULLUP);
	Interrupt::registr(pinNr_, this, RISING);
}

RotaryEncoderInterruptB::RotaryEncoderInterruptB(uint8_t const pinNr_, RotaryEncoder* owner_)
{
	owner = owner_;
	pinMode(pinNr_, INPUT_PULLUP);
	Interrupt::registr(pinNr_, this, RISING);
}

RotaryEncoderInterruptP::RotaryEncoderInterruptP(uint8_t const pinNr_, RotaryEncoder* owner_)
{
	owner = owner_;
	pinMode(pinNr_, INPUT_PULLUP);
	Interrupt::registr(pinNr_, this, CHANGE);
}

void
RotaryEncoderInterruptA::isr(void)
{
	noInterrupts();
	{
		bool aInDetent = digitalRead(owner->pinA);
		bool bInDetent = digitalRead(owner->pinB);
		if (aInDetent && bInDetent && owner->expectRisingEdgeOnPinA) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
			owner->encoderPos--;
			owner->expectRisingEdgeOnPinB = false;
			owner->expectRisingEdgeOnPinA = false;
		}
		else if (aInDetent) {
			owner->expectRisingEdgeOnPinB = true;  // we're expecting pinB to signal the transition to detent from free rotation
		}
	}
	interrupts();
}

void
RotaryEncoderInterruptB::isr(void)
{
	noInterrupts();
	{
		bool aInDetent = digitalRead(owner->pinA);
		bool bInDetent = digitalRead(owner->pinB);
		if (aInDetent && bInDetent && owner->expectRisingEdgeOnPinB) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
			owner->encoderPos++;
			owner->expectRisingEdgeOnPinB = false;
			owner->expectRisingEdgeOnPinA = false;
		}
		else if (bInDetent) {
			owner->expectRisingEdgeOnPinA = true;
		}
	}
	interrupts();
}

void
RotaryEncoderInterruptP::isr()
{
	noInterrupts();
	{
		owner->pushed = !digitalRead(owner->pinP);
	}
	interrupts();
}

RotaryEncoder::RotaryEncoder(uint8_t const pinP_,
							 uint8_t const pinA_,
							 uint8_t const pinB_)
	: pinP(pinP_), pinA(pinA_), pinB(pinB_)
{
	expectRisingEdgeOnPinA = true;
	expectRisingEdgeOnPinB = true;
	InterruptPtrA = new RotaryEncoderInterruptA(pinA_, this);
	InterruptPtrB = new RotaryEncoderInterruptB(pinB_, this);
	InterruptPtrP = new RotaryEncoderInterruptP(pinP_, this);
}

void
RotaryEncoder::begin(void)
{
}

encoderPos_t
RotaryEncoder::getPosition(void)
{
	return encoderPos;
}

bool
RotaryEncoder::isPushed(void)
{
	return pushed;
}
