#ifndef INC_CAN_BOARD_DEFS_H
#define INC_CAN_BOARD_DEFS_H

#include "can_signal_defs.h"

/* Generated from MAIN_DBC.dbc. Do not edit by hand. */

typedef struct {
    can_msg_GF_Wheel_Sensors_t GF_Wheel_Sensors;
    can_msg_GF_Misc_Sensors_t GF_Misc_Sensors;
} can_board_Gateway_Front_t;

typedef struct {
    can_msg_GR_Wheel_Sensors_t GR_Wheel_Sensors;
    can_msg_GR_Misc_Sensors_t GR_Misc_Sensors;
    can_msg_GR_Liquid_Sensors_t GR_Liquid_Sensors;
} can_board_Gateway_Rear_t;

typedef struct {
    can_msg_ECU_Inverter_Sig_t ECU_Inverter_Sig;
    can_msg_ECU_Software_Data_t ECU_Software_Data;
    can_msg_ECU_Rear_NMOS_t ECU_Rear_NMOS;
    can_msg_ECU_Front_NMOS_t ECU_Front_NMOS;
} can_board_ECU_t;

typedef struct {
    can_msg_BMS_Errors_Temps_t BMS_Errors_Temps;
    can_msg_BMS_SoC_Voltage_t BMS_SoC_Voltage;
    can_msg_BMS_Cell_Data_t BMS_Cell_Data;
    can_msg_BMS_Cell_Data_2_t BMS_Cell_Data_2;
    can_msg_BMS_Current_t BMS_Current;
    can_msg_BMS_Data_Random_t BMS_Data_Random;
} can_board_BMS_t;

typedef struct {
    can_msg_GIL_Inv_Data_1_t GIL_Inv_Data_1;
    can_msg_GIL_Inv_Data_2_t GIL_Inv_Data_2;
    can_msg_GIL_Software_t GIL_Software;
} can_board_GI_Left_t;

typedef struct {
    can_msg_GIR_Inv_Data_1_t GIR_Inv_Data_1;
    can_msg_GIR_Inv_Data_2_t GIR_Inv_Data_2;
    can_msg_GIR_Software_t GIR_Software;
} can_board_GI_Right_t;

typedef struct {
    can_msg_DASH_Buttons_t DASH_Buttons;
} can_board_Dashboard_t;

typedef struct {
    uint8_t reserved;
} can_board_Telemetry_t;

typedef struct {
    can_msg_IMU_Data_t IMU_Data;
    can_msg_IMU_Recal_t IMU_Recal;
} can_board_IMU_t;

typedef struct {
    can_msg_PRG_Bootloader_t PRG_Bootloader;
    can_msg_PRG_Inverter_t PRG_Inverter;
} can_board_Programming_t;

#endif /* INC_CAN_BOARD_DEFS_H */
