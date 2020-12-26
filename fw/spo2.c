#include "spo2.h"
#include "debug.h"
#include <string.h>
#include <math.h>

/* Samples to average to filter AC and DC signals */
#define MAVG_SIZE (SPO2_SAMPLE_BUF_SIZE)

/* R*100 -> SpO2 correlation : -45.060*r^2 + 30.354*r + 94.845.
 * Table obtained from The Internet™.
 */
static uint8_t spo_r_tbl[] = {
	95, 95, 95, 96, 96, 96, 97, 97, 97, 97,
	97, 98, 98, 98, 98, 98, 99, 99, 99, 99,
	99, 99, 99, 99, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 99, 99, 99, 99, 99, 99,
	99, 99, 98, 98, 98, 98, 98, 98, 97, 97,
	97, 97, 96, 96, 96, 96, 95, 95, 95, 94,
	94, 94, 93, 93, 93, 92, 92, 92, 91, 91,
	90, 90, 89, 89, 89, 88, 88, 87, 87, 86,
	86, 85, 85, 84, 84, 83, 82, 82, 81, 81,
	80, 80, 79, 78, 78, 77, 76, 76, 75, 74,
	74, 73, 72, 72, 71, 70, 69, 69, 68, 67,
	66, 66, 65, 64, 63, 62, 62, 61, 60, 59,
	58, 57, 56, 56, 55, 54, 53, 52, 51, 50,
	49, 48, 47, 46, 45, 44, 43, 42, 41, 40,
	39, 38, 37, 36, 35, 34, 33, 31, 30, 29,
	28, 27, 26, 25, 23, 22, 21, 20, 19, 17,
	16, 15, 14, 12, 11, 10, 9, 7, 6, 5,
	3, 2, 1
};

void spo2_init(struct spo2 *spo2)
{
	memset(spo2, 0, sizeof(*spo2));
}

void spo2_add_sample(struct spo2 *spo2, uint32_t red, uint32_t ir)
{
	spo2->buf_red[spo2->pos] = red;
	spo2->buf_ir[spo2->pos] = ir;
	spo2->pos = (spo2->pos + 1) % SPO2_SAMPLE_BUF_SIZE;
}

/* Algorithm obtained from https://www.ti.com/lit/an/slaa655/slaa655.pdf
   Note that the above document is based on light absorption, but since
   MAX30102 is based on light reflection, the ratio calculation is inverted. */
uint32_t spo2_get_percent(struct spo2* spo2, bool *valid)
{
	int8_t pos;
	uint32_t mavg_red = 0, mavg_ir = 0;
	int32_t ac_red2_sum = 0, ac_ir2_sum = 0;
	float ac_red_rms, ac_ir_rms, r;
	float r_avg;
	uint32_t tbl_idx;
	uint32_t spo2_percent = 0;

	*valid = true;
	pos = spo2->pos - MAVG_SIZE;
	if (pos < 0)
		pos += SPO2_SAMPLE_BUF_SIZE;

	for (int8_t i = 0; i < MAVG_SIZE; i++) {
		int8_t idx = (pos + i) % SPO2_SAMPLE_BUF_SIZE;
		/* low values usually means the sensor is not attached to
		   live tissue */
		if (spo2->buf_red[i] < SPO2_MIN_SENSOR_VAL ||
		    spo2->buf_ir[i] < SPO2_MIN_SENSOR_VAL) {
			*valid = false;
			goto exit;
		}
		mavg_red += spo2->buf_red[idx];
		mavg_ir += spo2->buf_ir[idx];
	}
	mavg_red /= MAVG_SIZE;
	mavg_ir /= MAVG_SIZE;
	/* compute ac rms and store some red ac to get heart rate */
	for (int8_t i = 0; i < MAVG_SIZE; i++) {
		int8_t idx = (pos + i) % SPO2_SAMPLE_BUF_SIZE;
		int32_t val = spo2->buf_red[idx] - mavg_red;

		ac_red2_sum += (val * val);
		val = spo2->buf_ir[idx] - mavg_ir;
		ac_ir2_sum += (val * val);
	}

	/* compute ratio */
	ac_red_rms = sqrtf((float)ac_red2_sum / (float)MAVG_SIZE);
	ac_ir_rms = sqrtf((float)ac_ir2_sum / (float)MAVG_SIZE);
	r = (ac_ir_rms / (float)mavg_ir) / (ac_red_rms / (float)mavg_red);
	/* add ratio to fifo so we can average them all */
	spo2->r[spo2->rpos] = r;
	spo2->rpos = (spo2->rpos + 1) % SPO2_RATIO_BUF_SIZE;
	/* compute average ratio */
	r_avg = 0.0f;
	pos = spo2->rpos - SPO2_RATIO_BUF_SIZE;
	if (pos < 0)
		pos += SPO2_RATIO_BUF_SIZE;
	for (int8_t i = 0; i < SPO2_RATIO_BUF_SIZE; i++) {
		uint8_t idx = (pos + i) % SPO2_RATIO_BUF_SIZE;
		r_avg += spo2->r[idx];
	}
	r_avg /= (float)SPO2_RATIO_BUF_SIZE;
	/* obtain SpO2 from table */
	tbl_idx = (int)(r_avg * 100.0f);
	*valid = tbl_idx < sizeof(spo_r_tbl);
	if (*valid)
		spo2_percent = spo_r_tbl[tbl_idx];
exit:
	return spo2_percent;
}