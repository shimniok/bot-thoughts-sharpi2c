/*
 * filter.c
 *
 * Leaky integrator filter code
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */
#include <stdint.h>
#include "registers.h"

static uint32_t filtsum = 0;	// running sum for filter
static int shift = 3;			// leaky integrator filter shift


/** initialize, attach pointer into I2C registers */
void filter_init() {
	filtsum = 0;
	setRegister(FILTER, shift);
}


/** Leaky integrator filter */
uint16_t filter(uint16_t value) {
	filtsum += (value - (filtsum >> shift));

	// did we get a new value for shift?
	int new_shift = getRegister(FILTER);
	if (new_shift != shift) {
		filtsum = 0;
		shift = new_shift;
	}

	return (filtsum >> shift);
}
