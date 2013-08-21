/* Read ADC values with oversampling, filtering, act as SPI slave.
 * A2Dspi.c
 *
 *  Created on: Aug 1, 2013
 *      Author: mes
 */
#include <util/delay.h>
#include <string.h>
#include "adc.h"
#include "filter.h"
#include "registers.h"
#include "addr.h"

/** Code */

int main(void) {

	// Initialize SPI

	// Initialize filter
	filter_init();

	// Initialize ADC
	adc_init();

	while (1) {
		// Should we go into ADC sleep mode?
		asm volatile ("NOP" ::);
	}
}


