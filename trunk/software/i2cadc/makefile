#!/bin/sh

PROJECT=i2cadc
TGT=t84
DEV=/dev/pololu0
PGM=avrispv2

install:
	(cd Release; avrdude -p $(TGT) -c $(PGM) -P $(DEV) -v -U flash:w:$(PROJECT).hex)

fuses:
	avrdude -p $(TGT) -c $(PGM) -P $(DEV) -v -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

