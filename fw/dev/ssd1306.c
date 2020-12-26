#include "ssd1306.h"
#include "../assert.h"

static const uint8_t _init[] = {
	0xAE,		/* Display OFF (sleep mode) */
	0x20, 0b00,	/* Set Memory Addressing Mode
			 * 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
                         * 10=Page Addressing Mode (RESET); 11=Invalid
			 */
	0xB0,		/* Set Page Start Address for Page Addressing Mode, 0-7 */
	0xC8,                   // Set COM Output Scan Direction
        0x00,                   // ---set low column address
        0x10,                   // ---set high column address
        0x40,                   // --set start line address
        0x81, 0x3F,             // Set contrast control register
        0xA1,                   // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
        0xA6,                   // Set display mode. A6=Normal; A7=Inverse
        0xA8, 0x3F,             // Set multiplex ratio(1 to 64)
        0xA4,                   // Output RAM to Display
                                        // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
        0xD3, 0x00,             // Set display offset. 00 = no offset
        0xD5,                   // --set display clock divide ratio/oscillator frequency
        0x80,                   // --set divide ratio
        0xD9, 0x22,             // Set pre-charge period
        0xDA, 0x12,             // Set com pins hardware configuration
        0xDB,                   // --set vcomh
        0x40,                   // 0x20,0.77xVcc
        0x8D, 0x14,             // Set DC-DC enable
        0xAF                    // Display ON in normal mode
};
static uint8_t _addr = 0;

static bool _send_cmd_byte(uint8_t cmd)
{
	static uint8_t buf[2] = { 0 };

	buf[1] = cmd;
	return twi_send(_addr, buf, 2);
}

bool ssd1306_init(uint8_t twi_addr)
{
	bool err = false;

	_addr = twi_addr;
	for (size_t i = 0; i < sizeof(_init); i++) {
		if (_send_cmd_byte(_init[i]))
			goto exit;
	}
	LOG_P(PSTR("init ok"));
	err = false;
exit:
	return err;
}

bool ssd1306_clear(void)
{
	bool err = true;

	for (size_t i = 0; i < 8; i++)
		assert(ssd1306_clear_line(i));
	err = false;
exit:
	return err;
}

bool ssd1306_clear_line(uint8_t y8)
{
	bool err = true;

	assert(ssd1306_setpos(0, y8));
	assert(twi_start());
	assert(twi_addr_write(_addr));
	assert(twi_data_write(0x40)); /* write to ram */
	for (size_t i = 0; i < 128; i++)
		assert(twi_data_write(0x00));
	twi_stop();
	err = false;
exit:
	return err;
}

bool ssd1306_setpos(uint8_t x, uint8_t y8)
{
	bool err = true;
	assert(_send_cmd_byte(0xb0 | (y8 & 0x0f)));
	assert(_send_cmd_byte(x & 0x0f));
	assert(_send_cmd_byte(((x >> 4) & 0x0f) | 0x10));
	err = false;
exit:
	return err;
}

bool ssd1306_fillb(uint8_t b)
{
	return ssd1306_fill(&b, 1);
}

bool ssd1306_fill(const uint8_t *b, size_t len)
{
	bool err = true;

	assert(twi_start());
	assert(twi_addr_write(_addr));
	assert(twi_data_write(0x40)); /* write to ram */
	for (size_t i = 0; i < len; i++)
		assert(twi_data_write(b[i]));
	twi_stop();
	err = false;
exit:
	return err;
}
