#ifndef __MAX30102_H__
#define __MAX30102_H__

#include <stdint.h>
#include <stdbool.h>
#include "twi.h"

bool max30102_init(uint8_t twi_addr);
bool max30102_reset(void);
bool max30102_config(uint8_t sample_avg);
bool max30102_get_sample(uint32_t *red, uint32_t *ir);

/* returns number of samples available, or -1 on error */
int8_t max30102_sample_ready(void);

#endif/*__MAX30102_H__*/