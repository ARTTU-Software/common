#ifndef INC_DEBIT_2061_H
#define INC_DEBIT_2061_H

#include <stdint.h>
#include "generic_linear_sensor.h"

typedef struct {
    float pulses_per_litre;
    uint16_t flow_ml_s;     // output in ml/s
    uint16_t last_count; // count measured in one interval
    uint8_t port;
    uint32_t pulse_count;
    uint32_t last_update_tick;
    Generic_Sensor_t generic_sensor;
} FlowSensor_t;

void flow_sensor_init(FlowSensor_t* s, uint32_t pulses_per_litre, uint8_t port);

void debit_sensor_update(FlowSensor_t* s, uint32_t current_tick);

void flow_sensor_EXTI_handler(FlowSensor_t* s);

uint16_t debit_sensor_decode(uint8_t high_byte, uint8_t low_byte);

#endif /* INC_DEBIT_2061_H */
