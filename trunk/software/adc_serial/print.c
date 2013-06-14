/*
 * print.c
 *
 *  Created on: Jun 12, 2013
 *      Author: mes
 */
#include "usi_uart.h"


void print_init()
{
	USI_UART_Initialise_Transmitter();
}


void print(const char *s)
{
	while (s && *s) {
		USI_UART_Transmit_Byte(*s++);
	}
}


// from Arduino source
void printUL(unsigned long n)
{
    char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[sizeof(buf) - 1];

    *str = '\0';

    do {
        unsigned long m = n;
        n /= 10;
        char c = m - 10 * n;
        *--str = c + '0';
    } while(n);

    print(str);
}

// from Arduino source
void printL(long n)
{
    if (n < 0) {
        USI_UART_Transmit_Byte('-');
        n = -n;
    }
    printUL(n);
}

