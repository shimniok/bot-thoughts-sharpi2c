/*
 * addr.c
 *
 * Determine address based on 3 inputs that can be in three states.
 * The three states are float, ground, or vcc.  By using a pull-down resistor
 * that is >> the MCU internal pull-up value (~50k), the MCU can sense when the
 * output pin is shorted to ground or unconnected to ground directly.
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */
#include <avr/io.h>
#include <stdint.h>
#include "addr.h"
#include <util/delay.h>

#define DELAY 1
#define test(pin) ((ADDR_PIN & (1<<pin)) == (1<<pin))
#define cbi(sfr, bit) ((sfr) &= ~(1<<(bit)))
#define sbi(sfr, bit) ((sfr) |= (1<<(bit)))

/**
 * Gets 3-way jumper setting and comes up with an address offset of 0..2 to add to
 * whatever I2C base address is chosen.
 *
 * Assumes there's a big pull-down resistor such that if the jumper is un-connected, it is
 * pulled low unless the internal pull-up resistor pulls it high. If it is connected low,
 * it looks low with internal pull-up and without (+0). If it is connected high it looks
 * high with pull-up and without.
 *
 * That is: float = 0, low = 1, high = 2
 *
 */
uint8_t get_one_address(int pin) {
	uint8_t result = 0;

	_delay_ms(DELAY);
	cbi(ADDR_DDR, pin); 			// set pin to input
	cbi(ADDR_PORT, pin);			// disable internal pull-up
	_delay_ms(DELAY);
	if (test(pin)) {
		result = 2;					// pin set high
	} else {						// is pin set low or floating?
		sbi(ADDR_PORT, pin);		// enable internal pull-up
		_delay_ms(DELAY);			// see Tiny44A datasheet, 10.1.4
		if (test(pin)) {			// ... is pin floating?
			result = 0;				// pin floating
		} else {
			result = 1;				// pin forced low
		}
	}
	cbi(ADDR_DDR, pin); 			// set pin to input
	cbi(ADDR_PORT, pin);			// disable internal pull-up
	_delay_ms(DELAY);

	return result;
}

/**
 * Formulates an address based on three input pins with 3-way jumpers.
 *
 * Each input pin can have 3 states (see get_one_address()). This routine
 * converts the "trinary" value of A0:2 to a decimal value. That is,
 * address = A0*3^0 + A1*3^1 + A1*3^2
 */
uint8_t get_address() {
	_delay_ms(DELAY);				// Trying this to see if voltages stabilize after delay
	return get_one_address(ADDR2)*9 + get_one_address(ADDR1)*3 + get_one_address(ADDR0);
}
