#ifndef __SSD1306_H__
#define __SSD1306_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "twi.h"

bool ssd1306_init(uint8_t twi_addr);
bool ssd1306_clear(void);
bool ssd1306_clear_line(uint8_t y8);
bool ssd1306_setpos(uint8_t x, uint8_t y8);
bool ssd1306_fillb(uint8_t b);
bool ssd1306_fill(const uint8_t *b, size_t len);


#endif/*__SSD1306_H__*/