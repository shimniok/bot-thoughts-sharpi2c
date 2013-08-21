/*
 * filter.h
 *
 *  Created on: Jun 18, 2013
 *      Author: mes
 */

#ifndef FILTER_H_
#define FILTER_H_

#include <stdint.h>

/**
 * Initialize filter
 */
void filter_init();

/**
 * Leaky integrator filter
 *
 * @param value is the new measurement to filter
 * @return filtered result
 */
uint16_t filter(uint16_t value);

/**
 * Leaky integrator filter
 *
 * @return shift value which determines amount of filtering
 */

//uint16_t filter_get_shift();

/**
 * Leaky integrator filter
 *
 * @param new_shift is the new shift value which determines amount of filtering
 */
//void filter_set_shift(uint16_t new_shift);


#endif /* FILTER_H_ */
