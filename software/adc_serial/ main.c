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
#include "print.h"

/** Globals */

int m = 64; 		// oversamples
int n = 3;  		// scale factor shift
int shift = 3;		// leaky integrator filter shift
int samples = 0;	// number of samples taken so far
uint16_t result;

/** Functions */

void adc_init();
uint16_t filter(uint16_t value);

/** Code */

int main(void)
{
	static unsigned int x = 0;
	print_init();
	adc_init();
	while (1) {
		printUL(x++);
		print(" ");
		printUL(result);
		print("\n");
		_delay_ms(1);
	}
}

/** ADC, oversampling, decimation and filtering code */

/** initialize ADC and begin first conversion */
void adc_init()
{
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);	// Set ADC prescaler given clock of 8MHz, need 50-200kHz, /64 = 125kHz
	//ADMUX |= (1<<REFS0); 					// External voltage reference at PA0 (AREF) pin, internal reference turned off
	ADCSRA |= (0<<ADATE);	 				// Put ADC in non-free-running mode (aka auto trigger disabled)
	//ADMUX |= (1 << ADLAR); 				// Left align ADC result
	ADMUX |= (0<<MUX3)|(0<<MUX2)|(1<<MUX1)|(0<<MUX0);	// Input: ADC2 (PB4)
	ADCSRA |= (1<<ADEN); 					// Enable ADC
	ADCSRA |= (1<<ADIE);					// Enable ADC conversion complete interrupt
	sei();
	samples = 0;
	ADCSRA |= (1<<ADSC); 			// start conversion
}

/**
 *  ADC Conversion Complete Interrupt Service Routine (ISR)
 *  Handles oversampling m times with right shift n
 */
ISR(ADC_vect)
{
	static uint32_t res;
	uint16_t val;

	if (samples++ < m) {				// add value to result m times, then right shift n times
		val = ADCL;						// must get ADC low byte first to avoid truncation, see datasheet
		val |= (ADCH << 8);				// high byte
		res += val;						// oversampling, add up values
	} else {
		//result[in++] = (res >> n);	// decimation, reduce to 10+n bit resolution
		//in &= 0x0f;					// wrap
		result = filter(res>>n);		// decimation, reduce to 10+n bit, then filter
		res = 0;
		samples = 0;
	}
	ADCSRA |= (1<<ADSC); 				// start conversion
}


/**
 * Leaky integrator filter
 *
 * @param value is the new measurement to filter
 * @return filtered result
 */
uint16_t filter(uint16_t value)
{
	static volatile uint32_t filter_value = 0;

    filter_value += (value - (filter_value >> shift));

    return (filter_value >> shift);
}
