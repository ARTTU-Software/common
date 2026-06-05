#ifndef INC_CAN_BOARD_DEFS_H
#define INC_CAN_BOARD_DEFS_H

#include "can_signal_defs.h"

/* Generated from MAIN_DBC.dbc. Do not edit by hand. */

typedef struct {
    can_msg_GF_Wheel_Sensors_t GF_Wheel_Sensors;
    can_msg_GF_Misc_Sensors_t GF_Misc_Sensors;
    can_msg_GF_Software_Status_t GF_Software_Status;
    can_msg_GF_Calibration_Values_t GF_Calibration_Values;
} can_board_Gateway_Front_t;

typedef struct {
    can_msg_GR_Wheel_Sensors_t GR_Wheel_Sensors;
    can_msg_GR_Misc_Sensors_t GR_Misc_Sensors;
    can_msg_GR_Liquid_Sensors_t GR_Liquid_Sensors;
    can_msg_GR_Software_Status_t GR_Software_Status;
} can_board_Gateway_Rear_t;

typedef struct {
    can_msg_ECU_Inverter_Sig_t ECU_Inverter_Sig;
    can_msg_ECU_Rear_NMOS_t ECU_Rear_NMOS;
    can_msg_ECU_Front_NMOS_t ECU_Front_NMOS;
    can_msg_ECU_Software_Status_Main_t ECU_Software_Status_Main;
    can_msg_ECU_Software_Status_R2D_t ECU_Software_Status_R2D;
    can_msg_ECU_Software_Status_CTRL_t ECU_Software_Status_CTRL;
    can_msg_ECU_UKF_Output_1_t ECU_UKF_Output_1;
    can_msg_ECU_UKF_Output_2_t ECU_UKF_Output_2;
} can_board_ECU_t;

typedef struct {
    can_msg_BMS_Errors_Temps_t BMS_Errors_Temps;
    can_msg_BMS_SoC_Voltage_t BMS_SoC_Voltage;
    can_msg_BMS_Cell_Open_t BMS_Cell_Open;
    can_msg_BMS_Cell_Voltage_t BMS_Cell_Voltage;
    can_msg_BMS_Cell_Resistance_t BMS_Cell_Resistance;
    can_msg_BMS_Current_t BMS_Current;
} can_board_BMS_t;

typedef struct {
    can_msg_GIL_Inv_Data_1_t GIL_Inv_Data_1;
    can_msg_GIL_Inv_Data_2_t GIL_Inv_Data_2;
    can_msg_GIL_Software_Status_t GIL_Software_Status;
} can_board_GI_Left_t;

typedef struct {
    can_msg_GIR_Inv_Data_1_t GIR_Inv_Data_1;
    can_msg_GIR_Inv_Data_2_t GIR_Inv_Data_2;
    can_msg_GIR_Software_Status_t GIR_Software_Status;
} can_board_GI_Right_t;

typedef struct {
    can_msg_DASH_Buttons_t DASH_Buttons;
} can_board_Dashboard_t;

typedef struct {
    can_msg_GPS_Angles_t GPS_Angles;
} can_board_Telemetry_t;

typedef struct {
    can_msg_IMU_Data_t IMU_Data;
    can_msg_IMU_Recal_t IMU_Recal;
} can_board_IMU_t;

typedef struct {
    can_msg_LVSOC_Status_1_t LVSOC_Status_1;
    can_msg_LVSOC_Status_2_t LVSOC_Status_2;
} can_board_LV_SOC_t;

typedef struct {
    can_msg_PRG_Bootloader_t PRG_Bootloader;
    can_msg_PRG_Inverter_t PRG_Inverter;
} can_board_Programming_t;

#endif /* INC_CAN_BOARD_DEFS_H */
