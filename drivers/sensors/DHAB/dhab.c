#include "dhab.h"
#include <math.h>
#include <stddef.h>

static float channel_range(dhab_model_t model, uint32_t channel)
{
    if (model == DHAB_S118) {
        return channel == DHAB_LOW ? 30.0f : 350.0f;
    }
    return channel == DHAB_LOW ? 50.0f : 200.0f;
}

static float channel_sensitivity(dhab_model_t model, uint32_t channel)
{
    if (model == DHAB_S118) {
        return channel == DHAB_LOW ? 0.0667f : 0.0057f;
    }
    return channel == DHAB_LOW ? 0.040f : 0.010f;
}

bool dhab_init(dhab_t *sensor, const dhab_config_t *config, uint32_t now_ms)
{
    if (sensor == NULL) {
        return false;
    }
    *sensor = (dhab_t){0};
    sensor->sample.range = DHAB_NONE;
    if (config == NULL || config->adc_full_scale == 0u
        || (config->model != DHAB_S118 && config->model != DHAB_S134)) {
        return false;
    }
    for (uint32_t i = 0u; i < 2u; ++i) {
        const dhab_channel_config_t *channel = &config->channel[i];
        if (!isfinite(channel->divider_gain) || channel->divider_gain < 1.0f
            || !isfinite(channel->diode_drop_v) || channel->diode_drop_v < 0.0f
            || !isfinite(channel->zero_offset_v)) {
            return false;
        }
    }
    sensor->config = *config;
    sensor->started_ms = now_ms;
    sensor->initialized = true;
    return true;
}

static void convert_channel(dhab_t *sensor, uint32_t index, uint32_t raw,
                            float supply_v, float adc_vref_v, uint32_t now_ms)
{
    dhab_channel_sample_t *sample = &sensor->sample.channel[index];
    const dhab_channel_config_t *config = &sensor->config.channel[index];
    sample->adc_raw = raw;
    if (raw > sensor->config.adc_full_scale || !isfinite(supply_v)
        || supply_v < 4.75f || supply_v > 5.25f
        || !isfinite(adc_vref_v) || adc_vref_v <= 0.0f) {
        sample->flags = DHAB_SAMPLE_INPUT;
        sensor->recovering[index] = true;
        sensor->last_overload_ms[index] = now_ms;
        return;
    }

    sample->adc_v = ((float)raw / (float)sensor->config.adc_full_scale) * adc_vref_v;
    sample->sensor_v = sample->adc_v * config->divider_gain + config->diode_drop_v;
    /* Normalize the measured output FIRST. Datasheet Uo=2.5 V and S are
     * specified at 5 V, while both physical offset and gain track supply. */
    const float normalized_v = sample->sensor_v * (5.0f / supply_v);
    sample->current_a = (normalized_v - 2.5f - config->zero_offset_v)
        / channel_sensitivity(sensor->config.model, index);
    if (!isfinite(sample->current_a)) {
        sample->flags |= DHAB_SAMPLE_INPUT;
    }
    if (raw == 0u || raw == sensor->config.adc_full_scale) {
        sample->flags |= DHAB_SAMPLE_ADC_RAIL;
    }
    /* Use the inner edges of the datasheet clamp tolerance bands. A clamp
     * voltage must not be mistaken for a precise over-range current. */
    if (normalized_v <= 0.3f || normalized_v >= 4.7f) {
        sample->flags |= DHAB_SAMPLE_CLAMP;
    }
    if (fabsf(sample->current_a) > channel_range(sensor->config.model, index)) {
        sample->flags |= DHAB_SAMPLE_RANGE;
    }
    if (sample->flags != DHAB_SAMPLE_OK) {
        sensor->recovering[index] = true;
        sensor->last_overload_ms[index] = now_ms;
    }
    if ((uint32_t)(now_ms - sensor->started_ms) < 1u
        || (sensor->recovering[index]
            && (uint32_t)(now_ms - sensor->last_overload_ms[index]) < 10u)) {
        sample->flags |= DHAB_SAMPLE_SETTLING;
    } else {
        sensor->recovering[index] = false;
    }
}

bool dhab_update(dhab_t *sensor, uint32_t low_raw, uint32_t high_raw,
                 float supply_v, float adc_vref_v, uint32_t now_ms)
{
    if (sensor == NULL) {
        return false;
    }
    const dhab_range_t previous_range = sensor->sample.range;
    sensor->sample = (dhab_sample_t){.timestamp_ms = now_ms, .range = DHAB_NONE};
    if (!sensor->initialized) {
        return false;
    }
    convert_channel(sensor, DHAB_LOW, low_raw, supply_v, adc_vref_v, now_ms);
    convert_channel(sensor, DHAB_HIGH, high_raw, supply_v, adc_vref_v, now_ms);

    if (sensor->sample.channel[DHAB_LOW].flags == DHAB_SAMPLE_OK
        && sensor->sample.channel[DHAB_HIGH].flags == DHAB_SAMPLE_OK
        && fabsf(sensor->sample.channel[DHAB_LOW].current_a
                 - sensor->sample.channel[DHAB_HIGH].current_a) > 5.0f) {
        /* With two plausible but inconsistent voltages we cannot identify
         * which channel failed. In particular, do not select a low channel
         * stuck at midscale and silently report zero during high current. */
        sensor->sample.channel[DHAB_LOW].flags |= DHAB_SAMPLE_DISAGREEMENT;
        sensor->sample.channel[DHAB_HIGH].flags |= DHAB_SAMPLE_DISAGREEMENT;
    }

    const dhab_channel_sample_t *low = &sensor->sample.channel[DHAB_LOW];
    const dhab_channel_sample_t *high = &sensor->sample.channel[DHAB_HIGH];
    const float threshold = channel_range(sensor->config.model, DHAB_LOW)
        * (previous_range == DHAB_HIGH ? 0.8f : 0.9f);
    /* Retain a working high channel before selecting the more sensitive one. */
    if (high->flags == DHAB_SAMPLE_OK) {
        sensor->sample.range = DHAB_HIGH;
        if (low->flags == DHAB_SAMPLE_OK && fabsf(low->current_a) < threshold) {
            sensor->sample.range = DHAB_LOW;
        }
        const float current_ma = sensor->sample.channel[sensor->sample.range].current_a * 1000.0f;
        sensor->sample.current_ma = (int32_t)roundf(current_ma);
        sensor->sample.valid = true;
    }
    return sensor->sample.valid;
}
