#include "can_driver_cases.h"

/* Renode selects a case before entering main directly. No board startup, HAL,
 * vector table or linker script is needed for this portable-core fixture. */
volatile uint32_t can_test_case;
volatile uint32_t can_test_done;
volatile uint32_t can_test_result;
volatile uint32_t can_test_progress;

int main(void)
{
    can_test_done = 0U;
    can_test_progress = 0U;
    can_test_result = can_driver_run_case(can_test_case);
    can_test_progress = 1U;
    can_test_done = 1U;
    for (;;) {
        /* Terminal test state, inspected with a bounded virtual-time assertion. */
    }
}
