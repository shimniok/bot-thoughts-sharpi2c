/********************************************************************************

 USI TWI Slave driver.

 Created by Donald R. Blake
 donblake at worldnet.att.net

 ---------------------------------------------------------------------------------

 Created from Atmel source files for Application Note AVR312: Using the USI Module
 as an I2C slave.

 This program is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 PARTICULAR PURPOSE.  See the GNU General Public License for more details.

 ---------------------------------------------------------------------------------

 Change Activity:

 Date       Description
 ------      -------------
 16 Mar 2007  Created.
 27 Mar 2007  Added support for ATtiny261, 461 and 861.
 26 Apr 2007  Fixed ACK of slave address on a read.
 04 Jul 2007  Fixed USISIF in ATtiny45 def

 ********************************************************************************/

/********************************************************************************

 includes

 ********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usiTwiSlave.h"

/********************************************************************************

 device dependent defines

 ********************************************************************************/

#if defined( __AVR_ATtiny2313__ )
#  define DDR_USI             DDRB
#  define PORT_USI            PORTB
#  define PIN_USI             PINB
#  define PORT_USI_SDA        PB5
#  define PORT_USI_SCL        PB7
#  define PIN_USI_SDA         PINB5
#  define PIN_USI_SCL         PINB7
#  define USI_START_COND_INT  USISIF
#  define USI_START_VECTOR    USI_START_vect
#  define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect
#endif

#if defined( __AVR_ATtiny25__ ) | \
		defined( __AVR_ATtiny45__ ) | \
		defined( __AVR_ATtiny85__ )
#  define DDR_USI             DDRB
#  define PORT_USI            PORTB
#  define PIN_USI             PINB
#  define PORT_USI_SDA        PB0
#  define PORT_USI_SCL        PB2
#  define PIN_USI_SDA         PINB0
#  define PIN_USI_SCL         PINB2
#  define USI_START_COND_INT  USISIF
#  define USI_START_VECTOR    USI_START_vect
#  define USI_OVERFLOW_VECTOR USI_OVF_vect
#endif

#if defined( __AVR_ATtiny26__ )
#  define DDR_USI             DDRB
#  define PORT_USI            PORTB
#  define PIN_USI             PINB
#  define PORT_USI_SDA        PB0
#  define PORT_USI_SCL        PB2
#  define PIN_USI_SDA         PINB0
#  define PIN_USI_SCL         PINB2
#  define USI_START_COND_INT  USISIF
#  define USI_START_VECTOR    USI_STRT_vect
#  define USI_OVERFLOW_VECTOR USI_OVF_vect
#endif

#if defined( __AVR_ATtiny261__ ) | \
		defined( __AVR_ATtiny461__ ) | \
		defined( __AVR_ATtiny861__ )
#  define DDR_USI             DDRB
#  define PORT_USI            PORTB
#  define PIN_USI             PINB
#  define PORT_USI_SDA        PB0
#  define PORT_USI_SCL        PB2
#  define PIN_USI_SDA         PINB0
#  define PIN_USI_SCL         PINB2
#  define USI_START_COND_INT  USISIF
#  define USI_START_VECTOR    USI_START_vect
#  define USI_OVERFLOW_VECTOR USI_OVF_vect
#endif

#if defined( __AVR_ATmega165__ ) | \
		defined( __AVR_ATmega325__ ) | \
		defined( __AVR_ATmega3250__ ) | \
		defined( __AVR_ATmega645__ ) | \
		defined( __AVR_ATmega6450__ ) | \
		defined( __AVR_ATmega329__ ) | \
		defined( __AVR_ATmega3290__ )
#  define DDR_USI             DDRE
#  define PORT_USI            PORTE
#  define PIN_USI             PINE
#  define PORT_USI_SDA        PE5
#  define PORT_USI_SCL        PE4
#  define PIN_USI_SDA         PINE5
#  define PIN_USI_SCL         PINE4
#  define USI_START_COND_INT  USISIF
#  define USI_START_VECTOR    USI_START_vect
#  define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect
#endif

#if defined( __AVR_ATmega169__ )
#  define DDR_USI             DDRE
#  define PORT_USI            PORTE
#  define PIN_USI             PINE
#  define PORT_USI_SDA        PE5
#  define PORT_USI_SCL        PE4
#  define PIN_USI_SDA         PINE5
#  define PIN_USI_SCL         PINE4
#  define USI_START_COND_INT  USISIF
#  define USI_START_VECTOR    USI_START_vect
#  define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect
#endif

/********************************************************************************

 functions implemented as macros

 ********************************************************************************/

#define SET_USI_TO_SEND_ACK( ) \
		{ \
	/* prepare ACK */ \
		USIDR = 0; \
		/* set SDA as output */ \
		DDR_USI |= ( 1 << PORT_USI_SDA ); \
		/* clear all interrupt flags, except Start Cond */ \
		USISR = \
		( 0 << USI_START_COND_INT ) | \
		( 1 << USIOIF ) | ( 1 << USIPF ) | \
		( 1 << USIDC )| \
		/* set USI counter to shift 1 bit */ \
		( 0x0E << USICNT0 ); \
		}

#define SET_USI_TO_READ_ACK( ) \
		{ \
	/* set SDA as input */ \
		DDR_USI &= ~( 1 << PORT_USI_SDA ); \
		/* prepare ACK */ \
		USIDR = 0; \
		/* clear all interrupt flags, except Start Cond */ \
		USISR = \
		( 0 << USI_START_COND_INT ) | \
		( 1 << USIOIF ) | \
		( 1 << USIPF ) | \
		( 1 << USIDC ) | \
		/* set USI counter to shift 1 bit */ \
		( 0x0E << USICNT0 ); \
		}

#define SET_USI_TO_TWI_START_CONDITION_MODE( ) \
		{ \
	USICR = \
	/* enable Start Condition Interrupt, disable Overflow Interrupt */ \
		( 1 << USISIE ) | ( 0 << USIOIE ) | \
		/* set USI in Two-wire mode, no USI Counter overflow hold */ \
		( 1 << USIWM1 ) | ( 0 << USIWM0 ) | \
		/* Shift Register Clock Source = External, positive edge */ \
		/* 4-Bit Counter Source = external, both edges */ \
		( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 0 << USICLK ) | \
		/* no toggle clock-port pin */ \
		( 0 << USITC ); \
		USISR = \
		/* clear all interrupt flags, except Start Cond */ \
		( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | \
		( 1 << USIDC ) | ( 0x0 << USICNT0 ); \
		}

#define SET_USI_TO_SEND_DATA( ) \
		{ \
	/* set SDA as output */ \
		DDR_USI |=  ( 1 << PORT_USI_SDA ); \
		/* clear all interrupt flags, except Start Cond */ \
		USISR    =  \
		( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | \
		( 1 << USIDC) | \
		/* set USI to shift out 8 bits */ \
		( 0x0 << USICNT0 ); \
		}

#define SET_USI_TO_READ_DATA( ) \
		{ \
	/* set SDA as input */ \
		DDR_USI &= ~( 1 << PORT_USI_SDA ); \
		/* clear all interrupt flags, except Start Cond */ \
		USISR    = \
		( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | \
		( 1 << USIPF ) | ( 1 << USIDC ) | \
		/* set USI to shift out 8 bits */ \
		( 0x0 << USICNT0 ); \
		}

/********************************************************************************

 typedef's

 ********************************************************************************/

typedef enum {
	USI_SLAVE_CHECK_ADDRESS = 0x00,
	USI_SLAVE_SEND_DATA = 0x01,
	USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA = 0x02,
	USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA = 0x03,
	USI_SLAVE_REQUEST_DATA_REG = 0x04,
	USI_SLAVE_GET_REG_AND_SEND_ACK = 0x05,
	USI_SLAVE_GET_VAL_AND_SEND_ACK = 0x06,
	USI_SLAVE_REQUEST_DATA_VAL = 0x07
} overflowState_t;

/********************************************************************************

 local variables

 ********************************************************************************/

#define MAXREG 32 // number of registers available via I2C
static int current = 0;
static int regmax = MAXREG;
static uint8_t registers[MAXREG];
static uint32_t write_mask = 0;
static uint8_t slaveAddress;
static volatile overflowState_t overflowState;

static uint8_t rxBuf[TWI_RX_BUFFER_SIZE];
static volatile uint8_t rxHead;
static volatile uint8_t rxTail;

static uint8_t txBuf[TWI_TX_BUFFER_SIZE];
static volatile uint8_t txHead;
static volatile uint8_t txTail;

/********************************************************************************

 local functions

 ********************************************************************************/

// flushes the TWI buffers
static
void flushTwiBuffers(void) {
	rxTail = 0;
	rxHead = 0;
	txTail = 0;
	txHead = 0;
} // end flushTwiBuffers

/********************************************************************************

 public functions

 ********************************************************************************/

// initialise USI for TWI slave mode

void usiTwiSlaveInit(uint8_t addr, int top, uint32_t wmask) {

	int i;

	write_mask = wmask;
	regmax = top;
	slaveAddress = addr;

	for (i = 0; i < regmax; i++) {
		registers[i] = 0xde;
	}

	flushTwiBuffers();

	// In Two Wire mode (USIWM1, USIWM0 = 1X), the slave USI will pull SCL
	// low when a start condition is detected or a counter overflow (only
	// for USIWM1, USIWM0 = 11).  This inserts a wait state.  SCL is released
	// by the ISRs (USI_START_vect and USI_OVERFLOW_vect).

	// Set SCL and SDA as output
	DDR_USI |= (1 << PORT_USI_SCL) | (1 << PORT_USI_SDA);

	// set SCL high
	PORT_USI |= (1 << PORT_USI_SCL);

	// set SDA high
	PORT_USI |= (1 << PORT_USI_SDA);

	// Set SDA as input
	DDR_USI &= ~(1 << PORT_USI_SDA);

	USICR =
	// enable Start Condition Interrupt
			(1 << USISIE) |
			// disable Overflow Interrupt
					(0 << USIOIE) |
					// set USI in Two-wire mode, no USI Counter overflow hold
					(1 << USIWM1) | (0 << USIWM0) |
					// Shift Register Clock Source = external, positive edge
					// 4-Bit Counter Source = external, both edges
					(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |
					// no toggle clock-port pin
					(0 << USITC);

	// clear all interrupt flags and reset overflow counter

	USISR = (1 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF)
			| (1 << USIDC);

} // end usiTwiSlaveInit

// put data in the transmission buffer, wait if buffer is full

void usiTwiTransmitByte(uint8_t data) {

	uint8_t tmphead;

	// calculate buffer index
	tmphead = (txHead + 1) & TWI_TX_BUFFER_MASK;

	// wait for free space in buffer
	while (tmphead == txTail)
		;

	// store data in buffer
	txBuf[tmphead] = data;

	// store new index
	txHead = tmphead;

} // end usiTwiTransmitByte

// return a byte from the receive buffer, wait if buffer is empty

uint8_t usiTwiReceiveByte(void) {
	// wait for Rx data
	while (rxHead == rxTail)
		;

	// calculate buffer index
	rxTail = (rxTail + 1) & TWI_RX_BUFFER_MASK;

	// return data from the buffer.
	return rxBuf[rxTail];
} // end usiTwiReceiveByte

/**
 * check if there is data in the receive buffer
 * @return 0 (false) if the receive buffer is empty
 */
bool usiTwiDataInReceiveBuffer(void) {
	return rxHead != rxTail;
} // end usiTwiDataInReceiveBuffer

/**
 * return value in specified register
 * @param reg is the register to return
 * @param value will have a copy of the register value if reg is in range
 * @return false if reg out of range, true otherwise
 */
bool usiTwiGetRegister(int reg, uint8_t *value) {
	bool result = false;

	if (reg >= 0 && reg < regmax) {
		*value = registers[reg];
		result = true;
	}

	return result;
}

/**
 * sets the specified register to the specified value
 * @param reg is the register to set
 * @param value is the value to be copied into the register
 * @return false if reg out of range, true otherwise
 */bool usiTwiSetRegister(int reg, uint8_t value) {
	bool result = false;

	if (reg >= 0 && reg < MAXREG) {
		registers[reg] = value;
		result = true;
	}

	return result;
}

// USI Start Condition ISR
//
ISR( USI_START_VECTOR ) {
	// set default starting conditions for new TWI package
	overflowState = USI_SLAVE_CHECK_ADDRESS;

	// set SDA as input
	DDR_USI &= ~(1 << PORT_USI_SDA);

	// wait for SCL to go low to ensure the Start Condition has completed (the
	// start detector will hold SCL low ) - if a Stop Condition arises then leave
	// the interrupt to prevent waiting forever - don't use USISR to test for Stop
	// Condition as in Application Note AVR312 because the Stop Condition Flag is
	// going to be set from the last TWI sequence
	// SCL his high and SDA is low
	while ((PIN_USI & (1 << PIN_USI_SCL)) && !((PIN_USI & (1 << PIN_USI_SDA))))
		// TODO add timeout?
		;

	if (!(PIN_USI & (1 << PIN_USI_SDA))) {
		// A STOP condition didn't occur, so set up for data transfer
		USICR = (1 << USISIE) |// keep Start Condition Interrupt enabled to detect RESTART
				(1 << USIOIE) |					// enable Overflow Interrupt
				(1 << USIWM1) | (1 << USIWM0) |// set USI in Two-wire mode, hold SCL low on USI Counter overflow
				(1 << USICS1) | (0 << USICS0) |// Shift Register Clock Source = External, positive edge
				(0 << USICLK) |// 4-Bit Counter Source = external, both edges
				(0 << USITC);					// no toggle clock-port pin
	} else {
		// a Stop Condition did occur, so reset, set up to watch for another start
		USICR = (1 << USISIE) |			// enable Start Condition Interrupt
				(0 << USIOIE) |				// disable Overflow Interrupt
				(1 << USIWM1) | (0 << USIWM0) |// set USI in Two-wire mode, no USI Counter overflow hold
				(1 << USICS1) | (0 << USICS0) |// Shift Register Clock Source = external, positive edge
				(0 << USICLK) |// 4-Bit Counter Source = external, both edges
				(0 << USITC);					// no toggle clock-port pin
	} // end if

	// clear interrupt flags - resetting the Start Condition Flag will release SCL
	USISR = (1 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF)
			| (1 << USIDC) | (0x0 << USICNT0); // set USI to sample 8 bits (count 16 external SCL pin toggles)

} // end ISR( USI_START_VECTOR )

// USI Overflow ISR
// Handles all the communication.
// Only disabled when waiting for a new Start Condition.
ISR( USI_OVERFLOW_VECTOR ) {
	uint8_t c;

	switch (overflowState) {
	case USI_SLAVE_CHECK_ADDRESS:
		// Address mode: check address and send ACK (and next USI_SLAVE_SEND_DATA) if OK,
		// else reset USI
		if ((USIDR == 0) || ((USIDR >> 1) == slaveAddress)) {
			if (USIDR & 0x01) {
				// READ
				overflowState = USI_SLAVE_SEND_DATA;
			} else {
				// WRITE
				overflowState = USI_SLAVE_REQUEST_DATA_REG;
			} // end if
			SET_USI_TO_SEND_ACK( )
			;
		} else {
			SET_USI_TO_TWI_START_CONDITION_MODE( )
			;
		}
		break;
	case USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA:
		// Master write data mode:
		// check reply and goto USI_SLAVE_SEND_DATA if ACK, if NACK reset USI
		if (USIDR ) {
			// if NACK, the master does not want more data
			current = 0; // if no more bytes requested, reset current register to 0
			SET_USI_TO_TWI_START_CONDITION_MODE( )
			;
			return;
		}
		// from here we just drop straight into USI_SLAVE_SEND_DATA if the
		// master sent an ACK
		// no break
	case USI_SLAVE_SEND_DATA:
		// send current register value
		// next USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA
		USIDR = registers[current++]; // loop through registers during this session
		if (current >= MAXREG) current = 0;
		overflowState = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
		SET_USI_TO_SEND_DATA( )
		;
		break;
	case USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA:
		// set USI to read reply from master
		// next USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA
		overflowState = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
		SET_USI_TO_READ_ACK( )
		;
		break;
	case USI_SLAVE_REQUEST_DATA_REG:
		// Master read data mode
		// Expects master to send register index first
		// Set USI to read data from master,
		// next USI_SLAVE_GET_REG_AND_SEND_ACK
		overflowState = USI_SLAVE_GET_REG_AND_SEND_ACK;
		SET_USI_TO_READ_DATA( )
		;
		break;
	case USI_SLAVE_GET_REG_AND_SEND_ACK:
		// copy data from USIDR and send ACK
		// next USI_SLAVE_REQUEST_DATA
		// set current to the first byte sent to us, the register index
		c = USIDR;
		if (c >= 0 && c < MAXREG) {
			current = c;
			overflowState = USI_SLAVE_REQUEST_DATA_VAL;
		} else {
			SET_USI_TO_TWI_START_CONDITION_MODE( )
			;
			return;
		}
		SET_USI_TO_SEND_ACK( )
		;
		break;
	case USI_SLAVE_REQUEST_DATA_VAL:
		// Set USI to read data from master,
		// next USI_SLAVE_GET_VAL_AND_SEND_ACK
		overflowState = USI_SLAVE_GET_VAL_AND_SEND_ACK;
		SET_USI_TO_READ_DATA( )
		;
		break;

	case USI_SLAVE_GET_VAL_AND_SEND_ACK:
		// put the next byte into current register if not read-only
		if (write_mask & (1<<current)) {
			registers[current] = USIDR;
		}
		overflowState = USI_SLAVE_REQUEST_DATA_REG;
		SET_USI_TO_SEND_ACK( )
		;
		break;

	} // end switch

} // end ISR( USI_OVERFLOW_VECTOR )
