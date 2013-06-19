/*
 * adc.h
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */

#ifndef ADC_H_
#define ADC_H_

void adc_init();
uint16_t adc_result();
uint16_t adc_get_m();
uint16_t adc_get_n();
void adc_set_n(uint16_t new_n);

#endif /* ADC_H_ */
