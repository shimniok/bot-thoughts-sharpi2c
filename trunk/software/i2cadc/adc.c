/*
 * adc.c
 *
 * ADC, oversampling, decimation
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usiTwiSlave.h"
#include "registers.h"
#include "filter.h"

/** static variables */

static int n = 3;  			// scale factor shift
static int m = 64; 			// oversamples
static int samples = 0;		// number of samples taken so far
uint16_t result = 0;	// filtered, oversampled, decimated result

/** initialize ADC and begin first conversion */
void adc_init() {
	// Set ADC prescaler given clock of 8MHz, need 50-200kHz, /64 = 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
	// Put ADC in non-free-running mode (aka auto trigger disabled)
	ADCSRA |= (0 << ADATE);
	// Input: ADC2 (Tiny85: PB4, Tiny84A: PA2)
	ADMUX |= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0);
	// Enable ADC
	ADCSRA |= (1 << ADEN);
	// Enable ADC conversion complete interrupt
	ADCSRA |= (1 << ADIE);
	sei();
	samples = 0;
	// start conversion
	ADCSRA |= (1 << ADSC);
	// Initialize TWI registers
	usiTwiSetRegister(OVERSAMPLES, m);
	usiTwiSetRegister(RESOLUTION, n);
}

/*
uint16_t adc_result() {
	return result;
}

uint16_t adc_get_m() {
	return m;
}

uint16_t adc_get_n() {
	return n;
}

void adc_set_n(uint16_t new_n) {
	n = new_n;
}
*/

/**
 *  ADC Conversion Complete Interrupt Service Routine (ISR)
 *  Handles oversampling m times with right shift n
 */
ISR(ADC_vect) {
	static uint32_t res;
	uint16_t val;

	if (samples++ < m) { 							// add value to result m times, then right shift n times
		val = ADCL;									// must get ADC low byte first to avoid truncation, see datasheet
		val |= (ADCH << 8);							// high byte
		res += val;									// oversampling, add up values
	} else {
		result = filter(res >> n);					// decimation, reduce to 10+n bit, then filter
		usiTwiSetRegister(RESHI, result >> 8); 		// reg 0 is the hi byte
		usiTwiSetRegister(RESLO, result & 0x0f);	// reg 1 is the lo byte
		res = 0;
		samples = 0;
		// did we get a new new n?
		int new_n = usiTwiGetRegister(RESOLUTION);
		if (new_n != n) {
			n = new_n;
			// TODO store n in eeprom
		}
	}
	ADCSRA |= (1 << ADSC); 								// start conversion
}


