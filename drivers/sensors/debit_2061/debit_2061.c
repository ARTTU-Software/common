#include "debit_2061.h"

void flow_sensor_init(FlowSensor_t* s, uint32_t pulses_per_litre, uint8_t port){
    if (s == 0) {
        return;
    }
    s->pulses_per_litre = (pulses_per_litre > 0U) ? (float)pulses_per_litre : 219.0f;
    s->last_count = 0U;
    s->flow_ml_s = 0U;
    s->pulse_count = 0U;
    s->last_update_tick = 0U;
    s->port = port;
}

void debit_sensor_update(FlowSensor_t* s, uint32_t current_tick){
    if (s == 0) {
        return;
    }

    if (s->last_update_tick == 0U) {
        s->last_update_tick = current_tick;
        s->last_count = s->pulse_count;
        s->flow_ml_s = 0U;
        return;
    }

    uint32_t dt_ms = current_tick - s->last_update_tick;
    if (dt_ms == 0U) {
        return;
    }
    s->last_update_tick = current_tick;

    uint32_t curr_count = s->pulse_count;
    uint32_t prev_count = s->last_count;
    s->last_count = curr_count;

    uint32_t delta_count = (curr_count >= prev_count) 
                           ? (curr_count - prev_count) 
                           : (0xFFFFFFFFU - prev_count + curr_count + 1U);

    if (delta_count == 0U || s->pulses_per_litre <= 0.0f) {
        s->flow_ml_s = 0U;
        return;
    }

    float dt_seconds = ((float)dt_ms) / 1000.0f;
    float litres_per_s = ((float)delta_count) / (s->pulses_per_litre * dt_seconds);
    uint32_t ml_per_s = (uint32_t)(litres_per_s * 1000.0f + 0.5f);

    if (ml_per_s > 0xFFFFU) ml_per_s = 0xFFFFU;
    s->flow_ml_s = (uint16_t)ml_per_s;
}

void flow_sensor_EXTI_handler(FlowSensor_t* s){
    if (s != 0) {
        s->pulse_count++;
    }
}

uint16_t debit_sensor_decode(uint8_t high_byte, uint8_t low_byte){
    uint16_t value = (high_byte << 8) | low_byte;
    return value;
}