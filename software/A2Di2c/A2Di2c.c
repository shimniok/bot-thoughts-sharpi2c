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

/** Code */

int main(void) {
	static uint8_t addr = 0x26;	// I2C base address (7-bit)

	registers_init();

	// TODO: read base addr configuration from eeprom
	addr += get_address();

	// Initialize TWI
	usiTwiSlaveInit(addr, MAXREG);

	// Initialize filter
	filter_init();

	// Initialize ADC
	adc_init();

	while(1) {
		// Should we go into ADC sleep mode?
		asm volatile ("NOP" ::);
	}
}


