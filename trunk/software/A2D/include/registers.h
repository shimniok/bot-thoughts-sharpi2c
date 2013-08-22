/*
 * registers.h
 *
 * Abstracts data into registers which can be used with SPI, Serial, I2C or any interface
 * Each application that uses this library can have its own register mapping, so the
 * communication module doesn't have to know what's in each register, it just dishes out
 * each register as requested by the caller.
 *
 *  Created on: Aug 1, 2013
 *      Author: mes
 */

#ifndef REGISTERS_H_
#define REGISTERS_H_

#include <stdint.h>
#include <stdbool.h>

#define MAXREG 5

/** Register map for ADC functionality */
#define RESHI			0
#define RESLO			1
#define OVERSAMPLES		2
#define RESOLUTION		3
#define FILTER			4

/**
 * initialize register data, set current register to 0, set write mask
 */
void registers_init();

/**
 * determine if current register is writeable
 *
 * @return true if current register is writeable, false otherwise
 */
bool isWriteableCurrent();

/**
 * set current register
 *
 * @param c register number to be the current register
 * @return true if c is in range, 0 if c is out of range
 */
bool setCurrent(uint8_t c);

/**
 * return value in the current register
 *
 * @param r is the register to return
 * @return copy of the register value
 */
uint8_t getCurrentRegister();

/**
 * return value in the current register and increment current to next register
 * (or 0 if current >= maxreg)
 *
 * @param r is the register to return
 * @return copy of the register value
 */
uint8_t getNextRegister();

/**
 * return value in specified register
 *
 * @param reg is the register to return
 * @return copy of the register value if reg is in range, 0 if reg is out of range
 */
uint8_t getRegister(uint8_t reg);

/**
 * sets the current register to the specified value
 *
 * @param reg is the register to set
 * @param value is the value to be copied into the register
 * @return false if reg out of range, true otherwise
 */
void setCurrentRegister(uint8_t value);

/**
 * sets the specified register to the specified value
 *
 * @param reg is the register to set
 * @param value is the value to be copied into the register
 * @return false if reg out of range, true otherwise
 */
bool setRegister(uint8_t reg, uint8_t value);


#endif /* REGISTERS_H_ */
