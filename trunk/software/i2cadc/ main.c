/* Read ADC values with oversampling and print out result via serial
 *
 * main.c
 *
 *  Created on: Jun 12, 2013
 *      Author: mes
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "usiTwiSlave.h" // Tiny85: SDA=PB0, SCL=PB2
/** Defines */

#define ADCRES 10 // our adc resolution is 10 bit
enum {
	RESHI = 0, RESLO = 1, OVERSAMPLES = 2, RESOLUTION = 3, FILTER = 4
};

/** Globals */

uint8_t addr = 0x26;	// 4d read, 4c write
uint16_t result = 0;	// filtered, oversampled, decimated result
int n = 3;  			// scale factor shift
int m = 64; 			// oversamples
int samples = 0;		// number of samples taken so far
uint32_t filtsum = 0;	// running sum for filter
int shift = 3;			// leaky integrator filter shift

/** Functions */

void adc_init();
uint16_t filter(uint16_t value);

/** Code */

int main(void) {
	// TODO: determine hardware configured address

	// Initialize TWI
	usiTwiSlaveInit(addr, 5, (1<<RESOLUTION)|(1<<FILTER));

	// Initialize registers
	usiTwiSetRegister(OVERSAMPLES, m);
	usiTwiSetRegister(RESOLUTION, n);
	usiTwiSetRegister(FILTER, shift);

	// Initialize ADC
	adc_init();

	while (1) {
		// TODO try running ADC stuff here as USI interrupts are lowest priority.

		// Do something else while waiting for the TWI transceiver to complete.
		asm volatile ("NOP" ::);
	}
}

/** ADC, oversampling, decimation and filtering code */

/** initialize ADC and begin first conversion */
void adc_init() {
	// Set ADC prescaler given clock of 8MHz, need 50-200kHz, /64 = 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
	// Put ADC in non-free-running mode (aka auto trigger disabled)
	ADCSRA |= (0 << ADATE);
	// Input: ADC2 (PB4)
	ADMUX |= (0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0);
	// Enable ADC
	ADCSRA |= (1 << ADEN);
	// Enable ADC conversion complete interrupt
	ADCSRA |= (1 << ADIE);
	sei();
	samples = 0;
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
		result = filter(res >> n);					// decimation, reduce to 10+n bit, then filter
		usiTwiSetRegister(RESHI, result >> 8); 		// reg 0 is the hi byte
		usiTwiSetRegister(RESLO, result & 0x0f);	// reg 1 is the lo byte
		res = 0;
		samples = 0;
		// did we get a new shift?
		// did we get a new new n?
	}
	ADCSRA |= (1 << ADSC); 								// start conversion
}

/**
 * Leaky integrator filter
 *
 * @param value is the new measurement to filter
 * @return filtered result
 */
uint16_t filter(uint16_t value) {
	filtsum += (value - (filtsum >> shift));
	return (filtsum >> shift);
}
