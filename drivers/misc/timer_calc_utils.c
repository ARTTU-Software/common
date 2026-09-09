#include "timer_calc_utils.h"

#define MAX_16BIT_VAL 65535ULL

/* Helper function to determine prescaler and auto-reload values from target ticks */
static bool calc_from_total_ticks(uint64_t total_ticks, uint16_t *psc, uint16_t *arr)
{
    if (total_ticks == 0ULL || psc == NULL || arr == NULL) {
        return false;
    }

    // Direct 16-bit fit with zero prescaler for highest resolution
    if (total_ticks <= (MAX_16BIT_VAL + 1ULL)) {
        *psc = 0U;
        *arr = (uint16_t)(total_ticks - 1ULL);
        return true;
    }

    // Minimum PSC required to keep ARR <= 65535
    uint32_t psc_val = (uint32_t)(total_ticks / (MAX_16BIT_VAL + 1ULL));
    if ((total_ticks % (MAX_16BIT_VAL + 1ULL)) != 0ULL) {
        psc_val++;
    }

    if (psc_val > (uint32_t)(MAX_16BIT_VAL + 1ULL)) {
        return false; // Period exceeds 16-bit prescaler range
    }

    uint32_t arr_val = (uint32_t)(total_ticks / psc_val);
    *psc = (uint16_t)(psc_val - 1U);
    *arr = (uint16_t)(arr_val - 1U);
    return true;
}

bool timer_calculate_psc_arr(uint32_t period_ms, uint32_t bus_freq_mhz, uint16_t *psc, uint16_t *arr)
{
    if (period_ms == 0U || bus_freq_mhz == 0U || psc == NULL || arr == NULL) {
        return false;
    }
    uint64_t total_ticks = (uint64_t)period_ms * (uint64_t)bus_freq_mhz * 1000ULL;
    return calc_from_total_ticks(total_ticks, psc, arr);
}

bool timer_calculate_freq_psc_arr(uint32_t freq_hz, uint32_t bus_freq_mhz, uint16_t *psc, uint16_t *arr)
{
    if (freq_hz == 0U || bus_freq_mhz == 0U || psc == NULL || arr == NULL) {
        return false;
    }
    uint64_t total_ticks = ((uint64_t)bus_freq_mhz * 1000000ULL) / (uint64_t)freq_hz;
    return calc_from_total_ticks(total_ticks, psc, arr);
}

bool timer_calc_pwm(uint32_t freq_hz,
                    uint8_t duty_percent,
                    uint32_t bus_freq_mhz,
                    uint16_t *psc,
                    uint16_t *arr,
                    uint16_t *ccr)
{
    if (freq_hz == 0U || duty_percent > 100U ||
        psc == NULL || arr == NULL || ccr == NULL)
    {
        return false;
    }

    uint16_t local_psc = 0U;
    uint16_t local_arr = 0U;

    // Calculate directly from frequency in Hertz (supports frequencies > 1000 Hz)
    if (!timer_calculate_freq_psc_arr(freq_hz, bus_freq_mhz, &local_psc, &local_arr)) {
        return false;
    }

    // Compute CCR based on duty percentage
    uint16_t local_ccr = (uint16_t)(((uint32_t)local_arr * (uint32_t)duty_percent) / 100U);

    *psc = local_psc;
    *arr = local_arr;
    *ccr = local_ccr;
    return true;
}
