#ifndef __TWI_H__
#define __TWI_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef SCL_FREQ
#	define SCL_FREQ 400000
#endif

/*
 * Return value: 0 -> success.
 */

void twi_init(void);

bool twi_start(void);
bool twi_rep_start(void);
void twi_stop(void);
bool twi_addr_write(uint8_t addr);
bool twi_addr_read(uint8_t addr);
bool twi_data_write(uint8_t data);
bool twi_data_write_buf(const uint8_t *data, size_t len);

bool twi_data_read_ack(uint8_t *data);
bool twi_data_read_ack_buf(uint8_t *data, size_t len);
bool twi_data_read_nack(uint8_t *data);
bool twi_data_read_nack_buf(uint8_t *data, size_t len);

/* higher level functions */
bool twi_send_byte(uint8_t addr, uint8_t data);
bool twi_send(uint8_t addr, const uint8_t *data, size_t len);
bool twi_recv(uint8_t addr, uint8_t *data, size_t len);
bool twi_write_reg(uint8_t addr, uint8_t reg, const uint8_t *data, size_t len);
bool twi_read_reg(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);
bool twi_read_next_reg(uint8_t addr, uint8_t *data, size_t len);

#endif/*__TWI_H__*/