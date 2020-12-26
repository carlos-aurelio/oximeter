#include <string.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "dev/uart.h"
#include "dev/ssd1306.h"
#include "dev/ssd1306_font.h"
#include "dev/max30102.h"
#include "spo2.h"
#include "debug.h"

#ifndef VERSION
#	define VERSION "0.0"
#endif
#ifdef DEBUG
#	define DEBUG_STR "*"
#else
#	define DEBUG_STR
#endif
#define VERSION_STRING "v." VERSION DEBUG_STR

#define FATAL_ERROR(s) fatal_error(PSTR(s))

uint32_t _clock_tick = 1;

ISR(TIMER1_OVF_vect)
{
	_clock_tick++;
}

void setup_interrupts(void)
{
	/* timer interrupt */
	TCNT1 = 0;
	OCR1A = 2499;
	TCCR1A = (1 << CS11) | (1 << CS10);
	TIMSK1 = 1 << OCIE1A;
	sei(); /* enable interrupts */
}

void fatal_error(const char *s)
{
	LOG_P(PSTR("FATAL ERROR:"));
	LOG_P(s);
	ssd1306_clear();
	ssd1306_font_setpos(0, 0);
	ssd1306_font_printf_P(PSTR("FATAL ERROR:"));
	ssd1306_font_setpos(0, 1);
	ssd1306_font_printf_P(s);
	for (;;)
		;
}

int main(void)
{
	int count = 0;
	static char *spinner[] = { ":;", ":<", "=:", ">:" };
	int spinner_pos = 0;
	int8_t n_samp;
	uint16_t t_samp = 0;
	uint32_t red, ir, spo2_percent, avg_red, avg_ir;
	static struct spo2 spo2;
	bool spo2_valid;
	bool err;
	const char *spo_fmt;
	
	/* led */
	DDRB |= 1 << PB5;

	/* initialize peripherals */
	uart_init();
	twi_init();

	/* init display */
	if (ssd1306_init(0x3c))
		FATAL_ERROR("display init");

	if (ssd1306_clear())
		FATAL_ERROR("display clear");

	/* init sensor */
	ssd1306_font_printf_P(PSTR("Initializing."));
	if (max30102_init(0x57))
		FATAL_ERROR("sensor init");

	/* config sensor */
	_delay_ms(100);
	ssd1306_font_printf_P(PSTR("."));
	if (max30102_config(4))
		FATAL_ERROR("sensor config");

	/* init data */
	_delay_ms(100);
	spo2_init(&spo2);
	ssd1306_font_printf_P(PSTR("."));

	/* UI bullshit */
	_delay_ms(100);
	ssd1306_clear();
	ssd1306_font_setpos(0, 0);


	LOG_P(PSTR("start"));
	ssd1306_clear();
	/* put software version in the bottom */
	ssd1306_font_setfont(ssd1306_font_6x8);
	ssd1306_font_setpos(0, 7);
	ssd1306_font_printf_P(PSTR(VERSION_STRING));
	ssd1306_font_setfont(ssd1306_font_64x32_spo2_symbol);
	/* SpO2 symbol is split to save ram */
	ssd1306_font_putc(1);
	ssd1306_font_putc(2);
	ssd1306_font_putc(3);
	ssd1306_font_putc(4);

	/* application loop */
	for(;;) {
		_delay_ms(100);
		PORTB ^= (1 << PINB5);
		n_samp = max30102_sample_ready();
		for (int8_t i = 0; i < n_samp; i++) {
			err = max30102_get_sample(&red, &ir);
			if (err)
				break;
			spo2_add_sample(&spo2, red, ir);
			LOGLV_P(2, PSTR("%lu,%lu\r\n"), red, ir);
		}

		spinner_pos = (spinner_pos + 1) % 4;
		spo2_percent = spo2_get_percent(&spo2, &spo2_valid);
		ssd1306_font_setfont(ssd1306_font_16x32_numbers);
		ssd1306_font_setpos(4, 0);
		if (spo2_valid) {
			if (spo2_percent < 10)
				spo_fmt = PSTR("::%d");
			else if (spo2_percent < 100)
				spo_fmt = PSTR(":%d");
			else
				spo_fmt = PSTR("%d");
			ssd1306_font_printf_P(spo_fmt, spo2_percent);
		} else {
			ssd1306_font_printf_P(PSTR(":%s"),
					      spinner[spinner_pos]);
		}
		ssd1306_font_setfont(ssd1306_font_8x16);
		ssd1306_font_setpos(2, 2);
		if (spinner_pos % 2)
			ssd1306_font_printf_P(PSTR(":"));
		else
			ssd1306_font_printf_P(PSTR(" "));
	}
}