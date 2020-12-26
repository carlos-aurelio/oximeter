#include "twi.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/twi.h>
#include <avr/pgmspace.h>
#include "../assert.h"

/* From ATmega328P Datasheet 21.5.2:
 * SCL_FREQ = CPU_FREQ / (16 + 2 * TWBR * PRESCALER)
 * TWBR -> 8 bits
 * 
 * First check viable prescaler values for given SCL frequency.
 * Then calculate value for TWBR.
 */
#if ((F_CPU / SCL_FREQ) - 16) / (2 * 1) < 0x100
#	define PRESCALER_VAL 1
#	define TWPS0_VAL 0
#	define TWPS1_VAL 0
#elif ((F_CPU / SCL_FREQ) - 16) / (2 * 4) < 0x100
#	define PRESCALER_VAL 4
#	define TWPS0_VAL 1
#	define TWPS1_VAL 0
#elif ((F_CPU / SCL_FREQ) - 16) / (2 * 16) < 0x100
#	define PRESCALER_VAL 16
#	define TWPS0_VAL 0
#	define TWPS1_VAL 1
#elif ((F_CPU / SCL_FREQ) - 16) / (2 * 16) < 0x100
#	define PRESCALER_VAL 64
#	define TWPS0_VAL 1
#	define TWPS1_VAL 1
#else
#	error "Invalid SCL_FREQ"
#endif
#define TWBR_VAL (((F_CPU / SCL_FREQ) - 16) / (2 * PRESCALER_VAL))

#define TWI_ADDR_W(addr) ((addr) << 1)
#define TWI_ADDR_R(addr) (((addr) << 1) | 1)

static void twi_wait_complete(void)
{
	while(!(TWCR & (1 << TWINT)))
		;
}

void twi_init(void)
{
	TWBR = TWBR_VAL;
	TWSR = (TWPS1_VAL << TWPS1) | (TWPS0_VAL << TWPS0);
	TWCR = 1 << TWEN;
}

static void twi_gen_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	twi_wait_complete();
}

bool twi_start(void)
{
	twi_gen_start();
	return TW_STATUS != TW_START;
}

bool twi_rep_start(void)
{
	twi_gen_start();
	return TW_STATUS != TW_REP_START;
}

void twi_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

bool twi_addr_write(uint8_t addr)
{
	TWDR = TWI_ADDR_W(addr);
	TWCR = (1 << TWINT) | (1 << TWEN);
	twi_wait_complete();
	return TW_STATUS != TW_MT_SLA_ACK;
}

bool twi_addr_read(uint8_t addr)
{
	TWDR = TWI_ADDR_R(addr);
	TWCR = (1 << TWINT) | (1 << TWEN);
	twi_wait_complete();
	return TW_STATUS != TW_MR_SLA_ACK;
}

bool twi_data_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	twi_wait_complete();
	return TW_STATUS != TW_MT_DATA_ACK;
}

bool twi_data_write_buf(const uint8_t *data, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (twi_data_write(data[i]))
			return 1;
	}
	return 0;
}

bool twi_data_read_ack(uint8_t *data)
{
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
	twi_wait_complete();
	*data = TWDR;
	return TW_STATUS != TW_MR_DATA_ACK;
}

bool twi_data_read_ack_buf(uint8_t *data, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (twi_data_read_ack(&data[i]))
			return 1;
	}
	return 0;
}

bool twi_data_read_nack(uint8_t *data)
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	twi_wait_complete();
	*data = TWDR;
	return TW_STATUS != TW_MR_DATA_NACK;
}

bool twi_data_read_nack_buf(uint8_t *data, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		if (twi_data_read_nack(&data[i]))
			return 1;
	}
	return 0;
}

bool twi_send_byte(uint8_t addr, uint8_t data)
{
	bool err = true;

	assert(twi_start());
	assert(twi_addr_write(addr));
	assert(twi_data_write(data));
	twi_stop();
	err = false;
exit:
	return err;
}

bool twi_send(uint8_t addr, const uint8_t *data, size_t len)
{
	bool err = true;

	assert(twi_start());
	assert(twi_addr_write(addr));
	assert(twi_data_write_buf(data, len));
	twi_stop();
	err = false;
exit:
	return err;
}

bool twi_recv(uint8_t addr, uint8_t *data, size_t len)
{
	bool err = true;

	assert(twi_start());
	assert(twi_addr_read(addr));
	assert(twi_data_read_ack_buf(data, len - 1));
	assert(twi_data_read_nack(&data[len - 1]));
	twi_stop();
	err = false;
exit:
	return err;
}

bool twi_write_reg(uint8_t addr, uint8_t reg, const uint8_t *data, size_t len)
{
	bool err = true;
	
	assert(twi_start());
	assert(twi_addr_write(addr));
	assert(twi_data_write(reg));
	assert(twi_data_write_buf(data, len));
	twi_stop();
	err = false;
exit:
	return err;
}

bool twi_read_reg(uint8_t addr, uint8_t reg, uint8_t *data, size_t len)
{
	bool err = true;
	
	assert(twi_start());
	assert(twi_addr_write(addr));
	assert(twi_data_write(reg));
	assert(twi_rep_start());
	assert(twi_addr_read(addr));
	assert(twi_data_read_ack_buf(data, len - 1));
	assert(twi_data_read_nack(&data[len - 1]));
	twi_stop();
	err = false;
exit:
	return err;
}

bool twi_read_next_reg(uint8_t addr, uint8_t *data, size_t len)
{
	bool err = true;

	assert(twi_start());
	assert(twi_addr_read(addr));
	assert(twi_data_read_ack_buf(data, len - 1));
	assert(twi_data_read_nack(&data[len - 1]));
	twi_stop();
	err = false;
exit:
	return err;
}
