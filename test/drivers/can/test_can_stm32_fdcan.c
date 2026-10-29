#include "unity.h"
#include "can_stm32_fdcan.h"
#include "can_fdcan_test_hal.h"

#include <string.h>

TEST_SOURCE_FILE("can_driver_core.c");
TEST_SOURCE_FILE("can_stm32_fdcan.c");

static FDCAN_HandleTypeDef handle;
static can_driver_t bus;
static can_driver_rx_frame_t storage[3];
static FDCAN_TxHeaderTypeDef submitted_header;
static uint8_t submitted_data[8];
static uint32_t tx_free;
static uint32_t submitted;
static HAL_StatusTypeDef tx_status;
static HAL_StatusTypeDef rx_status;
static FDCAN_RxHeaderTypeDef rx_headers[4];
static uint8_t rx_payload[4][64];
static uint32_t rx_count;
static uint32_t rx_index;

uint32_t HAL_FDCAN_GetTxFifoFreeLevel(FDCAN_HandleTypeDef *instance)
{
    TEST_ASSERT_EQUAL_PTR(&handle, instance);
    return tx_free;
}

HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *instance,
    const FDCAN_TxHeaderTypeDef *header, const uint8_t *data)
{
    TEST_ASSERT_EQUAL_PTR(&handle, instance);
    submitted_header = *header;
    memcpy(submitted_data, data, sizeof(submitted_data));
    submitted++;
    return tx_status;
}

uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *instance, uint32_t fifo)
{
    TEST_ASSERT_EQUAL_PTR(&handle, instance);
    TEST_ASSERT_TRUE(fifo == FDCAN_RX_FIFO0 || fifo == FDCAN_RX_FIFO1);
    return rx_count - rx_index;
}

HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *instance,
    uint32_t fifo, FDCAN_RxHeaderTypeDef *header, uint8_t *data)
{
    (void)fifo;
    TEST_ASSERT_EQUAL_PTR(&handle, instance);
    if (rx_status != HAL_OK) {
        return rx_status;
    }
    *header = rx_headers[rx_index];
    /* Exercise the adapter's scratch capacity even for a rejected FD frame. */
    memcpy(data, rx_payload[rx_index], 64U);
    rx_index++;
    return HAL_OK;
}

void setUp(void)
{
    memset(&bus, 0, sizeof(bus));
    memset(rx_headers, 0, sizeof(rx_headers));
    memset(rx_payload, 0x5a, sizeof(rx_payload));
    tx_free = 3U;
    submitted = 0U;
    tx_status = HAL_OK;
    rx_status = HAL_OK;
    rx_count = 0U;
    rx_index = 0U;
    const can_driver_config_t config = {
        .submit = can_stm32_fdcan_submit, .context = &handle,
        .rx_storage = storage, .rx_capacity = 3U
    };
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK, can_driver_init(&bus, &config, 0U));
    for (uint32_t i = 0U; i < 4U; i++) {
        rx_headers[i].Identifier = 0x123U + i;
        rx_headers[i].DataLength = FDCAN_DLC_BYTES_8;
    }
}

void tearDown(void) {}

void test_adapter_builds_classic_header_using_vendor_dlc(void)
{
    const can_driver_frame_t frame = {.id = 0x10U, .length = 8U, .data = {0x42U}};
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK, can_driver_send_now(&bus, &frame));
    TEST_ASSERT_EQUAL_UINT32(1U, submitted);
    TEST_ASSERT_EQUAL_HEX32(FDCAN_DLC_BYTES_8, submitted_header.DataLength);
    TEST_ASSERT_EQUAL_HEX32(0x10U, submitted_header.Identifier);
    TEST_ASSERT_EQUAL(FDCAN_STANDARD_ID, submitted_header.IdType);
    TEST_ASSERT_EQUAL(FDCAN_CLASSIC_CAN, submitted_header.FDFormat);
    TEST_ASSERT_EQUAL_UINT8(0x42U, submitted_data[0]);
}

void test_adapter_supports_extended_id_and_zero_length(void)
{
    const can_driver_frame_t frame = {.id = 0x18ff50e5U, .extended = true};
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK, can_driver_send_now(&bus, &frame));
    TEST_ASSERT_EQUAL(FDCAN_EXTENDED_ID, submitted_header.IdType);
    TEST_ASSERT_EQUAL_HEX32(FDCAN_DLC_BYTES_0, submitted_header.DataLength);
}

void test_adapter_full_fifo_does_not_attempt_submission(void)
{
    const can_driver_frame_t frame = {.id = 1U};
    tx_free = 0U;
    TEST_ASSERT_EQUAL(CAN_DRIVER_BUSY, can_driver_send_now(&bus, &frame));
    TEST_ASSERT_EQUAL_UINT32(0U, submitted);
}

void test_adapter_distinguishes_hal_busy_and_failure(void)
{
    const can_driver_frame_t frame = {.id = 1U};
    tx_status = HAL_BUSY;
    TEST_ASSERT_EQUAL(CAN_DRIVER_BUSY, can_driver_send_now(&bus, &frame));
    tx_status = HAL_ERROR;
    TEST_ASSERT_EQUAL(CAN_DRIVER_IO_ERROR, can_driver_send_now(&bus, &frame));
}

void test_adapter_rx_converts_dlc_and_obeys_batch_budget(void)
{
    rx_count = 3U;
    rx_headers[0].IdType = FDCAN_EXTENDED_ID;
    rx_headers[0].Identifier = 0x18ff50e5U;
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK,
        can_stm32_fdcan_receive_isr(&bus, &handle, FDCAN_RX_FIFO1, 321U, 2U));
    TEST_ASSERT_EQUAL_UINT32(2U, rx_index);
    can_driver_rx_frame_t frame;
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK, can_driver_receive(&bus, &frame));
    TEST_ASSERT_TRUE(frame.frame.extended);
    TEST_ASSERT_EQUAL_UINT8(8U, frame.frame.length);
    TEST_ASSERT_EQUAL_UINT32(321U, frame.received_ms);
    TEST_ASSERT_EQUAL_UINT8(0x5aU, frame.frame.data[7]);
}

void test_adapter_drops_fd_remote_and_invalid_dlc_without_copying_to_core(void)
{
    rx_count = 3U;
    rx_headers[0].FDFormat = FDCAN_FD_CAN;
    rx_headers[1].RxFrameType = FDCAN_REMOTE_FRAME;
    rx_headers[2].DataLength = 15U << 16U;
    TEST_ASSERT_EQUAL(CAN_DRIVER_INVALID,
        can_stm32_fdcan_receive_isr(&bus, &handle, FDCAN_RX_FIFO0, 0U, 3U));
    can_driver_rx_frame_t frame;
    TEST_ASSERT_EQUAL(CAN_DRIVER_EMPTY, can_driver_receive(&bus, &frame));
}

void test_adapter_software_overflow_keeps_oldest_and_remains_bounded(void)
{
    rx_count = 4U;
    TEST_ASSERT_EQUAL(CAN_DRIVER_BUSY,
        can_stm32_fdcan_receive_isr(&bus, &handle, FDCAN_RX_FIFO0, 0U, 3U));
    TEST_ASSERT_EQUAL_UINT32(3U, rx_index);
    can_driver_rx_frame_t frame;
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK, can_driver_receive(&bus, &frame));
    TEST_ASSERT_EQUAL_HEX32(0x123U, frame.frame.id);
    can_driver_stats_t stats;
    TEST_ASSERT_EQUAL(CAN_DRIVER_OK, can_driver_get_stats(&bus, &stats));
    TEST_ASSERT_EQUAL_UINT32(1U, stats.rx_dropped);
}

void test_adapter_reports_rx_hardware_failure_and_rejects_zero_budget(void)
{
    rx_count = 1U;
    rx_status = HAL_ERROR;
    TEST_ASSERT_EQUAL(CAN_DRIVER_IO_ERROR,
        can_stm32_fdcan_receive_isr(&bus, &handle, FDCAN_RX_FIFO0, 0U, 1U));
    TEST_ASSERT_EQUAL(CAN_DRIVER_INVALID,
        can_stm32_fdcan_receive_isr(&bus, &handle, FDCAN_RX_FIFO0, 0U, 0U));
}
