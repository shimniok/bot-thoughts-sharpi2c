/*
 * addr.c
 *
 * Determine address based on 3 inputs that can be in three states.
 * The three states are float, ground, or vcc.  By using a pulldown resistor
 * that is >> the pullup value in the MCU, the MCU can sense when the output
 * pin is pulled down directly to ground or through the pullup resistor by
 * using its own internal pullup
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */
#include <avr/io.h>
#include <stdint.h>
#include "addr.h"

uint8_t get_one_address(int pin) {
	uint8_t result = 0;

	ADDR_DDR &= ~(1 << pin); 			// set pin to input
	ADDR_PORT &= ~(1 << pin);			// disable pullup
	if (ADDR_PIN & (1 << pin)) {
		result++;
	}

	ADDR_PORT |= (1 << pin); 			// enable pullup
	if (ADDR_PIN & (1 << pin)) {
		result++;
	}

	return result;
}

uint8_t get_address() {
	return get_one_address(ADDR2)*9 + get_one_address(ADDR1)*3 + get_one_address(ADDR0);
}
