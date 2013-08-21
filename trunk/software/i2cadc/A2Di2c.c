/* Read ADC values with oversampling, filtering, act as I2C slave.
 *
 * main.c
 *
 *  Created on: Jun 12, 2013
 *      Author: mes
 */
#include <util/delay.h>
#include <string.h>
#include "registers.h"
#include "adc.h"
#include "filter.h"
#include "addr.h"
#include "usiTwiSlave.h"

/** Globals */

uint8_t addr = 0x26;	// I2C base address (7-bit)

// TODO: convert to manual makefiles

/** Code */

int main(void) {

	registers_init();

	// TODO: read base addr configuration from eeprom
	addr += get_address();

	// Initialize TWI
	usiTwiSlaveInit(addr, MAXREG, (1<<RESOLUTION)|(1<<FILTER));

	// Initialize filter
	filter_init();

	// Initialize ADC
	adc_init();

	while (1) {
		// Should we go into ADC sleep mode?
		asm volatile ("NOP" ::);
	}
}


