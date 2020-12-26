#ifndef __SPO2_H__
#define __SPO2_H__

#include <stdint.h>
#include <stdbool.h>

/* Watch out for RAM usage. You need to have at least 12x this value for the
   red and ir buffer data alone, see struct below */

/* red and ir samples to store */
#ifndef	SPO2_SAMPLE_BUF_SIZE
#	define SPO2_SAMPLE_BUF_SIZE 100
#endif
/* ratios to store (used to average) */
#ifndef SPO2_RATIO_BUF_SIZE
#	define SPO2_RATIO_BUF_SIZE 20
#endif

/* Readings below this will not be added as sample to the FIFO. */
#ifndef SPO2_MIN_SENSOR_VAL
#	define SPO2_MIN_SENSOR_VAL 10000
#endif

struct spo2 {
	uint32_t buf_red[SPO2_SAMPLE_BUF_SIZE];
	uint32_t buf_ir[SPO2_SAMPLE_BUF_SIZE];
	float r[SPO2_RATIO_BUF_SIZE];
	uint8_t pos;
	uint8_t rpos;
};

void spo2_init(struct spo2 *spo2);
void spo2_add_sample(struct spo2* spo2, uint32_t red, uint32_t ir);
uint32_t spo2_get_percent(struct spo2* spo2, bool *valid);

#endif/*__SPO2_H__*/