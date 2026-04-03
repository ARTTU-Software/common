#include "debit_2061.h"

void flow_sensor_init(FlowSensor_t* s, uint32_t pulses_per_litre, uint8_t port){
    s->pulses_per_litre = pulses_per_litre;
    s->last_count = 0;
    s->flow_ml_s = 0.0f;
    s->pulse_count = 0;
    s->port = port;
}

void debit_sensor_update(FlowSensor_t* s, uint32_t current_tick){
    uint32_t prev_count = s->last_count;
    uint32_t curr_count = s->pulse_count;
    s->last_count = curr_count;

    uint32_t delta_count = (curr_count >= prev_count) 
                           ? (curr_count - prev_count) 
                           : (0xFFFFFFFF - prev_count + curr_count + 1);

    if (delta_count == 0) {
        s->flow_ml_s = 0;
        return;
    }

    float dt_seconds = (current_tick - s->last_update_tick) / 1000.0f;
    s->last_update_tick = current_tick;

    float litres_per_s = (float)delta_count / (s->pulses_per_litre * dt_seconds);
    uint32_t ml_per_s = (uint32_t)(litres_per_s * 1000.0f); // convert to ml/s

    if (ml_per_s > 0xFFFF) ml_per_s = 0xFFFF; // clamp to uint16_t max
    s->flow_ml_s = (uint16_t)ml_per_s;
}

void flow_sensor_EXTI_handler(FlowSensor_t* s){
    s->pulse_count++; // increment on each rising edge
}

uint16_t debit_sensor_decode(uint8_t high_byte, uint8_t low_byte){
    uint16_t value = (high_byte << 8) | low_byte;
    return value;
}