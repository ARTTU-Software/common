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

#define CAN_MSG_GF_MISC_SENSORS_SIG_LAP_COUNT_START_BTN_FACTOR 1.0f

typedef struct {
    uint8_t APPS1; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="%" */
    uint8_t APPS2; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="%" */
    float Steering; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="deg" */
    uint8_t R2D_BTN; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t LAP_COUNT_START_BTN; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bool" */
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

#define CAN_MSG_BM_BATTERY_STATUS_ID 16U
#define CAN_MSG_BM_BATTERY_STATUS_DLC 8U
#define CAN_MSG_BM_BATTERY_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_BATTERY_STATUS_SIG_BATTERY_STATE_FACTOR 1.0f

#define CAN_MSG_BM_BATTERY_STATUS_SIG_HEALTH_FLAGS_FACTOR 1.0f

#define CAN_MSG_BM_BATTERY_STATUS_SIG_ACTIVE_FAULTS_FACTOR 1.0f

#define CAN_MSG_BM_BATTERY_STATUS_SIG_ASSERTED_SINCE_LAST_SUBMISSION_FACTOR 1.0f

#define CAN_MSG_BM_BATTERY_STATUS_SIG_CLEARED_SINCE_LAST_SUBMISSION_FACTOR 1.0f

typedef struct {
    uint8_t Battery_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Health_Flags; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="bits" */
    uint16_t Active_Faults; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="bits" */
    uint16_t Asserted_Since_Last_Submission; /* start_bit=39; length=16; factor=1.0; offset=0.0; unit="bits" */
    uint16_t Cleared_Since_Last_Submission; /* start_bit=55; length=16; factor=1.0; offset=0.0; unit="bits" */
} can_msg_BM_Battery_Status_t;

#define CAN_MSG_BM_CHARGER_STATUS_ID 22U
#define CAN_MSG_BM_CHARGER_STATUS_DLC 8U
#define CAN_MSG_BM_CHARGER_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_CHARGER_STATUS_SIG_CHARGER_STATE_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_REASON_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_LAST_FAULT_REASON_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_LAST_TRANSPORT_ERROR_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_CHARGER_FLAGS_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_TC_RAW_STATUS_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_CHARGE_PHASE_FACTOR 1.0f

#define CAN_MSG_BM_CHARGER_STATUS_SIG_COMMAND_CURRENT_FACTOR 100.0f

typedef struct {
    uint8_t Charger_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Reason; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Last_Fault_Reason; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Last_Transport_Error; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Charger_Flags; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bits" */
    uint8_t TC_Raw_Status; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bits" */
    uint8_t Charge_Phase; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="enum" */
    float Command_Current; /* start_bit=63; length=8; factor=100.0; offset=0.0; unit="mA" */
} can_msg_BM_Charger_Status_t;

#define CAN_MSG_BM_VEHICLE_STATUS_ID 229U
#define CAN_MSG_BM_VEHICLE_STATUS_DLC 8U
#define CAN_MSG_BM_VEHICLE_STATUS_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_VEHICLE_STATUS_SIG_VEHICLE_STATE_FACTOR 1.0f

#define CAN_MSG_BM_VEHICLE_STATUS_SIG_OPERATING_MODE_FACTOR 1.0f

#define CAN_MSG_BM_VEHICLE_STATUS_SIG_R2D_STATE_FACTOR 1.0f

#define CAN_MSG_BM_VEHICLE_STATUS_SIG_VEHICLE_REASON_FACTOR 1.0f

#define CAN_MSG_BM_VEHICLE_STATUS_SIG_COMMANDED_OUTPUTS_FACTOR 1.0f

#define CAN_MSG_BM_VEHICLE_STATUS_SIG_VEHICLE_FLAGS_FACTOR 1.0f

typedef struct {
    uint8_t Vehicle_State; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Operating_Mode; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t R2D_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Vehicle_Reason; /* start_bit=31; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Commanded_Outputs; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bits" */
    uint8_t Vehicle_Flags; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="bits" */
} can_msg_BM_Vehicle_Status_t;

#define CAN_MSG_BM_INVERTER_COMMAND_ID 224U
#define CAN_MSG_BM_INVERTER_COMMAND_DLC 8U
#define CAN_MSG_BM_INVERTER_COMMAND_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_INVERTER_COMMAND_SIG_MOTOR_ENABLE_FACTOR 1.0f

#define CAN_MSG_BM_INVERTER_COMMAND_SIG_RESERVED_1_2_FACTOR 1.0f

#define CAN_MSG_BM_INVERTER_COMMAND_SIG_LEFT_DEMAND_FACTOR 1.0f

#define CAN_MSG_BM_INVERTER_COMMAND_SIG_RIGHT_DEMAND_FACTOR 1.0f

#define CAN_MSG_BM_INVERTER_COMMAND_SIG_RESERVED_5_7_FACTOR 1.0f

typedef struct {
    uint8_t Motor_Enable; /* start_bit=0; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint16_t Reserved_1_2; /* start_bit=8; length=16; factor=1.0; offset=0.0 */
    uint8_t Left_Demand; /* start_bit=24; length=8; factor=1.0; offset=0.0; unit="%" */
    uint8_t Right_Demand; /* start_bit=32; length=8; factor=1.0; offset=0.0; unit="%" */
    uint32_t Reserved_5_7; /* start_bit=40; length=24; factor=1.0; offset=0.0 */
} can_msg_BM_Inverter_Command_t;

#define CAN_MSG_BM_REAR_BUZZER_ID 179U
#define CAN_MSG_BM_REAR_BUZZER_DLC 8U
#define CAN_MSG_BM_REAR_BUZZER_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_REAR_BUZZER_SIG_R2D_BUZZER_FACTOR 1.0f

#define CAN_MSG_BM_REAR_BUZZER_SIG_RESERVED_FACTOR 1.0f

typedef struct {
    uint8_t R2D_Buzzer; /* start_bit=0; length=8; factor=1.0; offset=0.0; unit="bool" */
    float Reserved; /* start_bit=8; length=56; factor=1.0; offset=0.0 */
} can_msg_BM_Rear_Buzzer_t;

#define CAN_MSG_BM_TC_COMMAND_ID 403105268U
#define CAN_MSG_BM_TC_COMMAND_IS_EXTENDED 1U
#define CAN_MSG_BM_TC_COMMAND_DLC 8U
#define CAN_MSG_BM_TC_COMMAND_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_TC_COMMAND_SIG_VOLTAGE_LIMIT_FACTOR 0.1f

#define CAN_MSG_BM_TC_COMMAND_SIG_CURRENT_LIMIT_FACTOR 0.1f

#define CAN_MSG_BM_TC_COMMAND_SIG_CONTROL_FACTOR 1.0f

#define CAN_MSG_BM_TC_COMMAND_SIG_RESERVED_FACTOR 1.0f

typedef struct {
    float Voltage_Limit; /* start_bit=7; length=16; factor=0.1; offset=0.0; unit="V" */
    float Current_Limit; /* start_bit=23; length=16; factor=0.1; offset=0.0; unit="A" */
    uint8_t Control; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint32_t Reserved; /* start_bit=47; length=24; factor=1.0; offset=0.0 */
} can_msg_BM_TC_Command_t;

#define CAN_MSG_BM_SETTINGS_REPLY_ID 402U
#define CAN_MSG_BM_SETTINGS_REPLY_DLC 8U
#define CAN_MSG_BM_SETTINGS_REPLY_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_SETTINGS_REPLY_SIG_OPERATION_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REPLY_SIG_TRANSACTION_TAG_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REPLY_SIG_RESULT_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REPLY_SIG_STORAGE_STATE_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REPLY_SIG_VALUE_BITS_FACTOR 1.0f

typedef struct {
    uint8_t Operation; /* start_bit=0; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Transaction_Tag; /* start_bit=8; length=8; factor=1.0; offset=0.0 */
    uint8_t Result; /* start_bit=16; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Storage_State; /* start_bit=24; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint32_t Value_Bits; /* start_bit=32; length=32; factor=1.0; offset=0.0 */
} can_msg_BM_Settings_Reply_t;

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

#define CAN_MSG_DASH_INPUTS_SIG_SLIP_CONTROL_ON_OFF_FACTOR 1.0f

#define CAN_MSG_DASH_INPUTS_SIG_LAP_RESET_BTN_FACTOR 1.0f

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
    uint8_t Slip_Control_On_Off; /* start_bit=16; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Lap_Reset_Btn; /* start_bit=17; length=1; factor=1.0; offset=0.0; unit="bool" */
} can_msg_DASH_Inputs_t;

#define CAN_MSG_DASH_PID_TUNE_1_ID 209U
#define CAN_MSG_DASH_PID_TUNE_1_DLC 8U
#define CAN_MSG_DASH_PID_TUNE_1_CYCLE_TIME_MS 0U

#define CAN_MSG_DASH_PID_TUNE_1_SIG_SLIP_CTRL_KP_FACTOR 1.0f

#define CAN_MSG_DASH_PID_TUNE_1_SIG_SLIP_CTRL_KI_FACTOR 1.0f

#define CAN_MSG_DASH_PID_TUNE_1_SIG_SLIP_CTRL_KD_FACTOR 0.01f

#define CAN_MSG_DASH_PID_TUNE_1_SIG_SLIP_CTRL_KN_FACTOR 1.0f

typedef struct {
    uint16_t Slip_Ctrl_Kp; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="unit" */
    uint16_t Slip_Ctrl_Ki; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="unit" */
    float Slip_Ctrl_Kd; /* start_bit=39; length=16; factor=0.01; offset=0.0; unit="unit" */
    uint16_t Slip_Ctrl_Kn; /* start_bit=55; length=16; factor=1.0; offset=0.0; unit="unit" */
} can_msg_DASH_PID_Tune_1_t;

#define CAN_MSG_DASH_PID_TUNE_2_ID 210U
#define CAN_MSG_DASH_PID_TUNE_2_DLC 8U
#define CAN_MSG_DASH_PID_TUNE_2_CYCLE_TIME_MS 0U

#define CAN_MSG_DASH_PID_TUNE_2_SIG_KP_TV_BETA_FACTOR 1.0f

#define CAN_MSG_DASH_PID_TUNE_2_SIG_KP_TV_YAW_FACTOR 1.0f

#define CAN_MSG_DASH_PID_TUNE_2_SIG_SLIP_STEER_TH_FACTOR 1.0f

typedef struct {
    uint16_t Kp_TV_Beta; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="unit" */
    uint16_t Kp_TV_Yaw; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="unit" */
    uint16_t Slip_Steer_Th; /* start_bit=39; length=16; factor=1.0; offset=0.0; unit="deg" */
} can_msg_DASH_PID_Tune_2_t;

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

#define CAN_MSG_LAP_COMPLETED_ID 241U
#define CAN_MSG_LAP_COMPLETED_DLC 8U
#define CAN_MSG_LAP_COMPLETED_CYCLE_TIME_MS 0U

#define CAN_MSG_LAP_COMPLETED_SIG_LAP_COMPLETED_FLAG_FACTOR 1.0f

#define CAN_MSG_LAP_COMPLETED_SIG_LAP_COUNT_FACTOR 1.0f

typedef struct {
    uint8_t Lap_Completed_Flag; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint16_t Lap_Count; /* start_bit=15; length=16; factor=1.0; offset=0.0; unit="dec" */
} can_msg_Lap_Completed_t;

#define CAN_MSG_SBG_ECAN_MSG_STATUS_03_ID 258U
#define CAN_MSG_SBG_ECAN_MSG_STATUS_03_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_STATUS_03_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_STATUS_03_SIG_SOLUTION_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_STATUS_03_SIG_HEAVE_STATUS_FACTOR 1.0f

typedef struct {
    uint32_t SOLUTION; /* start_bit=0; length=32; factor=1.0; offset=0.0 */
    uint16_t HEAVE_STATUS; /* start_bit=32; length=16; factor=1.0; offset=0.0 */
} can_msg_SBG_ECAN_MSG_STATUS_03_t;

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_ID 273U
#define CAN_MSG_SBG_ECAN_MSG_UTC_1_DLC 8U
#define CAN_MSG_SBG_ECAN_MSG_UTC_1_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_YEAR_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_MONTH_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_DAY_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_HOUR_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_MIN_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_SEC_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_UTC_1_SIG_MICRO_SEC_FACTOR 100.0f

typedef struct {
    float YEAR; /* start_bit=0; length=8; factor=1.0; offset=2000.0; unit="year" */
    uint8_t MONTH; /* start_bit=8; length=8; factor=1.0; offset=0.0; unit="month" */
    uint8_t DAY; /* start_bit=16; length=8; factor=1.0; offset=0.0; unit="d" */
    uint8_t HOUR; /* start_bit=24; length=8; factor=1.0; offset=0.0; unit="h" */
    uint8_t MIN; /* start_bit=32; length=8; factor=1.0; offset=0.0; unit="min" */
    uint8_t SEC; /* start_bit=40; length=8; factor=1.0; offset=0.0; unit="s" */
    float MICRO_SEC; /* start_bit=48; length=16; factor=100.0; offset=0.0; unit="us" */
} can_msg_SBG_ECAN_MSG_UTC_1_t;

#define CAN_MSG_SBG_ECAN_MSG_IMU_ACCEL_ID 289U
#define CAN_MSG_SBG_ECAN_MSG_IMU_ACCEL_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_IMU_ACCEL_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_IMU_ACCEL_SIG_ACCEL_X_FACTOR 0.01f

#define CAN_MSG_SBG_ECAN_MSG_IMU_ACCEL_SIG_ACCEL_Y_FACTOR 0.01f

#define CAN_MSG_SBG_ECAN_MSG_IMU_ACCEL_SIG_ACCEL_Z_FACTOR 0.01f

typedef struct {
    float ACCEL_X; /* start_bit=0; length=16; factor=0.01; offset=0.0; unit="m.s-2" */
    float ACCEL_Y; /* start_bit=16; length=16; factor=0.01; offset=0.0; unit="m.s-2" */
    float ACCEL_Z; /* start_bit=32; length=16; factor=0.01; offset=0.0; unit="m.s-2" */
} can_msg_SBG_ECAN_MSG_IMU_ACCEL_t;

#define CAN_MSG_SBG_ECAN_MSG_IMU_GYRO_ID 290U
#define CAN_MSG_SBG_ECAN_MSG_IMU_GYRO_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_IMU_GYRO_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_IMU_GYRO_SIG_GYRO_X_FACTOR 0.001f

#define CAN_MSG_SBG_ECAN_MSG_IMU_GYRO_SIG_GYRO_Y_FACTOR 0.001f

#define CAN_MSG_SBG_ECAN_MSG_IMU_GYRO_SIG_GYRO_Z_FACTOR 0.001f

typedef struct {
    float GYRO_X; /* start_bit=0; length=16; factor=0.001; offset=0.0; unit="rad.s-1" */
    float GYRO_Y; /* start_bit=16; length=16; factor=0.001; offset=0.0; unit="rad.s-1" */
    float GYRO_Z; /* start_bit=32; length=16; factor=0.001; offset=0.0; unit="rad.s-1" */
} can_msg_SBG_ECAN_MSG_IMU_GYRO_t;

#define CAN_MSG_SBG_ECAN_MSG_EKF_EULER_ID 306U
#define CAN_MSG_SBG_ECAN_MSG_EKF_EULER_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_EKF_EULER_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_EKF_EULER_SIG_ROLL_FACTOR 0.0001f

#define CAN_MSG_SBG_ECAN_MSG_EKF_EULER_SIG_PITCH_FACTOR 0.0001f

#define CAN_MSG_SBG_ECAN_MSG_EKF_EULER_SIG_YAW_FACTOR 0.0001f

typedef struct {
    float ROLL; /* start_bit=0; length=16; factor=0.0001; offset=0.0; unit="rad" */
    float PITCH; /* start_bit=16; length=16; factor=0.0001; offset=0.0; unit="rad" */
    float YAW; /* start_bit=32; length=16; factor=0.0001; offset=0.0; unit="rad" */
} can_msg_SBG_ECAN_MSG_EKF_EULER_t;

#define CAN_MSG_SBG_ECAN_MSG_EKF_POS_ID 308U
#define CAN_MSG_SBG_ECAN_MSG_EKF_POS_DLC 8U
#define CAN_MSG_SBG_ECAN_MSG_EKF_POS_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_EKF_POS_SIG_LATITUDE_FACTOR 1e-07f

#define CAN_MSG_SBG_ECAN_MSG_EKF_POS_SIG_LONGITUDE_FACTOR 1e-07f

typedef struct {
    float LATITUDE; /* start_bit=0; length=32; factor=1e-07; offset=0.0; unit="Â°" */
    float LONGITUDE; /* start_bit=32; length=32; factor=1e-07; offset=0.0; unit="Â°" */
} can_msg_SBG_ECAN_MSG_EKF_POS_t;

#define CAN_MSG_SBG_ECAN_MSG_EKF_ALTITUDE_ID 309U
#define CAN_MSG_SBG_ECAN_MSG_EKF_ALTITUDE_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_EKF_ALTITUDE_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_EKF_ALTITUDE_SIG_ALTITUDE_FACTOR 0.001f

#define CAN_MSG_SBG_ECAN_MSG_EKF_ALTITUDE_SIG_UNDULATION_FACTOR 0.005f

typedef struct {
    float ALTITUDE; /* start_bit=0; length=32; factor=0.001; offset=0.0; unit="m" */
    float UNDULATION; /* start_bit=32; length=16; factor=0.005; offset=0.0; unit="m" */
} can_msg_SBG_ECAN_MSG_EKF_ALTITUDE_t;

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_NED_ID 311U
#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_NED_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_NED_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_NED_SIG_VELOCITY_N_FACTOR 0.01f

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_NED_SIG_VELOCITY_E_FACTOR 0.01f

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_NED_SIG_VELOCITY_D_FACTOR 0.01f

typedef struct {
    float VELOCITY_N; /* start_bit=0; length=16; factor=0.01; offset=0.0; unit="m.s-1" */
    float VELOCITY_E; /* start_bit=16; length=16; factor=0.01; offset=0.0; unit="m.s-1" */
    float VELOCITY_D; /* start_bit=32; length=16; factor=0.01; offset=0.0; unit="m.s-1" */
} can_msg_SBG_ECAN_MSG_EKF_VEL_NED_t;

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_BODY_ID 313U
#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_BODY_DLC 6U
#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_BODY_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_BODY_SIG_VELOCITY_X_FACTOR 0.01f

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_BODY_SIG_VELOCITY_Y_FACTOR 0.01f

#define CAN_MSG_SBG_ECAN_MSG_EKF_VEL_BODY_SIG_VELOCITY_Z_FACTOR 0.01f

typedef struct {
    float VELOCITY_X; /* start_bit=0; length=16; factor=0.01; offset=0.0; unit="m.s-1" */
    float VELOCITY_Y; /* start_bit=16; length=16; factor=0.01; offset=0.0; unit="m.s-1" */
    float VELOCITY_Z; /* start_bit=32; length=16; factor=0.01; offset=0.0; unit="m.s-1" */
} can_msg_SBG_ECAN_MSG_EKF_VEL_BODY_t;

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_INFO_ID 372U
#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_INFO_DLC 8U
#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_INFO_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_INFO_SIG_TIME_STAMP_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_INFO_SIG_STATUS_FACTOR 1.0f

typedef struct {
    uint32_t TIME_STAMP; /* start_bit=0; length=32; factor=1.0; offset=0.0; unit="Âµs" */
    uint32_t STATUS; /* start_bit=32; length=32; factor=1.0; offset=0.0 */
} can_msg_SBG_ECAN_MSG_GPS1_POS_INFO_t;

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_ID 374U
#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_DLC 8U
#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_CYCLE_TIME_MS 0U

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_SIG_ALTITUDE_FACTOR 0.001f

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_SIG_UNDULATION_FACTOR 0.005f

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_SIG_NUM_SV_FACTOR 1.0f

#define CAN_MSG_SBG_ECAN_MSG_GPS1_POS_ALT_SIG_DIFF_CORR_AGE_FACTOR 1.0f

typedef struct {
    float ALTITUDE; /* start_bit=0; length=32; factor=0.001; offset=0.0; unit="m" */
    float UNDULATION; /* start_bit=32; length=16; factor=0.005; offset=0.0; unit="m" */
    uint8_t NUM_SV; /* start_bit=48; length=8; factor=1.0; offset=0.0 */
    uint8_t DIFF_CORR_AGE; /* start_bit=56; length=8; factor=1.0; offset=0.0; unit="s" */
} can_msg_SBG_ECAN_MSG_GPS1_POS_ALT_t;

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

#define CAN_MSG_BM_SETTINGS_REQUEST_ID 401U
#define CAN_MSG_BM_SETTINGS_REQUEST_DLC 8U
#define CAN_MSG_BM_SETTINGS_REQUEST_CYCLE_TIME_MS 0U

#define CAN_MSG_BM_SETTINGS_REQUEST_SIG_OPERATION_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REQUEST_SIG_TRANSACTION_TAG_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REQUEST_SIG_RESERVED_FACTOR 1.0f

#define CAN_MSG_BM_SETTINGS_REQUEST_SIG_VALUE_BITS_FACTOR 1.0f

typedef struct {
    uint8_t Operation; /* start_bit=0; length=8; factor=1.0; offset=0.0; unit="enum" */
    uint8_t Transaction_Tag; /* start_bit=8; length=8; factor=1.0; offset=0.0 */
    uint16_t Reserved; /* start_bit=16; length=16; factor=1.0; offset=0.0 */
    uint32_t Value_Bits; /* start_bit=32; length=32; factor=1.0; offset=0.0 */
} can_msg_BM_Settings_Request_t;

#define CAN_MSG_TIC_SWITCH_SDC_ID 400U
#define CAN_MSG_TIC_SWITCH_SDC_DLC 8U
#define CAN_MSG_TIC_SWITCH_SDC_CYCLE_TIME_MS 0U

#define CAN_MSG_TIC_SWITCH_SDC_SIG_CHARGE_SWITCH_FACTOR 1.0f

#define CAN_MSG_TIC_SWITCH_SDC_SIG_SDC_CLOSED_FACTOR 1.0f

#define CAN_MSG_TIC_SWITCH_SDC_SIG_RESERVED_FACTOR 1.0f

typedef struct {
    uint8_t Charge_Switch; /* start_bit=0; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t SDC_Closed; /* start_bit=8; length=8; factor=1.0; offset=0.0; unit="bool" */
    float Reserved; /* start_bit=16; length=48; factor=1.0; offset=0.0 */
} can_msg_TIC_Switch_SDC_t;

#define CAN_MSG_TC_CHARGER_FEEDBACK_ID 419385573U
#define CAN_MSG_TC_CHARGER_FEEDBACK_IS_EXTENDED 1U
#define CAN_MSG_TC_CHARGER_FEEDBACK_DLC 8U
#define CAN_MSG_TC_CHARGER_FEEDBACK_CYCLE_TIME_MS 0U

#define CAN_MSG_TC_CHARGER_FEEDBACK_SIG_OUTPUT_VOLTAGE_FACTOR 0.1f

#define CAN_MSG_TC_CHARGER_FEEDBACK_SIG_CURRENT_NEGATIVE_FACTOR 1.0f

#define CAN_MSG_TC_CHARGER_FEEDBACK_SIG_CURRENT_MAGNITUDE_FACTOR 0.1f

#define CAN_MSG_TC_CHARGER_FEEDBACK_SIG_RAW_STATUS_FACTOR 1.0f

#define CAN_MSG_TC_CHARGER_FEEDBACK_SIG_RAW_TEMPERATURE_FACTOR 1.0f

#define CAN_MSG_TC_CHARGER_FEEDBACK_SIG_RESERVED_FEEDBACK_FACTOR 1.0f

typedef struct {
    float Output_Voltage; /* start_bit=7; length=16; factor=0.1; offset=0.0; unit="V" */
    uint8_t Current_Negative; /* start_bit=23; length=1; factor=1.0; offset=0.0; unit="bool" */
    float Current_Magnitude; /* start_bit=22; length=15; factor=0.1; offset=0.0; unit="A" */
    uint8_t Raw_Status; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bits" */
    uint8_t Raw_Temperature; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="raw" */
    uint16_t Reserved_Feedback; /* start_bit=55; length=16; factor=1.0; offset=0.0; unit="raw" */
} can_msg_TC_Charger_Feedback_t;

#endif /* INC_CAN_SIGNAL_DEFS_H */
