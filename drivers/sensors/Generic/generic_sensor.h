#ifndef INC_GENERIC_SENSOR_H
#define INC_GENERIC_SENSOR_H

#include "stdint.h"

typedef struct {
  float state[1];
  float covariance[1];
  float R[1];      // Measurement noise covariance
  float Ht[1];     // Transposed measurement matrix
  float chi2;
  // --- Add these for the prediction step ---
  float Phi[1];    // State transition matrix
  float G[1];      // Process noise distribution matrix
  float Q[1];      // Process noise covariance (tuning parameter)

  // --- Initialization flag ---
  uint8_t is_initialized;
} Generic_Kalman_Settings_t;

typedef struct {
  float raw_value;
  float filtered_value;
  int8_t kalman_status;    // Last Kalman filter status
  uint8_t port;            // Port on MCU
  
  // Kalman filter state
  Generic_Kalman_Settings_t kalman_settings;
} Generic_Sensor_t;

float generic_sensor_encode(const Generic_Sensor_t* sensor, float value);

#endif /* INC_GENERIC_SENSOR_H */
