/*
 * filter.c
 *
 * Leaky integrator filter code
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */
#include <stdint.h>
#include "usiTwiSlave.h"
#include "registers.h"

static uint32_t filtsum = 0;	// running sum for filter
static int shift = 3;			// leaky integrator filter shift

/** initialize, attach pointer into I2C registers */
void filter_init() {
	filtsum = 0;
	usiTwiSetRegister(FILTER, shift);
}

/** return the shift value */
/*
 uint16_t filter_get_shift() {
	return shift;
}
*/

/** set a new shift value */
/*
void filter_set_shift(uint16_t new_shift) {
	shift = new_shift;
	filtsum = 0; // need to reset this otherwise we jack up the results
}
*/

/** Leaky integrator filter */
uint16_t filter(uint16_t value) {
	filtsum += (value - (filtsum >> shift));

	// did we get a new value for shift?
	int new_shift = usiTwiGetRegister(FILTER);
	if (new_shift != shift) {
		filtsum = 0;
		shift = new_shift;
		// TODO Store shift in eeprom
	}

	return (filtsum >> shift);
}
