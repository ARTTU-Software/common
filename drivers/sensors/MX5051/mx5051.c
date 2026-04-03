#include "mx5051.h"

void MX5051_Init(MX5051_Typedef_t* s,
                 uint32_t tim_clk_hz,
                 uint32_t pulses_per_rev,
                 uint8_t timer_is_32bit,
                 uint8_t port,
                 Generic_Kalman_Settings_t k) {
    
    s->tim_clk_hz = tim_clk_hz;
    s->pulses_per_rev = pulses_per_rev;
    
    // Handle 16-bit vs 32-bit rollover math automatically
    s->timer_mask = (timer_is_32bit) ? 0xFFFFFFFF : 0x0000FFFF;
    
    // Initialize new state variables
    s->read_idx = 0;
    s->last_timestamp = 0;
    s->rpm = 0.0f;
    s->last_valid_rpm = 0.0f;
    
    // Pre-calculate the heavy floating-point math
    // RPM Constant = (60 * TimerFreq) / PulsesPerRev
    s->rpm_constant = (60.0f * (float)tim_clk_hz) / (float)pulses_per_rev;

    // Zero buffer
    for(int i = 0; i < MX5051_BUF_SIZE; i++) s->dma_buffer[i] = 0;

    // Restore your generic sensor config
    s->generic_sensor.port = port;
    s->generic_sensor.kalman_settings = k;
}

void MX5051_Update_RPM(MX5051_Typedef_t* s, uint32_t dma_items_left, uint32_t current_tim_cnt) {
    
    // 1. Where is the DMA writing next?
    uint32_t write_idx = (MX5051_BUF_SIZE - dma_items_left) % MX5051_BUF_SIZE;
    
    // 2. How many new pulses arrived since we last checked?
    uint32_t new_pulses = (write_idx - s->read_idx + MX5051_BUF_SIZE) % MX5051_BUF_SIZE;

    // --- PART A: Calculate Speed from New Pulses ---
    if (new_pulses > 0) {
        
        // Find the timestamp of the absolute newest pulse
        uint32_t newest_idx = (write_idx - 1 + MX5051_BUF_SIZE) % MX5051_BUF_SIZE;
        uint32_t timestamp_new = s->dma_buffer[newest_idx];

        // Delta between the newest pulse now, and the newest pulse from the LAST batch
        uint32_t delta_ticks = (timestamp_new - s->last_timestamp) & s->timer_mask;

        // Valid Check: Delta must be reasonable. If it's massive, it's either an initial 
        // startup glitch or an invalid timer wrap. We only calculate if it's safe.
        if (delta_ticks > 0 && delta_ticks < (s->timer_mask / 2)) {
            s->last_valid_rpm = ((float)new_pulses * s->rpm_constant) / (float)delta_ticks;
        }
        
        // Always anchor the timestamp for the next cycle, regardless of validity
        s->last_timestamp = timestamp_new;
        s->read_idx = write_idx;
    }

    // --- PART B: Zero-Speed & Deceleration Timeout ---
    uint32_t time_since_last = (current_tim_cnt - s->last_timestamp) & s->timer_mask;

    // If a massive amount of time has passed (e.g., wheel physically stopped)
    if (time_since_last > (s->timer_mask / 2)) {
        s->rpm = 0.0f;
        s->last_valid_rpm = 0.0f; 
    } else {
        // BUG FIX: Default to current speed, NOT zero! 
        // A time_since_last of 0 means infinite possible speed.
        float max_possible_rpm = s->last_valid_rpm; 
        
        if (time_since_last > 0) {
            max_possible_rpm = s->rpm_constant / (float)time_since_last;
        }

        // Smoothly pull down RPM only if the physics of time explicitly demand it
        if (max_possible_rpm < s->last_valid_rpm) {
            s->rpm = max_possible_rpm;
        } else {
            s->rpm = s->last_valid_rpm;
        }
    }

    // Hard clamp for absolute zero to cut through floating point noise
    if (s->rpm < 1.0f) {
        s->rpm = 0.0f;
    }
}

uint16_t MX5051_decode(uint8_t high_byte, uint8_t low_byte) {
    return (uint16_t)((high_byte << 8) | low_byte);
}