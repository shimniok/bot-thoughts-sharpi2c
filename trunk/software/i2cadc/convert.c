/*
 * convert.c
 *
 * Handles conversion of ADC results
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */
#include <stdint.h>
#include "usiTwiSlave.h"
#include "registers.h"

enum {SHARP_GP2D12=0, SHARP_GP2D120=1, SHARP_GP2Y0A02=2, SHARP_GP2Y0A710=3};

enum {RAW=0, CM=1, IN=2 };

uint8_t convert(uint8_t raw) {
	uint8_t result = 0;
	uint8_t device = usiTwiGetRegister(DEVICE);
	uint8_t units = usiTwiGetRegister(UNITS);

	switch (units) {
	case RAW:
		result = raw;
		break;
	case CM:
		break;
	case IN:
		break;
	default:
		break;
	}

	return result;
}
