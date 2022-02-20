#pragma once
#include <Arduino.h>

typedef uint32_t encoderPos_t;

class RotaryEncoderInterruptA;
class RotaryEncoderInterruptB;
class RotaryEncoderInterruptP;

class RotaryEncoder {
    friend class RotaryEncoderInterruptA;
    friend class RotaryEncoderInterruptB;
    friend class RotaryEncoderInterruptP;

  public:
    RotaryEncoder(uint8_t const pinA_,
	              uint8_t const pinB_,
				  uint8_t const pinP_,
	              bool const linear_ = true);
    void begin(void);
    encoderPos_t getPosition(void);
    void setPosition(encoderPos_t const pos);
    bool isPushed(void);
    void reset(void);

  private:
    // parameters
    uint8_t const pinP;
    uint8_t const pinA;
    uint8_t const pinB;
    bool const linear;

    // instantiated child classes
    RotaryEncoderInterruptA * InterruptPtrA;
    RotaryEncoderInterruptB * InterruptPtrB;
    RotaryEncoderInterruptP * InterruptPtrP;

    // state
    bool expectRisingEdgeOnPinA;
    bool expectRisingEdgeOnPinB;
    volatile bool pushed;
    volatile encoderPos_t encoderPos = 0;
    typedef enum { increment, decrement } encoderPosDir_t;

    // calculates the number of positions to change
    encoderPos_t delta(encoderPosDir_t const requestedDirection);
    struct {
        uint64_t lastTime;
        encoderPosDir_t direction;
    } nonlinear;
};
