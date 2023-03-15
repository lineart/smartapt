# smartapt
Light Controls for Smart Apartment using Arduino

## Prerequisits
* 1x Arduino (Nano) board
* 6x PCF8574 IO extenders + library for Arduino
* 16x Opto-isolators for input switches
* 2x16 Relay Board
* Zero-cross detection circuit

## Setup
This project was made for Smart Apartment lights control

It uses Arduino (Nano) as a core relay controller with a bunch of PCF8574-based IO extenders.

The specific setup uses 16 wall switches as an input and 32 relays as an ouput

Zero-cross detection circuit is added as an IRQ for better relay toggle timing
