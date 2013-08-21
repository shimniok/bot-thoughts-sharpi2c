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
#include <math.h>
#include "adc.h"
#include "registers.h"
#include "filter.h"

/** static variables */

static int n = 3;  			// scale factor shift
static int m = 64; 			// oversamples
static int samples = 0;		// number of samples taken so far

/** initialize ADC and begin first conversion */
void adc_init() {
	samples = 0;
	setRegister(OVERSAMPLES, m);
	setRegister(RESOLUTION, n);
	// Set ADC prescaler given clock of 8MHz, need 50-200kHz, /64 = 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
	// Put ADC in non-free-running mode (aka auto trigger disabled)
	ADCSRA |= (0 << ADATE);
	// Input: ADC7 (Tiny44A)
	ADMUX |= (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	// Enable ADC
	ADCSRA |= (1 << ADEN);
	// Enable ADC conversion complete interrupt
	ADCSRA |= (1 << ADIE);
	sei();
	// start conversion
	ADCSRA |= (1 << ADSC);
}


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
		uint32_t result = filter(res >> n);			// decimation, reduce to 10+n bit, then filter
		uint8_t hi = (result>>8);
		uint8_t lo = result & 0xff;
		setRegister(RESHI, hi); 					// hi byte
		setRegister(RESLO, lo);						// lo byte
		res = 0;
		samples = 0;
		// did we get a new new n?
		int new_n = getRegister(RESOLUTION);
		if (new_n != n) {
			n = new_n;
			m = pow(4,n);
		}
	}
	ADCSRA |= (1 << ADSC); 								// start conversion
}


