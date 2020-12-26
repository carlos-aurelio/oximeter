PORT		?= COM10

MCU		?= atmega328p
CLK		?= 16000000
PRG		?= arduino

AVRDUDE		:= avrdude -c ${PRG} -v -v -V -p ${MCU} -b 57600
SIZE		:= avr-size --format=avr --mcu=${MCU}
OBJCOPY		?= avr-objcopy
CC		?= avr-gcc
LD		?= avr-ld

src		:= $(wildcard *.c) $(wildcard dev/*.c)

avr_cflags	:= -Os -DF_CPU=${CLK} -D__AVR_ATmega328P__ -mmcu=${MCU} -I drivers/inc

ifeq (${CC},avr-gcc)
    platform_cflags := ${avr_cflags}
else
    platform_cflags := -O0 -g3
endif

git_version := $(shell git describe --tags --always --dirty || echo 0.0)

all_cflags	:= -Wall -Wextra -Wno-unused -DVERSION=\"${git_version}\" ${platform_cflags} ${CPPFLAGS} ${CFLAGS}
all_ldflags	:= ${LDFLAGS}
all_flags	:= ${all_cflags} ${all_ldflags}

all_hostcflags	:= -Wall -Wextra -Wno-unused -DHOST -g3 -O0 ${HOSTCFLAGS} ${HOSTCPPFLAGS}
all_hostldflags	:= -lm ${HOSTLDFLAGS}
all_hostflags	:= ${all_hostcflags} ${all_hostldflags}

all: oximeter.hex

flash: oximeter.hex
	${AVRDUDE} -P ${PORT} -U flash:w:$<:i

size: oximeter.elf
	${SIZE} $<

oximeter.hex: oximeter.elf
	${RM} $@
	${OBJCOPY} -j .text -j .data -O ihex $< $@
	${SIZE} $<

oximeter.elf: ${src}
	${CC} -o $@ ${src} ${all_flags}

clean:
	${RM} oximeter.elf oximeter.hex

.PHONY: all flash size clean