#include "unity.h"
#include "generic_sensor_recal.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/* simple read_raw helper: context is pointer to uint32_t current sample */
static uint32_t fake_read_raw(void *context)
{
	return *((uint32_t *)context);
}

void test_recal_full_sequence_should_compute_expected_min_max_and_scale(void)
{
	Recal_Instance_t inst;
	Recal_Callbacks_t cb;
	Recal_Config_t cfg = { .sample_window = 4U, .min_offset = 8U, .max_offset = 8U };

	uint32_t raw_val = 100U; /* initial min samples */
	cb.read_raw = fake_read_raw;
	cb.nvm_write = NULL;
	cb.nvm_read = NULL;
	cb.context = &raw_val;

	Recal_Init(&inst, &cb, &cfg);

	/* start min capture */
	TEST_ASSERT_EQUAL(RECAL_OK, Recal_Start(&inst));

	/* feed sample_window samples of min */
	for (uint32_t i = 0; i < cfg.sample_window; ++i) {
		Recal_Tick(&inst, 1);
	}

	/* after sampling min we should be waiting for max capture */
	TEST_ASSERT_EQUAL(RECAL_STATE_WAIT_MAX_CAPTURE, Recal_GetState(&inst));

	/* trigger capture -> begin max sampling */
	TEST_ASSERT_EQUAL(RECAL_OK, Recal_Capture(&inst));

	/* now change raw to represent max range */
	raw_val = 4000U;

	/* feed sample_window samples of max */
	for (uint32_t i = 0; i < cfg.sample_window; ++i) {
		Recal_Tick(&inst, 1);
	}

	/* one more tick to run COMPUTE, and one to run STORE (which goes straight to COMPLETED when NVM callbacks are NULL) */
	Recal_Tick(&inst, 1);
	Recal_Tick(&inst, 1);

	TEST_ASSERT_EQUAL(RECAL_STATE_COMPLETED, Recal_GetState(&inst));

	const Recal_Data_t *d = Recal_GetData(&inst);
	TEST_ASSERT_NOT_NULL(d);

	/* offsets applied: adjusted_min = 100 + 8 = 108, adjusted_max = 4000 - 8 = 3992 */
	TEST_ASSERT_EQUAL_UINT32(108U, d->raw_min);
	TEST_ASSERT_EQUAL_UINT32(3992U, d->raw_max);

	/* scale should be 1.0f / (raw_max - raw_min) */
	float expected_scale = 1.0f / (float)(d->raw_max - d->raw_min);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, expected_scale, d->scale);

	/* offset should be -raw_min * scale */
	float expected_offset = -((float)d->raw_min) * expected_scale;
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, expected_offset, d->offset);
}

