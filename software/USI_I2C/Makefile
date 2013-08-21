##
## Makefile for A2D library based on Eclipse auto-generated makefile
##

PROJECT  = libUSI_I2C
C_SRCS   = usiTwiSlave.c 
OBJS     = usiTwiSlave.o 
INCLUDES = -I"./include" -I"../A2D/include"

F_CPU    = 8000000UL
MCU      = attiny44a
CFLAGS   =  -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=$(F_CPU) 
 
RM := rm -rf

# Add inputs and outputs from these tool invocations to the build variables 
LSS += $(PROJECT).lss

SIZEDUMMY += sizedummy

# All Target
all: $(PROJECT).a secondary-outputs

# Tool invocations
$(PROJECT).a: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR Archiver'
	avr-ar -r  "$(PROJECT).a" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(PROJECT).lss: $(PROJECT).a
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S $(PROJECT).a  >"$(PROJECT).lss"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: $(PROJECT).a
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t $(PROJECT).a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(ARCHIVES)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) $(PROJECT).a
	-@echo ' '

secondary-outputs: $(LSS) $(SIZEDUMMY)

%.o: %.c
	@echo 'Building file: $<'
	avr-gcc $(INCLUDES) $(CFLAGS) -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

.PHONY: all clean dependents
.SECONDARY:

