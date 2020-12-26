#include <stdio.h>
#include "twi.h"
#include "max30102.h"
#include "../assert.h"

static uint8_t _addr = 0;

static bool write_reg(uint8_t reg, uint8_t val)
{
	return twi_write_reg(_addr, reg, &val, 1);
}

static bool read_reg(uint8_t reg, uint8_t *val)
{
	return twi_read_reg(_addr, reg, val, 1);
}

bool max30102_init(uint8_t twi_addr)
{
	bool err = true;
	uint8_t id = 0, rev = 0;

	_addr = twi_addr;
	assert(read_reg(0xfe, &rev));
	assert(read_reg(0xff, &id));
	LOG_P(PSTR("ID: 0x%02X"), id);
	LOG_P(PSTR("REV: %hhu"), rev);
	assert(id != 0x15);
	LOG_P(PSTR("init ok"));
	err = false;
exit:
	return err;
}

bool max30102_reset(void)
{
	return write_reg(0x09, 0x43);
}

bool max30102_config(uint8_t sample_avg)
{
	bool err = true;
	uint8_t fifo_config_reg = 0;
	uint8_t sample_avg_bits = 0;
	uint8_t spo2_config_reg = 0;

	assert(twi_read_reg(_addr, 0x08, &fifo_config_reg, 1));
	fifo_config_reg &= ~0xf0;
	switch (sample_avg) {
	case 32:
		sample_avg_bits++;
	case 16:
		sample_avg_bits++;
	case 8:
		sample_avg_bits++;
	case 4:
		sample_avg_bits++;
	case 2:
		sample_avg_bits++;
		break;
	case 1:
		break;
	default:
		sample_avg_bits = 2;
	}
	fifo_config_reg |= (sample_avg_bits << 5);

	assert(write_reg(0x08, fifo_config_reg));
	assert(write_reg(0x09, 3)); /* spo2 mode */

	spo2_config_reg |= (1 << 5); /* adc range full scale: 4096 */
	spo2_config_reg |= (2 << 2); /* 200 samples/sec */
	spo2_config_reg |= 3; /* 18 bit resolution */
	assert(write_reg(0x0a, spo2_config_reg));
	assert(write_reg(0x0c, 0x24)); /* red = X */
	assert(write_reg(0x0d, 0x24)); /* red = X */
	

	err = false;
exit:
	return err;
}

static uint32_t le3_to_cpu(const uint8_t *buf)
{
	return ((uint32_t)(buf[0] & 0x03) << 16)
		| ((uint32_t)(buf[1]) << 8)
		| (uint32_t)(buf[2]);
}

bool max30102_get_sample(uint32_t *red, uint32_t *ir)
{
	bool err = true;
	uint8_t buf[6] = { 0 };

	/* get 6 bytes from fifo */
	assert(twi_read_reg(_addr, 0x07, buf, sizeof(buf)));
	/* align data and return */
	*red = le3_to_cpu(buf);
	*ir = le3_to_cpu(buf + 3);
	err = false;
exit:
	return err;
}

int8_t max30102_sample_ready(void)
{
	int8_t ret = -1;
	uint8_t wr_ptr, rd_ptr, ov;

	assert(read_reg(0x04, &wr_ptr));
	assert(read_reg(0x06, &rd_ptr));
	if (wr_ptr > rd_ptr) {
		ret = wr_ptr - rd_ptr;
	} else if (wr_ptr < rd_ptr) {
		ret = wr_ptr + 32 - rd_ptr;
	} else {
		assert(read_reg(0x05, &ov));
		if (ov)
			ret = 32;
	}
exit:
	return ret;
}