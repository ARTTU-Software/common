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

#define CAN_MSG_GF_MISC_SENSORS_SIG_APPS1_FACTOR 0.001f

#define CAN_MSG_GF_MISC_SENSORS_SIG_APPS2_FACTOR 0.001f

#define CAN_MSG_GF_MISC_SENSORS_SIG_STEERING_FACTOR 0.01f

#define CAN_MSG_GF_MISC_SENSORS_SIG_R2D_BTN_FACTOR 1.0f

#define CAN_MSG_GF_MISC_SENSORS_SIG_SOFTWARE_STATUS_FACTOR 1.0f

typedef struct {
    float APPS1; /* start_bit=7; length=8; factor=0.001; offset=0.0; unit="%" */
    float APPS2; /* start_bit=15; length=8; factor=0.001; offset=0.0; unit="%" */
    float Steering; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="deg" */
    uint8_t R2D_BTN; /* start_bit=39; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Software_Status; /* start_bit=47; length=8; factor=1.0; offset=0.0; unit="enum" */
} can_msg_GF_Misc_Sensors_t;

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

#define CAN_MSG_GR_MISC_SENSORS_SIG_SOFTWARE_STATUS_FACTOR 1.0f

typedef struct {
    float Brake; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="bar" */
    float JB_50A; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="A" */
    float JB_200A; /* start_bit=39; length=16; factor=0.01; offset=0.0; unit="A" */
    uint8_t SDC; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Software_Status; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="enum" */
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

#define CAN_MSG_ECU_SOFTWARE_DATA_ID 229U
#define CAN_MSG_ECU_SOFTWARE_DATA_DLC 8U
#define CAN_MSG_ECU_SOFTWARE_DATA_CYCLE_TIME_MS 0U

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_REGEN_ENABLE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_MOTOR_ENABLE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_R2D_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_TS_ON_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_INPRECHARGE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_PRECH_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_AIR_NEG_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_AIR_POS_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_FAN_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_PUMP_STATE_FACTOR 1.0f

#define CAN_MSG_ECU_SOFTWARE_DATA_SIG_ECU_CODE_STATE_FACTOR 1.0f

typedef struct {
    uint8_t Regen_Enable; /* start_bit=0; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Motor_Enable; /* start_bit=1; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t R2D_State; /* start_bit=2; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t TS_ON; /* start_bit=3; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t inPrecharge; /* start_bit=4; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t PRECH_State; /* start_bit=5; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t AIR_Neg_State; /* start_bit=6; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t AIR_Pos_State; /* start_bit=7; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Fan_State; /* start_bit=8; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Pump_State; /* start_bit=9; length=1; factor=1.0; offset=0.0; unit="bool" */
    uint8_t ECU_Code_State; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="enum" */
} can_msg_ECU_Software_Data_t;

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

#define CAN_MSG_BMS_ERRORS_TEMPS_ID 16U
#define CAN_MSG_BMS_ERRORS_TEMPS_DLC 8U
#define CAN_MSG_BMS_ERRORS_TEMPS_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_TOTAL_PACK_CAPACITY_FACTOR 0.2f

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

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A04_CELL_OPEN_WIRING_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_DTC_P0A80_WEAK_CELL_FAULT_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_MULTIPURPOSE_OUTPUT_2_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_BLANK_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_BLANK2_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_LOW_TEMPERATURE_FACTOR 0.333333f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_HIGH_TEMPERATURE_FACTOR 0.333333f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_HIGH_THERMISTOR_ID_FACTOR 1.0f

#define CAN_MSG_BMS_ERRORS_TEMPS_SIG_LOW_THERMISTOR_ID_FACTOR 1.0f

typedef struct {
    float Total_Pack_Capacity; /* start_bit=7; length=8; factor=0.2; offset=0.0; unit="Ahr" */
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
    uint8_t DTC_P0A04_Cell_Open_Wiring_Fault_2; /* start_bit=27; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t DTC_P0A80_Weak_Cell_Fault; /* start_bit=28; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t MultiPurpose_Output_2; /* start_bit=29; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Blank; /* start_bit=30; length=1; factor=1.0; offset=0.0; unit="Bool" */
    uint8_t Blank2; /* start_bit=31; length=1; factor=1.0; offset=0.0; unit="Bool" */
    float Low_Temperature; /* start_bit=39; length=8; factor=0.333333; offset=0.0; unit="C" */
    float High_Temperature; /* start_bit=47; length=8; factor=0.333333; offset=0.0; unit="C" */
    uint8_t High_Thermistor_ID; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t Low_Thermistor_ID; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Errors_Temps_t;

#define CAN_MSG_BMS_SOC_VOLTAGE_ID 17U
#define CAN_MSG_BMS_SOC_VOLTAGE_DLC 8U
#define CAN_MSG_BMS_SOC_VOLTAGE_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_OPEN_VOLTAGE_FACTOR 0.01f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_INST_VOLTAGE_FACTOR 0.01f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_HEALTH_FACTOR 0.5f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_SIMULATED_SOC_FACTOR 0.5f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_PACK_SOC_FACTOR 0.5f

#define CAN_MSG_BMS_SOC_VOLTAGE_SIG_ADAPTIVE_SOC_FACTOR 0.5f

typedef struct {
    float Pack_Open_Voltage; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="V" */
    float Pack_Inst_Voltage; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="V" */
    float Pack_Health; /* start_bit=39; length=8; factor=0.5; offset=0.0; unit="%" */
    float Simulated_SOC; /* start_bit=47; length=8; factor=0.5; offset=0.0; unit="%" */
    float Pack_SOC; /* start_bit=55; length=8; factor=0.5; offset=0.0; unit="%" */
    float Adaptive_SOC; /* start_bit=63; length=8; factor=0.5; offset=0.0; unit="%" */
} can_msg_BMS_SoC_Voltage_t;

#define CAN_MSG_BMS_CELL_DATA_ID 18U
#define CAN_MSG_BMS_CELL_DATA_DLC 8U
#define CAN_MSG_BMS_CELL_DATA_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CELL_DATA_SIG_AVG_CELL_RESISTANCE_FACTOR 0.5f

#define CAN_MSG_BMS_CELL_DATA_SIG_LOW_CELL_RESISTANCE_FACTOR 0.5f

#define CAN_MSG_BMS_CELL_DATA_SIG_HIGH_CELL_RESISTANCE_FACTOR 0.5f

#define CAN_MSG_BMS_CELL_DATA_SIG_LOW_OPENCELL_VOLTAGE_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_SIG_HIGH_OPENCELL_VOLTAGE_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_SIG_AVG_OPENCELL_VOLTAGE_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_SIG_LOW_OPENCELL_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_DATA_SIG_HIGH_OPENCELL_ID_FACTOR 1.0f

typedef struct {
    float Avg_Cell_Resistance; /* start_bit=7; length=8; factor=0.5; offset=0.0; unit="mOhm" */
    float Low_Cell_Resistance; /* start_bit=15; length=8; factor=0.5; offset=0.0; unit="mOhm" */
    float High_Cell_Resistance; /* start_bit=23; length=8; factor=0.5; offset=0.0; unit="mOhm" */
    float Low_Opencell_Voltage; /* start_bit=31; length=8; factor=0.02; offset=0.0; unit="V" */
    float High_Opencell_Voltage; /* start_bit=39; length=8; factor=0.02; offset=0.0; unit="V" */
    float Avg_Opencell_Voltage; /* start_bit=47; length=8; factor=0.02; offset=0.0; unit="V" */
    uint8_t Low_Opencell_ID; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t High_Opencell_ID; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Cell_Data_t;

#define CAN_MSG_BMS_CELL_DATA_2_ID 19U
#define CAN_MSG_BMS_CELL_DATA_2_DLC 8U
#define CAN_MSG_BMS_CELL_DATA_2_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CELL_DATA_2_SIG_ADAPTIVE_TOTAL_CAPACITY_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_HIGH_INTRES_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_LOW_INTRES_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_LOW_CELL_VOLTAGE_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_HIGH_CELL_VOLTAGE_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_AVG_CELL_VOLTAGE_FACTOR 0.02f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_LOW_CELL_VOLTAGE_ID_FACTOR 1.0f

#define CAN_MSG_BMS_CELL_DATA_2_SIG_HIGH_CELL_VOLTAGE_ID_FACTOR 1.0f

typedef struct {
    float Adaptive_Total_Capacity; /* start_bit=7; length=8; factor=0.02; offset=0.0; unit="Ahr" */
    uint8_t High_Intres_ID; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t Low_Intres_ID; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="ID" */
    float Low_Cell_Voltage; /* start_bit=31; length=8; factor=0.02; offset=0.0; unit="V" */
    float High_Cell_Voltage; /* start_bit=39; length=8; factor=0.02; offset=0.0; unit="V" */
    float Avg_Cell_Voltage; /* start_bit=47; length=8; factor=0.02; offset=0.0; unit="V" */
    uint8_t Low_Cell_Voltage_ID; /* start_bit=55; length=8; factor=1.0; offset=0.0; unit="ID" */
    uint8_t High_Cell_Voltage_ID; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="ID" */
} can_msg_BMS_Cell_Data_2_t;

#define CAN_MSG_BMS_CURRENT_ID 20U
#define CAN_MSG_BMS_CURRENT_DLC 8U
#define CAN_MSG_BMS_CURRENT_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_CURRENT_SIG_AVERAGE_CURRENT_FACTOR 0.01f

#define CAN_MSG_BMS_CURRENT_SIG_PACK_CURRENT_FACTOR 0.01f

#define CAN_MSG_BMS_CURRENT_SIG_PACK_DOD_FACTOR 0.5f

#define CAN_MSG_BMS_CURRENT_SIG_PACK_DCL_FACTOR 0.01f

#define CAN_MSG_BMS_CURRENT_SIG_BLANK_FACTOR 1.0f

typedef struct {
    float Average_Current; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="A" */
    float Pack_Current; /* start_bit=23; length=16; factor=0.01; offset=0.0; unit="A" */
    float Pack_DOD; /* start_bit=39; length=8; factor=0.5; offset=0.0; unit="%" */
    float Pack_DCL; /* start_bit=47; length=16; factor=0.01; offset=0.0; unit="A" */
    uint8_t Blank; /* start_bit=63; length=8; factor=1.0; offset=0.0; unit="const0x0" */
} can_msg_BMS_Current_t;

#define CAN_MSG_BMS_DATA_RANDOM_ID 54U
#define CAN_MSG_BMS_DATA_RANDOM_DLC 8U
#define CAN_MSG_BMS_DATA_RANDOM_CYCLE_TIME_MS 0U

#define CAN_MSG_BMS_DATA_RANDOM_SIG_CELLID_FACTOR 1.0f

#define CAN_MSG_BMS_DATA_RANDOM_SIG_CELLVOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_DATA_RANDOM_SIG_CELLRESISTANCE_FACTOR 0.01f

#define CAN_MSG_BMS_DATA_RANDOM_SIG_CELLOPENVOLTAGE_FACTOR 0.0001f

#define CAN_MSG_BMS_DATA_RANDOM_SIG_CHECKSUM_FACTOR 1.0f

#define CAN_MSG_BMS_DATA_RANDOM_SIG_CELLBALANCING_FACTOR 1.0f

typedef struct {
    uint8_t CellId; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="ID" */
    float CellVoltage; /* start_bit=15; length=16; factor=0.0001; offset=0.0; unit="V" */
    float CellResistance; /* start_bit=30; length=15; factor=0.01; offset=0.0; unit="mOhms" */
    float CellOpenVoltage; /* start_bit=47; length=16; factor=0.0001; offset=0.0; unit="V" */
    uint8_t Checksum; /* start_bit=63; length=8; factor=1.0; offset=0.0 */
    uint8_t CellBalancing; /* start_bit=31; length=1; factor=1.0; offset=0.0 */
} can_msg_BMS_Data_Random_t;

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
    uint16_t Motor_Temp_L; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="degC" */
    uint16_t Inverter_Temp_L; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="degC" */
    float Aux_Brake_Input_L; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="V" */
    float Throttle_Input_L; /* start_bit=55; length=16; factor=0.001; offset=0.0; unit="V" */
} can_msg_GIL_Inv_Data_2_t;

#define CAN_MSG_GIL_SOFTWARE_ID 34U
#define CAN_MSG_GIL_SOFTWARE_DLC 8U
#define CAN_MSG_GIL_SOFTWARE_CYCLE_TIME_MS 0U

#define CAN_MSG_GIL_SOFTWARE_SIG_SOFTWARE_STATUS_FACTOR 1.0f

typedef struct {
    uint16_t Software_Status; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="enum" */
} can_msg_GIL_Software_t;

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
    uint16_t Motor_Temp_R; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="degC" */
    uint16_t Inverter_Temp_R; /* start_bit=23; length=16; factor=1.0; offset=0.0; unit="degC" */
    float Aux_Input_R; /* start_bit=39; length=16; factor=0.001; offset=0.0; unit="V" */
    float Throttle_Input_R; /* start_bit=55; length=16; factor=0.001; offset=0.0; unit="V" */
} can_msg_GIR_Inv_Data_2_t;

#define CAN_MSG_GIR_SOFTWARE_ID 50U
#define CAN_MSG_GIR_SOFTWARE_DLC 8U
#define CAN_MSG_GIR_SOFTWARE_CYCLE_TIME_MS 0U

#define CAN_MSG_GIR_SOFTWARE_SIG_SOFTWARE_STATUS_FACTOR 1.0f

typedef struct {
    uint16_t Software_Status; /* start_bit=7; length=16; factor=1.0; offset=0.0; unit="enum" */
} can_msg_GIR_Software_t;

#define CAN_MSG_DASH_BUTTONS_ID 208U
#define CAN_MSG_DASH_BUTTONS_DLC 8U
#define CAN_MSG_DASH_BUTTONS_CYCLE_TIME_MS 0U

#define CAN_MSG_DASH_BUTTONS_SIG_APPS_RECAL_FACTOR 1.0f

#define CAN_MSG_DASH_BUTTONS_SIG_LOG_SD_TOGGLE_FACTOR 1.0f

#define CAN_MSG_DASH_BUTTONS_SIG_LOG_LTE_TOGGLE_FACTOR 1.0f

#define CAN_MSG_DASH_BUTTONS_SIG_GPS_SPEED_FACTOR 0.01f

#define CAN_MSG_DASH_BUTTONS_SIG_GPS_COURSE_FACTOR 0.0001f

#define CAN_MSG_DASH_BUTTONS_SIG_GPS_YAW_ANGLE_FACTOR 0.0001f

typedef struct {
    uint8_t APPS_Recal; /* start_bit=7; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Log_SD_Toggle; /* start_bit=15; length=8; factor=1.0; offset=0.0; unit="bool" */
    uint8_t Log_LTE_Toggle; /* start_bit=23; length=8; factor=1.0; offset=0.0; unit="bool" */
    float Gps_speed; /* start_bit=7; length=16; factor=0.01; offset=0.0; unit="m/s" */
    float Gps_course; /* start_bit=23; length=16; factor=0.0001; offset=0.0; unit="rad [UW]" */
    float Gps_yaw_angle; /* start_bit=39; length=16; factor=0.0001; offset=0.0; unit="rad [UW]" */
} can_msg_DASH_Buttons_t;

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
