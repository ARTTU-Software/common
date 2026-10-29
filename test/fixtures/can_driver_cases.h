#ifndef INC_CAN_DRIVER_CASES_H
#define INC_CAN_DRIVER_CASES_H

#include <stdint.h>

#define CAN_DRIVER_CASE_COUNT 13U

/* Shared host/ARM scenarios. Zero passes; a nonzero result is the failing line. */
uint32_t can_driver_run_case(uint32_t number);

#endif /* INC_CAN_DRIVER_CASES_H */
