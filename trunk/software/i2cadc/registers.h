/*
 * registers.h
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */

#ifndef __REGISTERS_H_
#define __REGISTERS_H_

#define ADCRES 10 // our adc resolution is 10 bit

// Mapping of registers for I2C
enum {
	RESHI = 0, RESLO = 1, OVERSAMPLES = 2, RESOLUTION = 3, FILTER = 4, DEVICE = 5, UNITS = 6
};


#endif /* REGISTERS_H_ */
