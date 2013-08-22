/*
 * registers.c
 *
 *  Created on: Aug 1, 2013
 *      Author: mes
 */

#include "registers.h"

uint8_t reg[MAXREG];
uint8_t current=0;
uint8_t wmask;

/**
 * initialize registers
 *
 * @param max is the number of registers
 */
void registers_init() {
	uint8_t i;
	wmask = (1<<RESOLUTION)|(1<<FILTER);	// initialize write mask
	current = 0;							// current register starts at 0
	for (i = 0; i < MAXREG; i++) {
		reg[i] = 0xad; 						// fill registers with dummy data
	}
}

/**
 * determine if current register is writeable
 *
 * @return true if current register is writeable, false otherwise
 */
bool isWriteableCurrent() {
	return ((wmask & (1<<current)) != 0);
}

/**
 * set current register
 *
 * @param c register number to be the current register
 */
bool setCurrent(uint8_t c) {
	bool result = false;

	if (current >= 0 && current < MAXREG ) {
		current = c;
		result = true;
	}

	return result;
}

/**
 * return value in the current register
 *
 * @param r is the register to return
 * @return copy of the register value
 */
uint8_t getCurrentRegister() {
	return reg[current];
}

/**
 * return value in the next register; current+1,
 * or 0 if (current+1 == maxreg)
 *
 * @param r is the register to return
 * @return copy of the register value
 */
uint8_t getNextRegister() {
	uint8_t result = reg[current];

	if (++current >= MAXREG) current = 0;

	return result;
}

/**
 * return value in specified register
 *
 * @param r is the register to return
 * @return copy of the register value if reg is in range, 0 if reg is out of range
 */
uint8_t getRegister(uint8_t r) {
	uint8_t result = 0;
	if (r >= 0 && r < MAXREG)
		result = reg[r];
	return result;
}

/**
 * sets the current register to the specified value
 *
 * @param reg is the register to set
 * @param value is the value to be copied into the register
 * @return false if reg out of range, true otherwise
 */
void setCurrentRegister(uint8_t value) {
	reg[current] = value;
}

/**
 * sets the specified register to the specified value
 *
 * @param reg is the register to set
 * @param value is the value to be copied into the register
 * @return false if reg out of range, true otherwise
 */
bool setRegister(uint8_t r, uint8_t value) {
	bool result = false;
	if (r >= 0 && r < MAXREG) {
		reg[r] = value;
		result = true;
	}
	return result;
}
