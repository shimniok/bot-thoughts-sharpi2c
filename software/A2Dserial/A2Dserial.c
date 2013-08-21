/* Read ADC values with oversampling, filtering, serial interface
 *
 * main.c
 *
 *  Created on: Aug 1, 2013
 *      Author: mes
 */
#include <stdint.h>
#include "adc.h"
#include "filter.h"
#include "print.h"

/** Globals */

uint8_t addr = 0x26;	// 4d read, 4c write

/** Code */

int main(void) {

	// TODO: serial protocol
	// TODO: binary vs. mode
	// TODO: continuous vs. on demand mode

	// Initialize Serial
	print_init();

	// Initialize filter
	filter_init();

	// Initialize ADC
	adc_init();

	while (1) {
		// Should we go into ADC sleep mode?
		asm volatile ("NOP" ::);
	}
}
