#include "timer_calc_utils.h"

// Define maximum values for 16-bit timers
#define MAX_16BIT_VAL 65535

bool timer_calculate_psc_arr(uint32_t period_ms, uint32_t bus_freq_mhz, uint16_t* psc, uint16_t* arr)
{
    if (psc == NULL || arr == NULL || period_ms == 0 || bus_freq_mhz == 0)
    {
        return false;
    }

    // Calculate the total number of timer ticks required for the period.
    // Use 64-bit integer to prevent overflow during calculation.
    // Ticks = Period (s) * Frequency (Hz)
    // Ticks = (period_ms / 1000) * (bus_freq_mhz * 1,000,000)
    // Ticks = period_ms * bus_freq_mhz * 1000
    uint64_t total_ticks = (uint64_t)period_ms * bus_freq_mhz * 1000;

    // If total_ticks fits within the ARR register, no prescaler is needed for the highest resolution.
    if (total_ticks <= (MAX_16BIT_VAL + 1)) {
        *psc = 0;
        *arr = (uint16_t)(total_ticks - 1);
        return true;
    }

    // To find a direct solution, we need to solve: (PSC+1) * (ARR+1) = total_ticks
    // We can't have two unknowns. So, we calculate the minimum PSC needed to make ARR fit.
    // The maximum value for ARR is MAX_16BIT_VAL, so ARR+1 is MAX_16BIT_VAL + 1.
    // min(PSC+1) = ceil(total_ticks / (MAX_16BIT_VAL + 1))
    
    uint32_t psc_val = (uint32_t)(total_ticks / (MAX_16BIT_VAL + 1));
    // If there's a remainder, we need to increment the prescaler to ensure ARR fits.
    if (total_ticks % (MAX_16BIT_VAL + 1) != 0) {
        psc_val++;
    }

    // Check if the required prescaler is too large
    if (psc_val > (MAX_16BIT_VAL + 1)) {
        return false; // Period is too long to achieve with 16-bit registers
    }

    // Recalculate ARR with the chosen PSC
    uint32_t arr_val = (uint32_t)(total_ticks / psc_val);

    *psc = psc_val - 1;
    *arr = arr_val - 1;

    return true;
}


bool timer_calc_pwm(uint32_t freq_hz,
                    uint8_t duty_percent,
                    uint32_t bus_freq_mhz,
                    uint16_t *psc,
                    uint16_t *arr,
                    uint16_t *ccr)
{
    if (freq_hz == 0 || duty_percent > 100 ||
        psc == NULL || arr == NULL || ccr == NULL)
    {
        return false;
    }

    // Period_ms = 1000 / frequency
    uint32_t period_ms = 1000UL / freq_hz;

    uint16_t local_psc, local_arr;

    // Use your existing prescaler/ARR calculator
    if (!timer_calculate_psc_arr(period_ms, bus_freq_mhz, &local_psc, &local_arr))
        return false;

    // Compute CCR based on duty
    uint16_t local_ccr = (uint32_t)local_arr * duty_percent / 100;

    // Output results
    *psc = local_psc;
    *arr = local_arr;
    *ccr = local_ccr;

    return true;
}
