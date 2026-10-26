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
    can_msg_BM_Battery_Status_t BM_Battery_Status;
    can_msg_BM_Charger_Status_t BM_Charger_Status;
    can_msg_BM_Vehicle_Status_t BM_Vehicle_Status;
    can_msg_BM_Inverter_Command_t BM_Inverter_Command;
    can_msg_BM_Rear_Buzzer_t BM_Rear_Buzzer;
    can_msg_BM_TC_Command_t BM_TC_Command;
    can_msg_BM_Settings_Reply_t BM_Settings_Reply;
} can_board_BMS_Master_t;

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
    can_msg_DASH_Inputs_t DASH_Inputs;
    can_msg_DASH_PID_Tune_1_t DASH_PID_Tune_1;
    can_msg_DASH_PID_Tune_2_t DASH_PID_Tune_2;
} can_board_Dashboard_t;

typedef struct {
    can_msg_GPS_Angles_t GPS_Angles;
    can_msg_Lap_Completed_t Lap_Completed;
} can_board_Telemetry_t;

typedef struct {
    can_msg_SBG_ECAN_MSG_STATUS_03_t SBG_ECAN_MSG_STATUS_03;
    can_msg_SBG_ECAN_MSG_UTC_1_t SBG_ECAN_MSG_UTC_1;
    can_msg_SBG_ECAN_MSG_IMU_ACCEL_t SBG_ECAN_MSG_IMU_ACCEL;
    can_msg_SBG_ECAN_MSG_IMU_GYRO_t SBG_ECAN_MSG_IMU_GYRO;
    can_msg_SBG_ECAN_MSG_EKF_EULER_t SBG_ECAN_MSG_EKF_EULER;
    can_msg_SBG_ECAN_MSG_EKF_POS_t SBG_ECAN_MSG_EKF_POS;
    can_msg_SBG_ECAN_MSG_EKF_ALTITUDE_t SBG_ECAN_MSG_EKF_ALTITUDE;
    can_msg_SBG_ECAN_MSG_EKF_VEL_NED_t SBG_ECAN_MSG_EKF_VEL_NED;
    can_msg_SBG_ECAN_MSG_EKF_VEL_BODY_t SBG_ECAN_MSG_EKF_VEL_BODY;
    can_msg_SBG_ECAN_MSG_GPS1_POS_INFO_t SBG_ECAN_MSG_GPS1_POS_INFO;
    can_msg_SBG_ECAN_MSG_GPS1_POS_ALT_t SBG_ECAN_MSG_GPS1_POS_ALT;
} can_board_INS_t;

typedef struct {
    can_msg_PRG_Bootloader_t PRG_Bootloader;
    can_msg_PRG_Inverter_t PRG_Inverter;
    can_msg_BM_Settings_Request_t BM_Settings_Request;
} can_board_Programming_t;

typedef struct {
    can_msg_TIC_Switch_SDC_t TIC_Switch_SDC;
    can_msg_TC_Charger_Feedback_t TC_Charger_Feedback;
} can_board_Charger_t;

#endif /* INC_CAN_BOARD_DEFS_H */
