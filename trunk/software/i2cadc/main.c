/* Read ADC values with oversampling and print out result via serial
 *
 * main.c
 *
 *  Created on: Jun 12, 2013
 *      Author: mes
 */
#include <util/delay.h>
#include <string.h>
#include "usiTwiSlave.h" // Tiny85: SDA=PB0, SCL=PB2; Tiny84A: SDA=PA6, SCL=PA4
#include "adc.h"
#include "filter.h"
#include "registers.h"
#include "addr.h"

/** Globals */

uint8_t addr = 0x26;	// 4d read, 4c write

/** Code */

int main(void) {

	// TODO: read configuration from eeprom

	addr += get_address();

	// Initialize TWI
	usiTwiSlaveInit(addr, 5, (1<<RESOLUTION)|(1<<FILTER));

	// TODO: how frequently is the ADC sampling?

	// TODO: Distance conversion - how?

	// Initialize filter
	filter_init();

	// Initialize ADC
	adc_init();

	while (1) {
		// TODO try running ADC stuff here as USI interrupts are lowest priority.
		// Do something else while waiting for the TWI transceiver to complete.
		// Should we go into ADC sleep mode?
		asm volatile ("NOP" ::);
	}
}


