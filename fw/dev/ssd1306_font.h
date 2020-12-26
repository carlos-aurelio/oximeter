#ifndef __SSD1306_FONT_H__
#define __SSD1306_FONT_H__

#include <avr/pgmspace.h>
#include "ssd1306.h"

enum ssd1306_font {
	ssd1306_font_6x8,
	ssd1306_font_8x16,
	ssd1306_font_16x32_numbers,
	ssd1306_font_64x32_spo2_symbol,
	ssd1306_font_max,
};

bool ssd1306_font_setfont(enum ssd1306_font font);
bool ssd1306_font_setpos(uint8_t col, uint8_t line);
bool ssd1306_font_setoffset(uint8_t x, uint8_t y);
bool ssd1306_font_putc(uint8_t c);
bool ssd1306_font_printf_P(const char *fmt_P, ...);

#endif/*__SSD1306_FONT_H__*/