# Controller settings
MMCU=atmega328p
F_CPU=16000000UL
PROGRAMMER_ID=arduino

# Add used USB port here (e.g. /dev/ttyACM0)
PORT=/dev/

# Files to build
TARGET = main
SRC = $(TARGET).c libs/logger.c libs/suart_midi.c libs/midi.c libs/poti.c button.c


OBJ = $(SRC:%.c=%.o)
CFLAGS=-g -Wall -mmcu=${MMCU} -DF_CPU=${F_CPU} -Os -mcall-prologues

flash: build clean
	avrdude -p ${MMCU} -c $(PROGRAMMER_ID) -P $(PORT) -vv -U flash:w:$(TARGET).hex

build: $(TARGET).hex size

%.hex: $(TARGET).elf
	avr-objcopy -O ihex $< $@

%.elf: $(OBJ)
	avr-gcc ${CFLAGS} $^ -o $@

%.o: %.c
	avr-gcc -c ${CFLAGS} $< -o $@

clean:
	rm *.obj *.hex *.o *.elf

size: $(TARGET).elf
	avr-size --format=avr --mcu=$(MMCU) $^

.PHONY: flash build clean size
.DEFAULT_GOAL := build
