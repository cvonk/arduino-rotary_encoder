#pragma once
#include <Arduino.h>

#define MAX_INTERRUPTS (6)

typedef uint32_t encoderPos_t;

class Interrupt {
public:
	static void registr(uint8_t const pinNr, Interrupt* intHandlerThis, uint32_t const mode);
	//static void deregistr(uint8_t const pinNr, Interrupt * intHandlerThis);
	static void isr0(void);  // wrapper functions to isr()
	static void isr1(void);
	static void isr2(void);
	static void isr3(void);
	static void isr4(void);
	static void isr5(void);  // number of isrX() should match MAX_INTERRUPTS
	virtual void isr(void) = 0;
private:
	static Interrupt * isrVectorTable[MAX_INTERRUPTS];  // needs to be defined in .cpp file!!
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

class RotaryEncoder
{
	friend class RotaryEncoderInterruptA;
	friend class RotaryEncoderInterruptB;
	friend class RotaryEncoderInterruptP;
  public:
	RotaryEncoder(uint8_t const pinA_, uint8_t const pinB_, uint8_t const pinP_);
	void begin(void);
	encoderPos_t getPosition(void);
	bool isPushed(void);
  private:
	uint8_t const pinP;
	uint8_t const pinA;
	uint8_t const pinB;
	RotaryEncoderInterruptA* InterruptPtrA;
	RotaryEncoderInterruptB* InterruptPtrB;
	RotaryEncoderInterruptP* InterruptPtrP;
	bool expectRisingEdgeOnPinA;
	bool expectRisingEdgeOnPinB;
	volatile bool pushed;
	volatile encoderPos_t encoderPos = 0;
};
