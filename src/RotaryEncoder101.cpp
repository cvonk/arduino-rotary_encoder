// RotaryEncoderArduino101 library example
// IDE: Arduino 1.8.5, 1.6.11
// Board support pkg: Intel Curie Boards 1.0.7 (could work on other boards with sufficient IRQs)
// Wiring: rotary encoder 1: common to ground, push to D4, rotary A on D5, rotary B on D6
//         rotary encoder 2: common to ground, push to D7, rotary A on D8, rotary B on D9
// (c) 2016 by Coert Vonk (http://www.coertvonk.com)
// see enclosed LICENSE.txt
// inspired by http://www.embedded.com/design/prototyping-and-development/4023817/Interrupts-in-C-
//         and http://www.instructables.com/id/Improved-Arduino-Rotary-Encoder-Reading/

#include "RotaryEncoder101.h"

uint8_t const maxInterrupts = 6;
encoderPos_t const encoderPos_min = 0L;
encoderPos_t const encoderPos_max = 99999999L;

class Interrupt {
public:
	static void registr(uint8_t const pinNr, Interrupt* intHandlerThis, uint32_t const mode);
	//static void deregistr(uint8_t const pinNr, Interrupt * intHandlerThis);
	static void isr0(void);  // wrapper functions to isr()
	static void isr1(void);
	static void isr2(void);
	static void isr3(void);
	static void isr4(void);
	static void isr5(void);  // number of isrX() should match maxInterrupts
	virtual void isr(void) = 0;
private:
	static Interrupt * isrVectorTable[maxInterrupts];  // needs to be defined in .cpp file!!
};

class RotaryEncoder; // forward declaration

class RotaryEncoderInterruptA : public Interrupt
{
public:
	RotaryEncoderInterruptA(uint8_t const intnr, RotaryEncoder* ownerptr);
	void isr(void);
private:
	RotaryEncoder * owner;
};

class RotaryEncoderInterruptB : public Interrupt
{
public:
	RotaryEncoderInterruptB(uint8_t const intnr, RotaryEncoder* ownerptr);
	void isr(void);
private:
	RotaryEncoder * owner;
};

class RotaryEncoderInterruptP : public Interrupt
{
public:
	RotaryEncoderInterruptP(uint8_t const intnr, RotaryEncoder* ownerptr);
	void isr(void);
private:
	RotaryEncoder * owner;
};

typedef void(*isrFnc_t)(void);

void Interrupt::registr(uint8_t const pinNr, Interrupt * intHandlerThis, uint32_t const mode_)
{
	for (uint8_t ii = 0; ii < maxInterrupts; ii++) {
		if (isrVectorTable[ii] == NULL) {
			isrVectorTable[ii] = intHandlerThis;
			isrFnc_t fnc;
			switch (ii) {
				case 0: fnc = isr0; break;
				case 1: fnc = isr1; break;
				case 2: fnc = isr2; break;
				case 3: fnc = isr3; break;
				case 4: fnc = isr4; break;
				case 5: fnc = isr5; break; // should match maxInterrupts
				default: fnc = NULL;
			}
			if (fnc) {
				PinDescription *p = &g_APinDescription[pinNr];
				Serial.print("<");
				Serial.print(p->ulGPIOType == SS_GPIO ? "GPIO" : "SS");
				Serial.print(pinNr);  Serial.print(">");
				attachInterrupt(pinNr, fnc, mode_);
			}
			break;
		}
	}
}

#if 0
void Interrupt::deregistr(uint8_t const pinNr, Interrupt * intHandlerThis)
{
	for (uint8_t ii = 0; ii < maxInterrupts; ii++) {
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
Interrupt * Interrupt::isrVectorTable[maxInterrupts];

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

encoderPos_t
RotaryEncoder::delta(encoderPosDir_t const requestedDirection)
{
	if (lineair) {
		return 1;
	}

	uint64_t const now = millis();
	uint64_t diff = now - nonLineair.lastTime;

    // ignore spurious direction changes when turning very quickly
	// (probably caused by missed interrupts)
	if (requestedDirection != nonLineair.direction && diff < 100) {
		return 0;
	}
	nonLineair.direction = requestedDirection;

	encoderPos_t scale = 1;
	while (diff < 200 && scale < encoderPos_max / 10) {
		scale *= 7;
		diff <<= 1;
	}
	nonLineair.lastTime = now;
	return scale;
}

void
RotaryEncoderInterruptA::isr(void)
{
	//noInterrupts();  not needed, 'cause caller already masked IRQ
	{
		bool aInDetent = digitalRead(owner->pinA);
		bool bInDetent = digitalRead(owner->pinB);
		if (aInDetent && bInDetent && owner->expectRisingEdgeOnPinA) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
			encoderPos_t const delta = owner->delta(owner->increment);
			encoderPos_t const pos = owner->encoderPos;
			if (pos < encoderPos_min + delta) {
				owner->encoderPos = encoderPos_min;
			} else {
				owner->encoderPos = pos - delta;
			}
			owner->expectRisingEdgeOnPinB = false;
			owner->expectRisingEdgeOnPinA = false;
		}
		else if (aInDetent) {
			owner->expectRisingEdgeOnPinB = true;  // we're expecting pinB to signal the transition to detent from free rotation
		}
	}
	//interrupts();  not needed, 'cause caller already masked IRQ
}

void
RotaryEncoderInterruptB::isr(void)
{
	//noInterrupts();  not needed, 'cause caller already masked IRQ
	{
		bool aInDetent = digitalRead(owner->pinA);
		bool bInDetent = digitalRead(owner->pinB);
		if (aInDetent && bInDetent && owner->expectRisingEdgeOnPinB) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
			encoderPos_t const delta = owner->delta(owner->decrement);
			encoderPos_t const pos = owner->encoderPos;
			if (pos + delta > encoderPos_max) {
				owner->encoderPos = encoderPos_max;
			} else {
				owner->encoderPos = pos + delta;
			}
			owner->expectRisingEdgeOnPinB = false;
			owner->expectRisingEdgeOnPinA = false;
		}
		else if (bInDetent) {
			owner->expectRisingEdgeOnPinA = true;
		}
	}
	//interrupts();  not needed, 'cause caller already masked IRQ
}

void
RotaryEncoderInterruptP::isr()
{
	//noInterrupts();  not needed, 'cause caller already masked IRQ
	{
		owner->pushed = !digitalRead(owner->pinP);
	}
	//interrupts();  not needed, 'cause caller already masked IRQ
}

RotaryEncoder::RotaryEncoder(uint8_t const pinA_,
							 uint8_t const pinB_,
							 uint8_t const pinP_,
							 bool const lineair_)
	: pinP(pinP_), pinA(pinA_), pinB(pinB_), lineair(lineair_)
{
	expectRisingEdgeOnPinA = true;
	expectRisingEdgeOnPinB = true;
}

void
RotaryEncoder::begin(void)
{
	InterruptPtrA = new RotaryEncoderInterruptA(pinA, this);
	InterruptPtrB = new RotaryEncoderInterruptB(pinB, this);
	InterruptPtrP = new RotaryEncoderInterruptP(pinP, this);
}

encoderPos_t
RotaryEncoder::getPosition(void)
{
	return encoderPos;
}

void
RotaryEncoder::setPosition(encoderPos_t const pos)
{
	encoderPos = pos;
}

bool
RotaryEncoder::isPushed(void)
{
	return pushed;
}
