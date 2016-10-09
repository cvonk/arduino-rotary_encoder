// RotaryEncoderArduino101 library example
// IDE: Arduino 1.6.11
// Board support pkg: Intel Curie Boards 1.0.7 (could work on other boards with sufficient IRQs)
// Wiring: rotary encoder 1: common to ground, push to D4, rotary A on D5, rotary B on D6
//         rotary encoder 2: common to ground, push to D7, rotary A on D8, rotary B on D9
// (c) 2016 by Coert Vonk (http://www.coertvonk.com)
// see enclosed LICENSE.txt

#include <RotaryEncoderArduino101.h>

RotaryEncoder * rotEncoder1;
RotaryEncoder * rotEncoder2;

void setup()
{
	Serial.begin(115200);
	while (!Serial) {
		; // wait for serial port to connect
	}
	delay(50);
	Serial.println("RotaryEncoder");

	rotEncoder1 = new RotaryEncoder(4, 5, 6);
	rotEncoder2 = new RotaryEncoder(7, 8, 9);
	rotEncoder1->begin();
	rotEncoder2->begin();
}

void loop()
{
	uint8_t const nrEncoders = 2;

	static encoderPos_t lastEncPos[nrEncoders] = { rotEncoder1->getPosition(), rotEncoder2->getPosition() };
	static bool lastPushed[nrEncoders] = { rotEncoder1->isPushed(), rotEncoder2->isPushed() };

	encoderPos_t const pos[nrEncoders] = { rotEncoder1->getPosition(), rotEncoder2->getPosition() };
	bool const pushed[nrEncoders] = { rotEncoder1->isPushed(), rotEncoder2->isPushed() };

	for (uint8_t ii = 0; ii < nrEncoders; ii++) {

		if ((lastEncPos[ii] != pos[ii])) {
			if (Serial.availableForWrite() > 60) {
				Serial.print(ii); Serial.print(" ");
				Serial.println(pos[ii], DEC);
			}
			lastEncPos[ii] = pos[ii];
		}

		if ((lastPushed[ii] != pushed[ii])) {
			if (Serial.availableForWrite() > 60) {
				Serial.print(ii); Serial.print(" ");
				Serial.println(pushed[ii] ? "pushed" : "released");
			}
			lastPushed[ii] = pushed[ii];
		}
	}
}
