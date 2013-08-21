/********************************************************************************

Header file for the USI TWI Slave driver.

Created by Donald R. Blake
donblake at worldnet.att.net

Modified by Michael Shimniok
http://www.bot-thoughts.com/

Modifications:
Using registers for read and write, now, sort of emulating an EEPROM
Also using a read-only mask
Using Bus Pirate notation; 0x4c=write, 0x4d=read ( start is [ stop is ] )
[0x4c 3 5] writes 5 to register 3, and sets current register to 3
[0x4d r] reads the current register, resets current register to 0 after
[0x4c 4 [0x4d r] reads register 4; current register reset to 0 after
[0x4c 4][0x4d r] reads register 4; current register reset to 0 after
[0x4d rr] reads the first two registers, resets current register to 0 after

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
  15 Mar 2007  Created.
  07 Aug 2013  Modified to use registers, add readonly mask, etc.

********************************************************************************/

#ifndef _USI_TWI_SLAVE_H_
#define _USI_TWI_SLAVE_H_


/********************************************************************************

                                    includes

********************************************************************************/

#include <stdbool.h>

/********************************************************************************

                                   prototypes

********************************************************************************/

/**
 * Initialize slave interface
 * @param addr is the 7-bit I2C address this interface will answer to
 * @param top is the maximum number of registers to make available (must be <= MAXREG)
 * @param wmask is the write mask, bits set to 1 are writeable registers
 */
void usiTwiSlaveInit(uint8_t addr, int top, uint32_t wmask);

/**
 * put data in the transmission buffer, wait if buffer is full
 * @param data is the data to push
 */
void usiTwiTransmitByte( uint8_t data);

/**
 * Return a byte from the receive buffer, wait if buffer is empty
 *
 * @return next byte from receive buffer
 */
uint8_t usiTwiReceiveByte( void );

/**
 * check if there is data in the receive buffer
 * @return 0 (false) if the receive buffer is empty
 */
bool usiTwiDataInReceiveBuffer( void );



/********************************************************************************

                           driver buffer definitions

********************************************************************************/

// permitted RX buffer sizes: 1, 2, 4, 8, 16, 32, 64, 128 or 256

#define TWI_RX_BUFFER_SIZE  ( 16 )
#define TWI_RX_BUFFER_MASK  ( TWI_RX_BUFFER_SIZE - 1 )

#if ( TWI_RX_BUFFER_SIZE & TWI_RX_BUFFER_MASK )
#  error TWI RX buffer size is not a power of 2
#endif

// permitted TX buffer sizes: 1, 2, 4, 8, 16, 32, 64, 128 or 256

#define TWI_TX_BUFFER_SIZE ( 16 )
#define TWI_TX_BUFFER_MASK ( TWI_TX_BUFFER_SIZE - 1 )

#if ( TWI_TX_BUFFER_SIZE & TWI_TX_BUFFER_MASK )
#  error TWI TX buffer size is not a power of 2
#endif



#endif  // ifndef _USI_TWI_SLAVE_H_
