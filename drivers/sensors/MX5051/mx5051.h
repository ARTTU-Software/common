#ifndef INC_MX5051_H
#define INC_MX5051_H

#include "generic_sensor.h"
#include <stdint.h>

#define MX5051_BUF_SIZE 32
// Removed MX5051_SAMPLE_GAP - we now dynamically process whatever arrives!

typedef struct {
    // --- Config ---
    uint32_t tim_clk_hz;      // Timer Freq (e.g., 100000)
    uint32_t pulses_per_rev;  // 25
    uint32_t timer_mask;      // 0xFFFF or 0xFFFFFFFF
    float    rpm_constant;    // Pre-calculated constant to save CPU cycles
    
    // --- State ---
    uint32_t dma_buffer[MX5051_BUF_SIZE]; 
    uint32_t read_idx;        // Replaces prev_head (Tracks where we last read from)
    uint32_t last_timestamp;  // Keeps track of the absolute last pulse time
    float rpm;                // Final Output
    
    float last_valid_rpm;     

    Generic_Sensor_t generic_sensor;

} MX5051_Typedef_t;

/**
 * @brief Initialize the sensor.
 * @param timer_is_32bit: 1 for TIM2/TIM5, 0 for TIM3/TIM4
 */
void MX5051_Init(MX5051_Typedef_t* s,
                 uint32_t tim_clk_hz,
                 uint32_t pulses_per_rev,
                 uint8_t timer_is_32bit,
                 uint8_t port,
                 Generic_Kalman_Settings_t k);

void MX5051_Update_RPM(MX5051_Typedef_t* s, uint32_t dma_items_left, uint32_t current_tim_cnt);

uint16_t MX5051_decode(uint8_t high_byte, uint8_t low_byte);

#endif