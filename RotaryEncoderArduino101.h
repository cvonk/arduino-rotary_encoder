#pragma once
#include <Arduino.h>

#define MAX_INTERRUPTS (6)

typedef uint64_t encoderPos_t;

class RotaryEncoderInterruptA;
class RotaryEncoderInterruptB;
class RotaryEncoderInterruptP;

class RotaryEncoder
{
	friend class RotaryEncoderInterruptA;
	friend class RotaryEncoderInterruptB;
	friend class RotaryEncoderInterruptP;

  public:
	RotaryEncoder(uint8_t const pinA_, uint8_t const pinB_, uint8_t const pinP_, bool const lineair_ = true);
	void begin(void);
	encoderPos_t getPosition(void);
	bool isPushed(void);

  private:
	  // parameters
	uint8_t const pinP;
	uint8_t const pinA;
	uint8_t const pinB;
	bool const lineair;
    
	  // instantiated child classes
	RotaryEncoderInterruptA * InterruptPtrA;
	RotaryEncoderInterruptB * InterruptPtrB;
	RotaryEncoderInterruptP * InterruptPtrP;

	  // state about what we're expecting
	bool expectRisingEdgeOnPinA;
	bool expectRisingEdgeOnPinB;

	  // current state
	volatile bool pushed;
	volatile encoderPos_t encoderPos = 0;
	typedef enum { increment, decrement } encoderPosDir_t;

	  // calculates the number of positions to change
	encoderPos_t delta(encoderPosDir_t const requestedDirection);
	struct {
		uint64_t lastTime;
		encoderPosDir_t direction;
	} nonLineair;
};
