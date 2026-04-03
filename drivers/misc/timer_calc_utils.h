#ifndef INC_TIMER_CALC_UTILS_H
#define INC_TIMER_CALC_UTILS_H

#include <stdint.h>
#include <stdbool.h>

bool timer_calc_pwm(uint32_t freq_hz,
                    uint8_t duty_percent,
                    uint32_t bus_freq_mhz,
                    uint16_t *psc,
                    uint16_t *arr,
                    uint16_t *ccr);

#endif /* INC_TIMER_CALC_UTILS_H */
