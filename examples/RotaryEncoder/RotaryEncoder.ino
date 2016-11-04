// RotaryEncoder101 library example
// IDE: Arduino 1.6.11
// Board support pkg: Intel Curie Boards 1.0.7 (could work on other boards with sufficient IRQs)
// Wiring: rotary encoder 1: common to ground, rotary A on D5, rotary B on D6, push to D7, 
//         rotary encoder 2: common to ground, rotary A on D2, rotary B on D3, push to D4
// (c) 2016 by Coert Vonk (http://www.coertvonk.com)
// see enclosed LICENSE.txt

#include <RotaryEncoder101.h>

uint8_t const nrEncoders = 2;

// push must be on a pin that supports CHANGE interrupts
RotaryEncoder rotEncoder[nrEncoders] = {
	RotaryEncoder(5, 6, 7, false),
	RotaryEncoder(4, 3, 2, false)
};

void setup()
{
	Serial.begin(115200);
	while (!Serial) {
		; // wait for serial port to connect
	}
	delay(50);
	Serial.println("RotaryEncoder");

	for (uint8_t ii = 0; ii < nrEncoders; ii++) {
		rotEncoder[ii].begin();
	}
}

void loop()
{
	typedef struct {
		encoderPos_t pos;
		bool pushed;
	} encoderState_t;

	static encoderState_t lastArr[nrEncoders];

	encoderState_t * last = lastArr;
	for (uint8_t ii = 0; ii < nrEncoders; ii++, last++) {

		encoderState_t const current = {
			.pos = rotEncoder[ii].getPosition(),
			.pushed = rotEncoder[ii].isPushed(),
		};
		if ((last->pos != current.pos)) {
			if (Serial.availableForWrite() > 60) {
				Serial.print(ii); Serial.print(" ");
				Serial.println((uint32_t)current.pos);
			}
			last->pos = current.pos;
		}
		if ((last->pushed != current.pushed)) {
			if (Serial.availableForWrite() > 60) {
				Serial.print(ii); Serial.print(" ");
				Serial.println(current.pushed ? "pushed" : "released");
			}
			last->pushed = current.pushed;
		}
	}
}
