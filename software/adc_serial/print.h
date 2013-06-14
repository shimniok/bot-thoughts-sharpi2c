/*
 * print.h
 *
 *  Created on: Jun 12, 2013
 *      Author: mes
 */

#ifndef PRINT_H_
#define PRINT_H_

/** initialize print routines */
void print_init();

/** print a string via serial */
void print(const char *s);

/** print an unsigned long via serial (from Arduino source) */
void printUL(unsigned long n);

/** print a signed long via serial (from Arduino source) */
void printL(long n);

#endif /* PRINT_H_ */
