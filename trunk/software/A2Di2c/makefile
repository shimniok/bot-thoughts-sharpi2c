#!/bin/sh

AVRDUDE=/usr/local/bin/avrdude
PROJECT=i2cadc
TARGET=t44
#Pololu programmer
PORT=/dev/pololu0
PROGRAMMER=avrispv2
#JTAG ICE MkII
#PORT=usb
#PROGRAMMER=jtag2isp
#AVRDUDE_FLAGS=-F

install:
	(cd Release; $(AVRDUDE) $(AVRDUDE_FLAGS) -p $(TARGET) -c $(PROGRAMMER) -P $(PORT) -v -U flash:w:$(PROJECT).hex)

fuses:
	$(AVRDUDE) -p $(TARGET) -c $(PGM) -P $(DEV) -v -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

