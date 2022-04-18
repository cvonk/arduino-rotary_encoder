# Rotary Encoder for Arduino-101  

![Build Status](https://travis-ci.org/cvonk/arduino-rotary_encoder.svg?branch=master)
[![GitHub Discussions](https://img.shields.io/github/discussions/cvonk/arduino-rotary_encoder)](https://github.com/cvonk/arduino-rotary_encoder/discussions)
![GitHub tag (latest by date)](https://img.shields.io/github/v/tag/cvonk/arduino-rotary_encoder)
![GitHub](https://img.shields.io/github/license/cvonk/ESP32_factory-ble-prov)

##Using interrupts and allowing multiple instances

This library supports rotary encoders connected to the Arduino/Genuino 101.  It should work for other microcontrollers for as long as they can generate the necessary interrupts.

Features:

* interrupt driven, no polling needed
* allows for more than one instance
* ignores spurious signals from encoder
* lineair or non-lineair mode

Assumptions:

* The common pins of the rotary encoder are connected to ground, and the remaining pins to GPIO ports

![Rotary encoder](media/rotrencoder.png)

Many sketches exist for the Arduino UNO, most of them only supporting a single rotary encoder.   I borrowed from [one of them](http://www.instructables.com/id/Improved-Arduino-Rotary-Encoder-Reading/), and adapted it for Arduino/Genuino 101.

The challenge was for the interrupt service routines to invoke a C++ Class Member Function.  The solution chosen relies on virtual functions and friend classes used as described in the article [Interrupts in C++](http://www.embedded.com/design/prototyping-and-development/4023817/Interrupts-in-C-) by Alan Dorfmeyer and Pat Baird.

![Rotary encoder](schematic/schematic.png)

Other Embedded C projects can be found at [coertvonk.com](http://coertvonk.com/category/sw/embedded).
