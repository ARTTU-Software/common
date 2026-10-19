#ifndef INC_TIMER_CALC_UTILS_H
#define INC_TIMER_CALC_UTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Calculates Prescaler (PSC) and Auto-Reload Register (ARR) from period in milliseconds.
 * @param period_ms Desired timer period in milliseconds.
 * @param bus_freq_mhz Clock frequency of the timer bus in MHz.
 * @param psc Output pointer for 16-bit prescaler register value.
 * @param arr Output pointer for 16-bit auto-reload register value.
 * @return true on success, false if parameters cannot fit in 16-bit registers.
 */
bool timer_calculate_psc_arr(uint32_t period_ms,
                            uint32_t bus_freq_mhz,
                            uint16_t *psc,
                            uint16_t *arr);

/**
 * @brief Calculates Prescaler (PSC) and Auto-Reload Register (ARR) from frequency in Hertz.
 * @param freq_hz Desired frequency in Hertz.
 * @param bus_freq_mhz Clock frequency of the timer bus in MHz.
 * @param psc Output pointer for 16-bit prescaler register value.
 * @param arr Output pointer for 16-bit auto-reload register value.
 * @return true on success, false if parameters cannot fit in 16-bit registers.
 */
bool timer_calculate_freq_psc_arr(uint32_t freq_hz,
                                 uint32_t bus_freq_mhz,
                                 uint16_t *psc,
                                 uint16_t *arr);

/**
 * @brief Computes 16-bit timer registers (PSC, ARR, and CCR) for a requested PWM frequency and duty.
 * @param freq_hz PWM frequency in Hertz.
 * @param duty_percent PWM duty cycle percentage (0 to 100).
 * @param bus_freq_mhz Clock frequency of the timer peripheral bus in MHz.
 * @param psc Output pointer for 16-bit prescaler register value.
 * @param arr Output pointer for 16-bit auto-reload register value.
 * @param ccr Output pointer for 16-bit capture/compare register value.
 * @return true on success, false if parameters exceed 16-bit limits or are invalid.
 */
bool timer_calc_pwm(uint32_t freq_hz,
                    uint8_t duty_percent,
                    uint32_t bus_freq_mhz,
                    uint16_t *psc,
                    uint16_t *arr,
                    uint16_t *ccr);

#endif /* INC_TIMER_CALC_UTILS_H */
