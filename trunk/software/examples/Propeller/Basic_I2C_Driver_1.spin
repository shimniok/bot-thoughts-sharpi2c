'' Basic I2C Routines  Version 1.3
'' Written by Michael Green and copyright (c) 2009
'' Permission is given to use this in any program for the Parallax
'' Propeller processor as long as this copyright notice is included.

'' Version: 1.0 - Initial version
''          1.1 - Corrected some comments
''          1.2 - Corrected example, added this list
''          1.3 - Added logic for non-memory devices

'' This is a minimal version of an I2C driver in SPIN.  It assumes
'' that the SDA pin is one higher than the SCL pin.  It assumes that
'' neither the SDA nor the SCL pins have pullups, so drives both.

'' These routines are primarily intended for reading and writing EEPROMs.
'' The low level I2C are provided for use with other devices, but the
'' read/write byte routines assume a standard I2C serial EEPROM with a
'' 16 bit device address register, paged writes, and acknowledge polling.

'' All of these read/write routines accept an EEPROM address up to 19
'' bits (512K) even though the EEPROM addressing scheme normally allows
'' for only 16 bits of addressing.  The upper 3 bits are used as part of
'' the device select code and these routines will take the upper 3 bits
'' of the address and "or" it with the supplied device select code bits
'' 3-1 which are used to select a particular EEPROM on an I2C bus.  There
'' are two schemes for selecting 64K "banks" in 128Kx8 EEPROMs.  Atmel's
'' 24LC1024 EEPROMs allow simple linear addressing up to 256Kx8 ($00000
'' to $3FFFF).  Microchip's 24LC1025 allows for up to 512Kx8, but in two
'' areas: $00000 to $3FFFF and $40000 to $7FFFF.  Each EEPROM provides
'' a 64K "bank" in each area.  See the device datasheets for details.

'' If bit 21 of the address value is non-zero, then bit 20 indicates how
'' many address bytes are to be sent to the device.  If bit 20 is zero,
'' no address is sent.  If bit 20 is a one, then a single address byte
'' is sent.
         
'' This will work with the boot EEPROM and does not require a pull-up
'' resistor on the SCL line (but does on the SDA line ... about 4.7K to
'' +3.3V).  According to the Philips I2C specification, both pull-ups
'' are required.  Many devices will tolerate the absence of a pull-up
'' on SCL.  Some may tolerate the absence of a pull-up on SDA as well.

'' Initialize may have to be called once at the beginning of your
'' program.  Sometimes an I2C device is left in an invalid state.  This
'' will reset the device to a known state so it will respond to the I2C
'' start transition (sent out by the i2cStart routine).

'' To read from or write to an EEPROM on pins 28/29 like the boot EEPROM:

'' CON
''   eepromAddress = $7000

'' VAR
''   byte buffer[32]

'' OBJ
''   i2c : "Basic_I2C_Driver"

'' PRI readIt
''   if i2c.ReadPage(i2c#BootPin, i2c#EEPROM, eepromAddress, @buffer, 32)
''     abort ' an error occurred during the read

'' PRI writeIt | startTime
''   if i2c.WritePage(i2c#BootPin, i2c#EEPROM, eepromAddress, @buffer, 32)
''     abort ' an error occured during the write
''   startTime := cnt ' prepare to check for a timeout
''   repeat while i2c.WriteWait(i2c#BootPin, i2c#EEPROM, eepromAddress)
''     if cnt - startTime > clkfreq / 10
''       abort ' waited more than a 1/10 second for the write to finish

'' Note that the read and write use something called paged reads/writes.
'' This means that any read using ReadPage must fit entirely in one
'' EEPROM if you have several attached to one set of pins.  For writes,
'' any write using i2cWritePage must fit entirely within a page of the
'' EEPROM.  Usually these pages are either 32, 64, 128 or 256 bytes in
'' size depending on the manufacturer and device type.  32 bytes is a
'' good limit for the number of bytes to be written at a time if you
'' don't know the specific page size (and the write must fit completely
'' within a multiple of the page size).  The WriteWait waits for the
'' write operation to complete.  Alternatively, you could wait for 5ms
'' since currently produced EEPROMs will finish within that time.

'' To read from an I2C device that uses a single register address byte,
'' you could use any of these methods and supply a device address as
'' follows:

'' i2c.ReadByte( <SCL pin #>, <device code>, <register> | i2c#OneAddr)

'' Similarly, to write to a device that doesn't use an address byte,
'' you'd use:

'' i2c.WriteByte( <SCL pin #>, <device code>, i2c#NoAddr)

CON
   ACK      = 0                        ' I2C Acknowledge
   NAK      = 1                        ' I2C No Acknowledge
   Xmit     = 0                        ' I2C Direction Transmit
   Recv     = 1                        ' I2C Direction Receive
   BootPin  = 28                       ' I2C Boot EEPROM SCL Pin
   EEPROM   = $A0                      ' I2C EEPROM Device Address
   NoAddr   = $200000                  ' I2C Don't use register address
   OneAddr  = $300000                  ' I2C Use 8-bit register address

PUB Initialize(SCL) | SDA              ' An I2C device may be left in an
   SDA := SCL + 1                      '  invalid state and may need to be
   outa[SCL]~
   outa[SDA]~
   dira[SCL]~                          ' float SCL high
   dira[SDA]~                          ' float SDA high
   repeat 9
      if ina[SDA]                      ' Repeat if SDA not driven high
         quit
      dira[SCL]~~                      ' Put out up to 9 clock pulses
      dira[SCL]~

PUB Start(SCL) | SDA                   ' SDA goes HIGH to LOW with SCL HIGH
   SDA := SCL + 1
   outa[SCL]~
   outa[SDA]~
   dira[SCL]~                          ' Initially float SCL HIGH
   dira[SDA]~                          ' Initially float SDA HIGH
   dira[SDA]~~                         ' pull SDA LOW
   dira[SCL]~~                         ' pull SCL LOW

PUB Stop(SCL) | SDA                    ' SDA goes LOW to HIGH with SCL High
   SDA := SCL + 1
   dira[SDA]~~                         ' SDA goes low
   dira[SCL]~                          ' float SCL HIGH
   dira[SDA]~                          ' float SDA HIGH

PUB Write(SCL, data) : ackbit | SDA
'' Write i2c data.  Data byte is output MSB first, SDA data line is valid
'' only while the SCL line is HIGH.  Data is always 8 bits (+ ACK/NAK).
'' SDA is assumed LOW and SCL and SDA are both left in the LOW state.
   SDA := SCL + 1
   ackbit := 0 
   data <<= 24
   repeat 8                            ' Output data to SDA
      dira[SDA] := ((data<-=1)&1)^1
      dira[SCL]~                       ' float SCL HIGH (is low entering in here)
      dira[SCL]~~                      ' pull SCL low again
   dira[SDA]~                          ' float SDA HIGH for ACK/NACK
   dira[SCL]~                          ' float SCL HIGH
   ackbit := ina[SDA]                  ' Sample SDA when SCL is HIGH
   dira[SCL]~~                         ' pull SCL LOW
   dira[SDA]~                          ' float SDA HIGH
   dira[SDA]~~                         ' Leave SDA pulled LOW

PUB Read(SCL, ackbit): data | SDA
'' Read in i2c data, Data byte is output MSB first, SDA data line is
'' valid only while the SCL line is HIGH.  SCL and SDA left in LOW state.
   SDA := SCL + 1
   data := 0
   dira[SDA]~                          ' Float SDA high
   repeat 8                            ' Receive data from SDA
      dira[SCL]~                       ' Sample SDA when SCL is floating HIGH
      data := (data << 1) | ina[SDA]
      dira[SCL]~~                      ' pull SCL LOW
   if (ackbit)
     dira[SDA]~
   else
     dira[SDA]~~
   dira[SDA]~~
   dira[SCL]~~                         ' float SCL HIGH
   dira[SCL]~                          ' Pull SCL LOW
   dira[SDA]~~                         ' Leave SDA driven LOW

PUB ReadPage(SCL, devSel, addrReg, dataPtr, count) : ackbit
'' Read in a block of i2c data.  Device select code is devSel.  Device starting
'' address is addrReg.  Data address is at dataPtr.  Number of bytes is count.
'' The device select code is modified using the upper 3 bits of the 19 bit addrReg.
'' Return zero if no errors or the acknowledge bits if an error occurred.
   devSel |= addrReg >> 15 & %1110
   if addrReg & $300000 <> $200000     ' One or two byte address or memory?
      Start(SCL)                       ' Select the device & send address
      ackbit := Write(SCL, devSel | Xmit)
      if addrReg & $300000 == $000000  ' Two byte address or memory device?
         ackbit := (ackbit << 1) | Write(SCL, addrReg >> 8 & $FF)
      ackbit := (ackbit << 1) | Write(SCL, addrReg & $FF)          
   Start(SCL)                          ' Reselect the device for reading
   ackbit := (ackbit << 1) | Write(SCL, devSel | Recv)
   repeat count - 1
      byte[dataPtr++] := Read(SCL, ACK)
   byte[dataPtr++] := Read(SCL, NAK)
   Stop(SCL)
   return ackbit

PUB ReadByte(SCL, devSel, addrReg) : data
'' Read in a single byte of i2c data.  Device select code is devSel.  Device
'' starting address is addrReg.  The device select code is modified using the
'' upper 3 bits of the 19 bit addrReg.  This returns true if an error occurred.
   if ReadPage(SCL, devSel, addrReg, @data, 1)
      return -1

PUB ReadWord(SCL, devSel, addrReg) : data
'' Read in a single word of i2c data.  Device select code is devSel.  Device
'' starting address is addrReg.  The device select code is modified using the
'' upper 3 bits of the 19 bit addrReg.  This returns true if an error occurred.
   if ReadPage(SCL, devSel, addrReg, @data, 2)
      return -1

PUB ReadLong(SCL, devSel, addrReg) : data
'' Read in a single long of i2c data.  Device select code is devSel.  Device
'' starting address is addrReg.  The device select code is modified using the
'' upper 3 bits of the 19 bit addrReg.  This returns true if an error occurred.
'' Note that you can't distinguish between a return value of -1 and true error.
   if ReadPage(SCL, devSel, addrReg, @data, 4)
      return -1

PUB WritePage(SCL, devSel, addrReg, dataPtr, count) : ackbit
'' Write out a block of i2c data.  Device select code is devSel.  Device starting
'' address is addrReg.  Data address is at dataPtr.  Number of bytes is count.
'' The device select code is modified using the upper 3 bits of the 19 bit addrReg.
'' Most devices have a page size of at least 32 bytes, some as large as 256 bytes.
'' Return zero if no errors or the acknowledge bits if an error occurred.  If
'' more than 31 bytes are transmitted, the sign bit is "sticky" and is the
'' logical "or" of the acknowledge bits of any bytes past the 31st.
   devSel |= addrReg >> 15 & %1110
   Start(SCL)                          ' Select the device & send address
   ackbit := Write(SCL, devSel | Xmit)
   if addrReg & $300000 <> $200000     ' One or two byte address or memory?
      if addrReg & $300000 == $000000  ' Two byte address or memory device?
         ackbit := (ackbit << 1) | Write(SCL, addrReg >> 8 & $FF)
      ackbit := (ackbit << 1) | Write(SCL, addrReg & $FF)          
   repeat count                        ' Now send the data
      ackbit := ackbit << 1 | ackbit & $80000000 ' "Sticky" sign bit         
      ackbit |= Write(SCL, byte[dataPtr++])
   Stop(SCL)
   return ackbit

PUB WriteByte(SCL, devSel, addrReg, data)
'' Write out a single byte of i2c data.  Device select code is devSel.  Device
'' starting address is addrReg.  The device select code is modified using the
'' upper 3 bits of the 19 bit addrReg.  This returns true if an error occurred.
   if WritePage(SCL, devSel, addrReg, @data, 1)
      return true
   return false

PUB WriteWord(SCL, devSel, addrReg, data)
'' Write out a single word of i2c data.  Device select code is devSel.  Device
'' starting address is addrReg.  The device select code is modified using the
'' upper 3 bits of the 19 bit addrReg.  This returns true if an error occurred.
'' Note that the word value may not span an EEPROM page boundary.
   if WritePage(SCL, devSel, addrReg, @data, 2)
      return true
   return false

PUB WriteLong(SCL, devSel, addrReg, data)
'' Write out a single long of i2c data.  Device select code is devSel.  Device
'' starting address is addrReg.  The device select code is modified using the
'' upper 3 bits of the 19 bit addrReg.  This returns true if an error occurred.
'' Note that the long word value may not span an EEPROM page boundary.
   if WritePage(SCL, devSel, addrReg, @data, 4)
      return true
   return false

PUB WriteWait(SCL, devSel, addrReg) : ackbit
'' Wait for a previous write to complete.  Device select code is devSel.  Device
'' starting address is addrReg.  The device will not respond if it is busy.
'' The device select code is modified using the upper 3 bits of the 18 bit addrReg.
'' This returns zero if no error occurred or one if the device didn't respond.
   devSel |= addrReg >> 15 & %1110
   Start(SCL)
   ackbit := Write(SCL, devSel | Xmit)
   Stop(SCL)
   return ackbit
