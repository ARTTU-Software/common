#ifndef INC_CAN_SIGNAL_DEFS_H
#define INC_CAN_SIGNAL_DEFS_H

#include <stdint.h>

/* Generated from MAIN_DBC.dbc. Do not edit by hand. */

#define CAN_MSG_GF_WHEEL_SENSORS_ID 160U
#define CAN_MSG_GF_WHEEL_SENSORS_DLC 8U
#define CAN_MSG_GF_WHEEL_SENSORS_CYCLE_TIME_MS 0U

#define CAN_MSG_GF_WHEEL_SENSORS_SIG_WHEEL_SPEED_FR_FACTOR 1.0f

#define CAN_MSG_GF_WHEEL_SENSORS_SIG_WHEEL_SPEED_FL_FACTOR 1.0f

#define CAN_MSG_GF_WHEEL_SENSORS_SIG_SUSP_TRAVEL_FR_FACTOR 0.001f

#define CAN_MSG_GF_WHEEL_SENSORS_SIG_SUSP_TRAVEL_FL_FACTOR 0.001f

typedef struct {
    uint16_t Wheel_Speed_FR; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="rpm" */
    uint16_t Wheel_Speed_FL; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="rpm" */
    float Susp_Travel_FR; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="mm" */
    float Susp_Travel_FL; /* start_bit=55; length=16; factor=0.001; offset=0.0; unit="mm" */
} can_msg_GF_Wheel_Sensors_t;

#define CAN_MSG_GF_MISC_SENSORS_ID 161U
#define CAN_MSG_GF_MISC_SENSORS_DLC 8U
#define CAN_MSG_GF_MISC_SENSORS_CYCLE_TIME_MS 0U

#define CAN_MSG_GF_MISC_SENSORS_SIG_APPS1_FACTOR 1.0f

#define CAN_MSG_GF_MISC_SENSORS_SIG_APPS2_FACTOR 1.0f

#define CAN_MSG_GF_MISC_SENSORS_SIG_STEERING_FACTOR 0.01f

#define CAN_MSG_GF_MISC_SENSORS_SIG_R2D_BTN_FACTOR 1.0f

typedef struct {
    uint8_t APPS1; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="%" */
    uint8_t APPS2; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="%" */
    float Steering; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="deg" */
    uint8_t R2D_BTN; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_GF_Misc_Sensors_t;

#define CAN_MSG_GF_SOFTWARE_STATUS_ID 175U
#define CAN_MSG_GF_SOFTWARE_STATUS_DLC 8U
#define CAN_MSG_GF_SOFTWARE_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_GF_SOFTWARE_STATUS_SIG_GF_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_GF_SOFTWARE_STATUS_SIG_GF_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_GF_SOFTWARE_STATUS_SIG_GF_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_GF_SOFTWARE_STATUS_SIG_GF_APPS_RECAL_STEP_FACTOR 1.0f

#define CAN_MSG_GF_SOFTWARE_STATUS_SIG_GF_BOARD_IS_STARTING_FACTOR 1.0f

#define CAN_MSG_GF_SOFTWARE_STATUS_SIG_GF_ADC_READY_FACTOR 1.0f

typedef struct {
    uint8_t GF_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GF_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GF_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GF_APPS_Recal_Step; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GF_Board_Is_Starting; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t GF_ADC_Ready; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_GF_Software_Status_t;

#define CAN_MSG_GF_CALIBRATION_VALUES_ID 174U
#define CAN_MSG_GF_CALIBRATION_VALUES_DLC 8U
#define CAN_MSG_GF_CALIBRATION_VALUES_CYCLE_TIME_MS 0U

#define CAN_MSG_GF_CALIBRATION_VALUES_SIG_GF_APPS_MIN_FACTOR 1.0f

#define CAN_MSG_GF_CALIBRATION_VALUES_SIG_GF_APPS_MAX_FACTOR 1.0f

#define CAN_MSG_GF_CALIBRATION_VALUES_SIG_GF_STEER_MIN_FACTOR 1.0f

#define CAN_MSG_GF_CALIBRATION_VALUES_SIG_GF_STEER_MAX_FACTOR 1.0f

typedef struct {
    uint16_t GF_APPS_Min; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="dec" */
    uint16_t GF_APPS_Max; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="dec" */
    uint16_t GF_Steer_Min; /* start_bit=39; length=16; factor=1.0; offset=0.0; unit="dec" */
    uint16_t GF_Steer_Max; /* start_bit=55; length=16; factor=1.0; offset=0.0; unit="dec" */
} can_msg_GF_Calibration_Values_t;

#define CAN_MSG_GR_WHEEL_SENSORS_ID 176U
#define CAN_MSG_GR_WHEEL_SENSORS_DLC 8U
#define CAN_MSG_GR_WHEEL_SENSORS_CYCLE_TIME_MS 0U

#define CAN_MSG_GR_WHEEL_SENSORS_SIG_WHEEL_SPEED_RR_FACTOR 1.0f

#define CAN_MSG_GR_WHEEL_SENSORS_SIG_WHEEL_SPEED_RL_FACTOR 1.0f

#define CAN_MSG_GR_WHEEL_SENSORS_SIG_SUSP_TRAVEL_RR_FACTOR 0.001f

#define CAN_MSG_GR_WHEEL_SENSORS_SIG_SUSP_TRAVEL_RL_FACTOR 0.001f

typedef struct {
    uint16_t Wheel_Speed_RR; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="rpm" */
    uint16_t Wheel_Speed_RL; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="rpm" */
    float Susp_Travel_RR; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="mm" */
    float Susp_Travel_RL; /* start_bit=55; length=16; factor=0.001; offset=0.0; unit="mm" */
} can_msg_GR_Wheel_Sensors_t;

#define CAN_MSG_GR_MISC_SENSORS_ID 177U
#define CAN_MSG_GR_MISC_SENSORS_DLC 8U
#define CAN_MSG_GR_MISC_SENSORS_CYCLE_TIME_MS 0U

#define CAN_MSG_GR_MISC_SENSORS_SIG_BRAKE_FACTOR 0.01f

#define CAN_MSG_GR_MISC_SENSORS_SIG_JB_50A_FACTOR 0.01f

#define CAN_MSG_GR_MISC_SENSORS_SIG_JB_200A_FACTOR 0.01f

#define CAN_MSG_GR_MISC_SENSORS_SIG_SDC_FACTOR 1.0f

typedef struct {
    float Brake; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="bar" */
    float JB_50A; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="A" */
    float JB_200A; /* start_bit=39; length=16; factor=0.01; offset=0.0; unit="A" */
    uint8_t SDC; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="Bool" */
} can_msg_GR_Misc_Sensors_t;

#define CAN_MSG_GR_LIQUID_SENSORS_ID 178U
#define CAN_MSG_GR_LIQUID_SENSORS_DLC 8U
#define CAN_MSG_GR_LIQUID_SENSORS_CYCLE_TIME_MS 0U

#define CAN_MSG_GR_LIQUID_SENSORS_SIG_FLOW_SENSOR_FACTOR 1.0f

#define CAN_MSG_GR_LIQUID_SENSORS_SIG_TEMP_1_FACTOR 0.001f

#define CAN_MSG_GR_LIQUID_SENSORS_SIG_TEMP_2_FACTOR 0.001f

typedef struct {
    uint16_t Flow_Sensor; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="L/m" */
    float Temp_1; /* start_bit=23; length=16; factor=0.001; offset=0.0; unit="degC" */
    float Temp_2; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="degC" */
} can_msg_GR_Liquid_Sensors_t;

#define CAN_MSG_GR_SOFTWARE_STATUS_ID 191U
#define CAN_MSG_GR_SOFTWARE_STATUS_DLC 8U
#define CAN_MSG_GR_SOFTWARE_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_GR_SOFTWARE_STATUS_SIG_GR_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_GR_SOFTWARE_STATUS_SIG_GR_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_GR_SOFTWARE_STATUS_SIG_GR_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_GR_SOFTWARE_STATUS_SIG_GR_APPS_RECAL_STEP_FACTOR 1.0f

#define CAN_MSG_GR_SOFTWARE_STATUS_SIG_GR_BOARD_IS_STARTING_FACTOR 1.0f

#define CAN_MSG_GR_SOFTWARE_STATUS_SIG_GR_ADC_READY_FACTOR 1.0f

typedef struct {
    uint8_t GR_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GR_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GR_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GR_APPS_Recal_Step; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GR_Board_Is_Starting; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t GR_ADC_Ready; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_GR_Software_Status_t;

#define CAN_MSG_ECU_INVERTER_SIG_ID 224U
#define CAN_MSG_ECU_INVERTER_SIG_DLC 8U
#define CAN_MSG_ECU_INVERTER_SIG_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_INVERTER_SIG_SIG_MOTOR_EN_DIS_REGEN_FACTOR 1.0f

#define CAN_MSG_ECU_INVERTER_SIG_SIG_INVERTER_L_REGEN_PERCENTAGE_FACTOR 1.0f

#define CAN_MSG_ECU_INVERTER_SIG_SIG_INVERTER_R_REGEN_PERCENTAGE_FACTOR 1.0f

#define CAN_MSG_ECU_INVERTER_SIG_SIG_INVERTER_L_SPEED_PERCENTAGE_FACTOR 1.0f

#define CAN_MSG_ECU_INVERTER_SIG_SIG_INVERTER_R_SPEED_PERCENTAGE_FACTOR 1.0f

#define CAN_MSG_ECU_INVERTER_SIG_SIG_CALCULATED_SPEED_FACTOR 0.01f

typedef struct {
    uint8_t Motor_EN_DIS_Regen; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Inverter_L_Regen_Percentage; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="%" */
    uint8_t Inverter_R_Regen_Percentage; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="%" */
    uint8_t Inverter_L_Speed_Percentage; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="%" */
    uint8_t Inverter_R_Speed_Percentage; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="%" */
    float Calculated_Speed; /* start_bit=47; length=16; factor=0.01; offset=0.0; unit="km/h" */
} can_msg_ECU_Inverter_Sig_t;

#define CAN_MSG_ECU_REAR_NMOS_ID 179U
#define CAN_MSG_ECU_REAR_NMOS_DLC 8U
#define CAN_MSG_ECU_REAR_NMOS_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_REAR_NMOS_SIG_R2D_BUZZER_ON_OFF_FACTOR 1.0f

typedef struct {
    uint8_t R2D_Buzzer_On_Off; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_ECU_Rear_NMOS_t;

#define CAN_MSG_ECU_FRONT_NMOS_ID 162U
#define CAN_MSG_ECU_FRONT_NMOS_DLC 8U
#define CAN_MSG_ECU_FRONT_NMOS_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_FRONT_NMOS_SIG_R2D_LED_DELAY_FACTOR 1.0f

typedef struct {
    uint8_t R2D_LED_Delay; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="Hz" */
} can_msg_ECU_Front_NMOS_t;

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_ID 229U
#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_DLC 8U
#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_SIG_ECU_ECU_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_SIG_ECU_ECU_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_SIG_ECU_ECU_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_SIG_ECU_AMS_ERROR_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_SIG_ECU_SDC_ERROR_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_MAIN_SIG_ECU_IS_BOOTING_UP_FACTOR 1.0f

typedef struct {
    uint8_t ECU_ECU_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_ECU_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_ECU_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_AMS_Error; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_SDC_Error; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_Is_Booting_Up; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_ECU_Software_Status_Main_t;

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_ID 230U
#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_DLC 8U
#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_R2D_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_R2D_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_R2D_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_PRECHARGE_DONE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_R2D_BUTTON_PRESSED_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_R2D_BRAKE_OK_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_PRECHARGE_TIMED_OUT_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_R2D_SIG_ECU_R2D_BUZZER_ELAPSED_FACTOR 1.0f

typedef struct {
    uint8_t ECU_R2D_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_R2D_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_R2D_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_Precharge_Done; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_R2D_Button_Pressed; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_R2D_Brake_Ok; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_Precharge_Timed_Out; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_R2D_Buzzer_Elapsed; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_ECU_Software_Status_R2D_t;

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_ID 231U
#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_DLC 8U
#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_CTRL_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_CTRL_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_CTRL_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_APPS_ERROR_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_APPS_PERCENTAGE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_CTRL_STOP_REQUEST_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_STATUS_CTRL_SIG_ECU_COOLING_RECALC_REQUESTED_FACTOR 1.0f

typedef struct {
    uint8_t ECU_CTRL_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_CTRL_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_CTRL_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t ECU_APPS_Error; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_APPS_Percentage; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_CTRL_Stop_Request; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_Cooling_Recalc_Requested; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_ECU_Software_Status_CTRL_t;

#define CAN_MSG_ECU_UKF_OUTPUT_1_ID 232U
#define CAN_MSG_ECU_UKF_OUTPUT_1_DLC 8U
#define CAN_MSG_ECU_UKF_OUTPUT_1_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_UKF_OUTPUT_1_SIG_ECU_UKF_VELOCITY_X_FACTOR 0.01f

#define CAN_MSG_ECU_UKF_OUTPUT_1_SIG_ECU_UKF_VELOCITY_Y_FACTOR 0.001f

#define CAN_MSG_ECU_UKF_OUTPUT_1_SIG_ECU_UKF_ROTATION_Z_FACTOR 0.01f

#define CAN_MSG_ECU_UKF_OUTPUT_1_SIG_ECU_UKF_ANGULAR_VELOCITY_Z_FACTOR 0.01f

typedef struct {
    float ECU_UKF_Velocity_X; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="km/h" */
    float ECU_UKF_Velocity_Y; /* start_bit=23; length=16; factor=0.001; offset=0.0; unit="m/s" */
    float ECU_UKF_Rotation_Z; /* start_bit=39; length=16; factor=0.01; offset=0.0; unit="rad" */
    float ECU_UKF_Angular_Velocity_Z; /* start_bit=55; length=16; factor=0.01; offset=0.0; unit="rad" */
} can_msg_ECU_UKF_Output_1_t;

#define CAN_MSG_ECU_UKF_OUTPUT_2_ID 233U
#define CAN_MSG_ECU_UKF_OUTPUT_2_DLC 8U
#define CAN_MSG_ECU_UKF_OUTPUT_2_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_UKF_OUTPUT_2_SIG_ECU_UKF_ACCEL_X_FACTOR 0.01f

#define CAN_MSG_ECU_UKF_OUTPUT_2_SIG_ECU_UKF_ACCEL_Y_FACTOR 0.01f

typedef struct {
    float ECU_UKF_Accel_X; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="m/s^2" */
    float ECU_UKF_Accel_Y; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="m/s^2" */
} can_msg_ECU_UKF_Output_2_t;

#define CAN_MSG_BMS_ERRORS_TEMPS_ID 16U
#define CAN_MSG_BMS_ERRORS_TEMPS_DLC 8U
#define CAN_MSG_BMS_ERRORS_TEMPS_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_INTERNAL_TEMPERATURE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0AFA_LOW_CELL_VOLTAGE_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A04_CELL_OPEN_WIRING_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A0F_CELL_BANK_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_U0100_CAN_COMMUNICATION_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A10_PACK_TOO_HOT_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A0B_INTERNAL_LOGIC_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A12_CELL_BALANCING_STUCK_OFF_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A0E_LOWEST_CELL_VOLTAGE_TOO_LOW_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_MULTIPURPOSE_OUTPUT_3_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_MULTIPURPOSE_ENABLE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_MULTIPURPOSE_OUTPUT_4_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_BALANCING_ACTIVE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_CHARGE_POWER_SIGNAL_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_CHARGE_MODE_ACTIVE_CAN_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_VOLTAGE_FAILSAFE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_CURRENT_FAILSAFE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A0C_HIGHEST_CELL_VOLTAGE_TOO_HIGH_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A9C_THERMISTOR_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0AC0_CURRENT_SENSOR_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A04_CELL_OPEN_WIRING_FAULT_1_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A80_WEAK_CELL_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_MULTIPURPOSE_OUTPUT_2_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_LOW_TEMPERATURE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_HIGH_TEMPERATURE_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_HIGH_THERMISTOR_ID_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_LOW_THERMISTOR_ID_FACTOR 1.0f

typedef struct {
    int8_t Internal_Temperature; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="Celsius" */
    uint8_t DTC_P0AFA_Low_Cell_Voltage_Fault; /* start_bit=8; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A04_Cell_Open_Wiring_Fault; /* start_bit=9; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A0F_Cell_Bank_Fault; /* start_bit=10; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_U0100_CAN_Communication_Fault; /* start_bit=11; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A10_Pack_Too_Hot_Fault; /* start_bit=12; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A0B_Internal_Logic_Fault; /* start_bit=13; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A12_Cell_Balancing_Stuck_Off_Fault; /* start_bit=14; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A0E_Lowest_Cell_Voltage_Too_Low_Fault; /* start_bit=15; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t MultiPurpose_Output_3; /* start_bit=16; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t MultiPurpose_Enable; /* start_bit=17; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t MultiPurpose_Output_4; /* start_bit=18; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Balancing_Active; /* start_bit=19; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Charge_Power_Signal; /* start_bit=20; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Charge_Mode_Active_CAN; /* start_bit=21; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Voltage_Failsafe; /* start_bit=22; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Current_Failsafe; /* start_bit=23; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A0C_Highest_Cell_Voltage_Too_High_Fault; /* start_bit=24; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A9C_Thermistor_Fault; /* start_bit=25; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0AC0_Current_Sensor_Fault; /* start_bit=26; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A04_Cell_Open_Wiring_Fault_1; /* start_bit=27; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A80_Weak_Cell_Fault; /* start_bit=28; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t MultiPurpose_Output_2; /* start_bit=29; length=1; factor=1.0; offset=0.0; unit="Bool" */
    int8_t Low_Temperature; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="Celsius" */
    int8_t High_Temperature; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="Celsius" */
    uint8_t High_Thermistor_ID; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t Low_Thermistor_ID; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Errors_Temps_t;

#define CAN_MSG_BMS_SOC_VOLTAGE_ID 17U
#define CAN_MSG_BMS_SOC_VOLTAGE_DLC 8U
#define CAN_MSG_BMS_SOC_VOLTAGE_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_OPEN_VOLTAGE_FACTOR 0.1f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_INST_VOLTAGE_FACTOR 0.1f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_HEALTH_FACTOR 1.0f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_SIMULATED_SOC_FACTOR 0.5f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_SOC_FACTOR 0.5f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_ADAPTIVE_SOC_FACTOR 0.5f

typedef struct {
    float Pack_Open_Voltage; /* start_bit=7; length=16; factor=0.1; offset=0.0; unit="Volts" */
    float Pack_Inst_Voltage; /* start_bit=23; length=16; factor=0.1; offset=0.0; unit="Volts" */
    uint8_t Pack_Health; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="Percent" */
    float Simulated_SOC; /* start_bit=47; length=8; factor=0.5; offset=0.0; unit="Percent" */
    float Pack_SOC; /* start_bit=55; length=8; factor=0.5; offset=0.0; unit="Percent" */
    float Adaptive_SOC; /* start_bit=63; length=8; factor=0.5; offset=0.0; unit="Percent" */
} can_msg_BMS_SoC_Voltage_t;

#define CAN_MSG_BMS_CELL_OPEN_ID 18U
#define CAN_MSG_BMS_CELL_OPEN_DLC 8U
#define CAN_MSG_BMS_CELL_OPEN_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CELL_OPEN_SIG_LOW_OPENCELL_VOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_CELL_OPEN_SIG_HIGH_OPENCELL_VOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_CELL_OPEN_SIG_AVG_OPENCELL_VOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_CELL_OPEN_SIG_LOW_OPENCELL_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_OPEN_SIG_HIGH_OPENCELL_ID_FACTOR 1.0f

typedef struct {
    float Low_Opencell_Voltage; /* start_bit=7; length=16; factor=0.0001; offset=0.0; unit="Volts" */
    float High_Opencell_Voltage; /* start_bit=23; length=16; factor=0.0001; offset=0.0; unit="Volts" */
    float Avg_Opencell_Voltage; /* start_bit=39; length=16; factor=0.0001; offset=0.0; unit="Volts" */
    uint8_t Low_Opencell_ID; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t High_Opencell_ID; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Cell_Open_t;

#define CAN_MSG_BMS_CELL_VOLTAGE_ID 19U
#define CAN_MSG_BMS_CELL_VOLTAGE_DLC 8U
#define CAN_MSG_BMS_CELL_VOLTAGE_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CELL_VOLTAGE_SIG_LOW_CELL_VOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_CELL_VOLTAGE_SIG_HIGH_CELL_VOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_CELL_VOLTAGE_SIG_AVG_CELL_VOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_CELL_VOLTAGE_SIG_LOW_CELL_VOLTAGE_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_VOLTAGE_SIG_HIGH_CELL_VOLTAGE_ID_FACTOR 1.0f

typedef struct {
    float Low_Cell_Voltage; /* start_bit=7; length=16; factor=0.0001; offset=0.0; unit="Volts" */
    float High_Cell_Voltage; /* start_bit=23; length=16; factor=0.0001; offset=0.0; unit="Volts" */
    float Avg_Cell_Voltage; /* start_bit=39; length=16; factor=0.0001; offset=0.0; unit="Volts" */
    uint8_t Low_Cell_Voltage_ID; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t High_Cell_Voltage_ID; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Cell_Voltage_t;

#define CAN_MSG_BMS_CELL_RESISTANCE_ID 20U
#define CAN_MSG_BMS_CELL_RESISTANCE_DLC 8U
#define CAN_MSG_BMS_CELL_RESISTANCE_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CELL_RESISTANCE_SIG_LOW_CELL_RESISTANCE_FACTOR 0.01f

#define CAN_MSG_BMS_CELL_RESISTANCE_SIG_HIGH_CELL_RESISTANCE_FACTOR 0.01f

#define CAN_MSG_BMS_CELL_RESISTANCE_SIG_AVG_CELL_RESISTANCE_FACTOR 0.01f

#define CAN_MSG_BMS_CELL_RESISTANCE_SIG_HIGH_INTRES_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_RESISTANCE_SIG_LOW_INTRES_ID_FACTOR 1.0f

typedef struct {
    float Low_Cell_Resistance; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="mOhm" */
    float High_Cell_Resistance; /* start_bit=39; length=16; factor=0.01; offset=0.0; unit="mOhm" */
    float Avg_Cell_Resistance; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="mOhm" */
    uint8_t High_Intres_ID; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t Low_Intres_ID; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Cell_Resistance_t;

#define CAN_MSG_BMS_CURRENT_ID 21U
#define CAN_MSG_BMS_CURRENT_DLC 8U
#define CAN_MSG_BMS_CURRENT_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CURRENT_SIG_AVERAGE_CURRENT_FACTOR 0.1f

#define CAN_MSG_BMS_CURRENT_SIG_PACK_CURRENT_FACTOR 0.1f

#define CAN_MSG_BMS_CURRENT_SIG_PACK_DOD_FACTOR 0.5f

#define CAN_MSG_BMS_CURRENT_SIG_PACK_DCL_FACTOR 1.0f

#define CAN_MSG_BMS_CURRENT_SIG_CHARGER_SAFETY_FACTOR 1.0f

#define CAN_MSG_BMS_CURRENT_SIG_CHARGE_RELAY_FACTOR 1.0f

typedef struct {
    float Average_Current; /* start_bit=7; length=16; factor=0.1; offset=0.0; unit="Amps" */
    float Pack_Current; /* start_bit=23; length=16; factor=0.1; offset=0.0; unit="Amps" */
    float Pack_DOD; /* start_bit=39; length=8; factor=0.5; offset=0.0; unit="Percent" */
    uint16_t Pack_DCL; /* start_bit=47; length=16; factor=1.0; offset=0.0; unit="Amps" */
    uint8_t Charger_Safety; /* start_bit=55; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Charge_Relay; /* start_bit=56; length=1; factor=1.0; offset=0.0; unit="Bool" */
} can_msg_BMS_Current_t;

#define CAN_MSG_GIL_INV_DATA_1_ID 32U
#define CAN_MSG_GIL_INV_DATA_1_DLC 8U
#define CAN_MSG_GIL_INV_DATA_1_CYCLE_TIME_MS 0U

#define CAN_MSG_GIL_INV_DATA_1_SIG_PWM_L_FACTOR 1.0f

#define CAN_MSG_GIL_INV_DATA_1_SIG_INPUT_VOLTAGE_L_FACTOR 0.1f

#define CAN_MSG_GIL_INV_DATA_1_SIG_PHASE_CURRENT_L_FACTOR 0.1f

#define CAN_MSG_GIL_INV_DATA_1_SIG_RPM_L_FACTOR 1.0f

typedef struct {
    uint16_t PWM_L; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="raw" */
    float Input_Voltage_L; /* start_bit=23; length=16; factor=0.1; offset=0.0; unit="V" */
    float Phase_Current_L; /* start_bit=39; length=16; factor=0.1; offset=0.0; unit="A" */
    uint16_t RPM_L; /* start_bit=55; length=16; factor=1.0; offset=0.0; unit="rpm" */
} can_msg_GIL_Inv_Data_1_t;

#define CAN_MSG_GIL_INV_DATA_2_ID 33U
#define CAN_MSG_GIL_INV_DATA_2_DLC 8U
#define CAN_MSG_GIL_INV_DATA_2_CYCLE_TIME_MS 0U

#define CAN_MSG_GIL_INV_DATA_2_SIG_MOTOR_TEMP_L_FACTOR 1.0f

#define CAN_MSG_GIL_INV_DATA_2_SIG_INVERTER_TEMP_L_FACTOR 1.0f

#define CAN_MSG_GIL_INV_DATA_2_SIG_AUX_BRAKE_INPUT_L_FACTOR 0.001f

#define CAN_MSG_GIL_INV_DATA_2_SIG_THROTTLE_INPUT_L_FACTOR 0.001f

typedef struct {
    int16_t Motor_Temp_L; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="degC" */
    int16_t Inverter_Temp_L; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="degC" */
    float Aux_Brake_Input_L; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="V" */
    float Throttle_Input_L; /* start_bit=55; length=16; factor=0.001; offset=0.0; unit="V" */
} can_msg_GIL_Inv_Data_2_t;

#define CAN_MSG_GIL_SOFTWARE_STATUS_ID 34U
#define CAN_MSG_GIL_SOFTWARE_STATUS_DLC 8U
#define CAN_MSG_GIL_SOFTWARE_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_FAULT_LATCHED_FACTOR 1.0f

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_CAN_DELTA_MS_FACTOR 1.0f

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_PASS_THROUGH_FLAG_FACTOR 1.0f

#define CAN_MSG_GIL_SOFTWARE_STATUS_SIG_GIL_BOARD_IS_STARTING_FACTOR 1.0f

typedef struct {
    uint8_t GIL_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIL_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIL_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIL_Fault_Latched; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint16_t GIL_CAN_Delta_MS; /* start_bit=39; length=16; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIL_Pass_Through_Flag; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t GIL_Board_Is_Starting; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_GIL_Software_Status_t;

#define CAN_MSG_GIR_INV_DATA_1_ID 48U
#define CAN_MSG_GIR_INV_DATA_1_DLC 8U
#define CAN_MSG_GIR_INV_DATA_1_CYCLE_TIME_MS 0U

#define CAN_MSG_GIR_INV_DATA_1_SIG_PWM_R_FACTOR 1.0f

#define CAN_MSG_GIR_INV_DATA_1_SIG_INPUT_VOLTAGE_R_FACTOR 0.1f

#define CAN_MSG_GIR_INV_DATA_1_SIG_PHASE_CURRENT_R_FACTOR 0.1f

#define CAN_MSG_GIR_INV_DATA_1_SIG_RPM_R_FACTOR 1.0f

typedef struct {
    uint16_t PWM_R; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="raw" */
    float Input_Voltage_R; /* start_bit=23; length=16; factor=0.1; offset=0.0; unit="V" */
    float Phase_Current_R; /* start_bit=39; length=16; factor=0.1; offset=0.0; unit="A" */
    uint16_t RPM_R; /* start_bit=55; length=16; factor=1.0; offset=0.0; unit="rpm" */
} can_msg_GIR_Inv_Data_1_t;

#define CAN_MSG_GIR_INV_DATA_2_ID 49U
#define CAN_MSG_GIR_INV_DATA_2_DLC 8U
#define CAN_MSG_GIR_INV_DATA_2_CYCLE_TIME_MS 0U

#define CAN_MSG_GIR_INV_DATA_2_SIG_MOTOR_TEMP_R_FACTOR 1.0f

#define CAN_MSG_GIR_INV_DATA_2_SIG_INVERTER_TEMP_R_FACTOR 1.0f

#define CAN_MSG_GIR_INV_DATA_2_SIG_AUX_INPUT_R_FACTOR 0.001f

#define CAN_MSG_GIR_INV_DATA_2_SIG_THROTTLE_INPUT_R_FACTOR 0.001f

typedef struct {
    int16_t Motor_Temp_R; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="degC" */
    int16_t Inverter_Temp_R; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="degC" */
    float Aux_Input_R; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="V" */
    float Throttle_Input_R; /* start_bit=55; length=16; factor=0.001; offset=0.0; unit="V" */
} can_msg_GIR_Inv_Data_2_t;

#define CAN_MSG_GIR_SOFTWARE_STATUS_ID 50U
#define CAN_MSG_GIR_SOFTWARE_STATUS_DLC 8U
#define CAN_MSG_GIR_SOFTWARE_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_CURRENT_STATE_FACTOR 1.0f

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_PREVIOUS_REASON_FACTOR 1.0f

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_NEXT_STATE_FACTOR 1.0f

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_FAULT_LATCHED_FACTOR 1.0f

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_CAN_DELTA_MS_FACTOR 1.0f

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_PASS_THROUGH_FLAG_FACTOR 1.0f

#define CAN_MSG_GIR_SOFTWARE_STATUS_SIG_GIR_BOARD_IS_STARTING_FACTOR 1.0f

typedef struct {
    uint8_t GIR_Current_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIR_Previous_Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIR_Next_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIR_Fault_Latched; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint16_t GIR_CAN_Delta_MS; /* start_bit=39; length=16; factor=1.0; offset=0.0; unit="hex" */
    uint8_t GIR_Pass_Through_Flag; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t GIR_Board_Is_Starting; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="bool" */
} can_msg_GIR_Software_Status_t;

#define CAN_MSG_DASH_INPUTS_ID 208U
#define CAN_MSG_DASH_INPUTS_DLC 8U
#define CAN_MSG_DASH_INPUTS_CYCLE_TIME_MS 0U

#define CAN_MSG_DASH_INPUTS_SIG_APPS_RECAL_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_STEERING_RECAL_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_SUSPENSION_RECAL_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_IMU_RECAL_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_FORCE_COOLING_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_MOTOR_CONTROL_ON_OFF_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_TELEMETRY_SD_TOGGLE_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_TELEMETRY_UDP_TOGGLE_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_MOTOR_CONTROL_MODE_TOGGLE_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_MOTOR_CONTROL_TV_OR_SLIP_FACTOR 1.0f

typedef struct {
    uint8_t APPS_Recal; /* start_bit=0; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Steering_Recal; /* start_bit=1; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Suspension_Recal; /* start_bit=2; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t IMU_Recal; /* start_bit=3; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Force_Cooling; /* start_bit=4; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Motor_Control_On_Off; /* start_bit=5; length=1; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Telemetry_SD_Toggle; /* start_bit=6; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Telemetry_UDP_Toggle; /* start_bit=7; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Motor_Control_Mode_Toggle; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Motor_Control_TV_Or_Slip; /* start_bit=16; length=1; factor=1.0; offset=0.0; unit="bool" */
} can_msg_DASH_Inputs_t;

#define CAN_MSG_GPS_ANGLES_ID 240U
#define CAN_MSG_GPS_ANGLES_DLC 8U
#define CAN_MSG_GPS_ANGLES_CYCLE_TIME_MS 0U

#define CAN_MSG_GPS_ANGLES_SIG_GPS_SPEED_FACTOR 0.01f

#define CAN_MSG_GPS_ANGLES_SIG_GPS_COURSE_FACTOR 0.0001f

#define CAN_MSG_GPS_ANGLES_SIG_GPS_YAW_ANGLE_FACTOR 0.0001f

typedef struct {
    float GPS_Speed; /* start_bit=0; length=16; factor=0.01; offset=0.0; unit="m/s" */
    float GPS_Course; /* start_bit=16; length=16; factor=0.0001; offset=0.0; unit="rad" */
    float GPS_Yaw_Angle; /* start_bit=32; length=16; factor=0.0001; offset=0.0; unit="rad" */
} can_msg_GPS_Angles_t;

#define CAN_MSG_IMU_DATA_ID 80U
#define CAN_MSG_IMU_DATA_DLC 8U
#define CAN_MSG_IMU_DATA_CYCLE_TIME_MS 0U

#define CAN_MSG_IMU_DATA_SIG_IMU_HEADER_FACTOR 1.0f

#define CAN_MSG_IMU_DATA_SIG_IMU_MSG_TYPE_FACTOR 1.0f

#define CAN_MSG_IMU_DATA_SIG_ACCEL_X_FACTOR 0.00048828125f

#define CAN_MSG_IMU_DATA_SIG_ACCEL_Y_FACTOR 0.00048828125f

#define CAN_MSG_IMU_DATA_SIG_ACCEL_Z_FACTOR 0.00048828125f

#define CAN_MSG_IMU_DATA_SIG_GYRO_X_FACTOR 0.06103515625f

#define CAN_MSG_IMU_DATA_SIG_GYRO_Y_FACTOR 0.06103515625f

#define CAN_MSG_IMU_DATA_SIG_GYRO_Z_FACTOR 0.06103515625f

#define CAN_MSG_IMU_DATA_SIG_ANGLE_ROLL_FACTOR 0.0054931640625f

#define CAN_MSG_IMU_DATA_SIG_ANGLE_PITCH_FACTOR 0.0054931640625f

#define CAN_MSG_IMU_DATA_SIG_ANGLE_YAW_FACTOR 0.0054931640625f

typedef struct {
    uint8_t IMU_Header; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="const_0x55" */
    uint8_t IMU_Msg_Type; /* start_bit=15; length=8; factor=1.0; offset=0.0; mux=M; unit="hex" */
    float Accel_X; /* start_bit=23; length=16; factor=0.00048828125; offset=0.0; mux=m81; unit="g" */
    float Accel_Y; /* start_bit=39; length=16; factor=0.00048828125; offset=0.0; mux=m81; unit="g" */
    float Accel_Z; /* start_bit=55; length=16; factor=0.00048828125; offset=0.0; mux=m81; unit="g" */
    float Gyro_X; /* start_bit=23; length=16; factor=0.06103515625; offset=0.0; mux=m82; unit="deg/s" */
    float Gyro_Y; /* start_bit=39; length=16; factor=0.06103515625; offset=0.0; mux=m82; unit="deg/s" */
    float Gyro_Z; /* start_bit=55; length=16; factor=0.06103515625; offset=0.0; mux=m82; unit="deg/s" */
    float Angle_Roll; /* start_bit=23; length=16; factor=0.0054931640625; offset=0.0; mux=m83; unit="deg" */
    float Angle_Pitch; /* start_bit=39; length=16; factor=0.0054931640625; offset=0.0; mux=m83; unit="deg" */
    float Angle_Yaw; /* start_bit=55; length=16; factor=0.0054931640625; offset=0.0; mux=m83; unit="deg" */
} can_msg_IMU_Data_t;

#define CAN_MSG_IMU_RECAL_ID 80U
#define CAN_MSG_IMU_RECAL_DLC 8U
#define CAN_MSG_IMU_RECAL_CYCLE_TIME_MS 0U

#define CAN_MSG_IMU_RECAL_SIG_PROTOCOL_HEADER_1_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_PROTOCOL_HEADER_2_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_ADDRESS_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_KEY_LOW_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_KEY_HIGH_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_CALSW_LOW_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_CALSW_HIGH_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_SAVE_LOW_FACTOR 1.0f

#define CAN_MSG_IMU_RECAL_SIG_SAVE_HIGH_FACTOR 1.0f

typedef struct {
    uint8_t Protocol_Header_1; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="const_0xff" */
    uint8_t Protocol_Header_2; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="const_0xaa" */
    uint8_t Address; /* start_bit=23; length=8; factor=1.0; offset=0.0; mux=M; unit="hex" */
    uint8_t KEY_Low; /* start_bit=31; length=8; factor=1.0; offset=0.0; mux=m105; unit="hex" */
    uint8_t KEY_High; /* start_bit=39; length=8; factor=1.0; offset=0.0; mux=m105; unit="hex" */
    uint8_t CALSW_Low; /* start_bit=31; length=8; factor=1.0; offset=0.0; mux=m1; unit="hex" */
    uint8_t CALSW_High; /* start_bit=39; length=8; factor=1.0; offset=0.0; mux=m1; unit="hex" */
    uint8_t SAVE_Low; /* start_bit=31; length=8; factor=1.0; offset=0.0; mux=m0; unit="hex" */
    uint8_t SAVE_High; /* start_bit=39; length=8; factor=1.0; offset=0.0; mux=m0; unit="hex" */
} can_msg_IMU_Recal_t;

#define CAN_MSG_LVSOC_STATUS_1_ID 768U
#define CAN_MSG_LVSOC_STATUS_1_DLC 8U
#define CAN_MSG_LVSOC_STATUS_1_CYCLE_TIME_MS 0U

#define CAN_MSG_LVSOC_STATUS_1_SIG_BAT_VOLTAGE_FACTOR 0.001f

#define CAN_MSG_LVSOC_STATUS_1_SIG_CURRENT_FACTOR 0.01f

#define CAN_MSG_LVSOC_STATUS_1_SIG_SOC_FACTOR 0.1f

#define CAN_MSG_LVSOC_STATUS_1_SIG_PEAK_CURRENT_10S_FACTOR 0.1f

typedef struct {
    float Bat_Voltage; /* start_bit=7; length=16; factor=0.001; offset=0.0; unit="V" */
    float Current; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="A" */
    float SoC; /* start_bit=39; length=16; factor=0.1; offset=0.0; unit="%" */
    float Peak_Current_10s; /* start_bit=55; length=16; factor=0.1; offset=0.0; unit="A" */
} can_msg_LVSOC_Status_1_t;

#define CAN_MSG_LVSOC_STATUS_2_ID 769U
#define CAN_MSG_LVSOC_STATUS_2_DLC 8U
#define CAN_MSG_LVSOC_STATUS_2_CYCLE_TIME_MS 0U

#define CAN_MSG_LVSOC_STATUS_2_SIG_CAPACITY_FACTOR 1.0f

#define CAN_MSG_LVSOC_STATUS_2_SIG_SUDO_POWEROFF_FACTOR 1.0f

#define CAN_MSG_LVSOC_STATUS_2_SIG_TIME_REMAINING_TTE_TTF_FACTOR 1.0f

#define CAN_MSG_LVSOC_STATUS_2_SIG_AVG_CURRENT_10S_FACTOR 0.01f

typedef struct {
    uint16_t Capacity; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="mAh" */
    uint16_t sudo_poweroff; /* start_bit=23; length=16; factor=1.0; offset=0.0 */
    uint16_t Time_Remaining_TTE_TTF; /* start_bit=39; length=16; factor=1.0; offset=0.0; unit="min" */
    float Avg_Current_10s; /* start_bit=55; length=16; factor=0.01; offset=0.0; unit="A" */
} can_msg_LVSOC_Status_2_t;

#define CAN_MSG_PRG_BOOTLOADER_ID 1264U
#define CAN_MSG_PRG_BOOTLOADER_DLC 8U
#define CAN_MSG_PRG_BOOTLOADER_CYCLE_TIME_MS 0U

#define CAN_MSG_PRG_BOOTLOADER_SIG_MCU_ID_FACTOR 1.0f

typedef struct {
    uint8_t MCU_ID; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="id" */
} can_msg_PRG_Bootloader_t;

#define CAN_MSG_PRG_INVERTER_ID 1265U
#define CAN_MSG_PRG_INVERTER_DLC 8U
#define CAN_MSG_PRG_INVERTER_CYCLE_TIME_MS 0U

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE0_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE1_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE2_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE3_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE4_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE5_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE6_FACTOR 1.0f

#define CAN_MSG_PRG_INVERTER_SIG_PRG_INV_BYTE7_FACTOR 1.0f

typedef struct {
    uint8_t Prg_Inv_Byte0; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte1; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte2; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte3; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte4; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte5; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte6; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="hex" */
    uint8_t Prg_Inv_Byte7; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="hex" */
} can_msg_PRG_Inverter_t;

#endif /* INC_CAN_SIGNAL_DEFS_H */
