// RotaryEncoderArduino101 library example
// IDE: Arduino 1.6.11
// Board support pkg: Intel Curie Boards 1.0.7 (could work on other boards with sufficient IRQs)
// Wiring: rotary encoder 1: common to ground, push to D4, rotary A on D5, rotary B on D6
//         rotary encoder 2: common to ground, push to D7, rotary A on D8, rotary B on D9
// (c) 2016 by Coert Vonk (http://www.coertvonk.com)
// see enclosed LICENSE.txt

#include <RotaryEncoderArduino101.h>

uint8_t const nrEncoders = 2;

RotaryEncoder rotEncoder[nrEncoders] = {
	RotaryEncoder(4, 5, 6, false),
	RotaryEncoder(7, 8, 9)
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
				Serial.println(current.pos, DEC);
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
