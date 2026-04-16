#include "unity.h"
#include "generic_can_driver.h"
#include <string.h>

TEST_SOURCE_FILE("generic_can_driver.c");

static CAN_Driver_t test_driver;
static CAN_Tx_Message_Frame_t tx_message_frames[2];
static CAN_Tx_Message_Frame_t tx_queue_storage[4];
static CAN_Rx_Message_Frame_t rx_queue_storage[2];

static uint32_t fifo_add_call_count;
static uint8_t fifo_should_fail;

static uint32_t test_add_to_fifo(void *hfdcan, void *hdr, uint8_t *payload)
{
    (void)hfdcan;
    (void)hdr;
    (void)payload;

    fifo_add_call_count++;

    if (fifo_should_fail != 0U) {
        return 1U;
    }

    return 0U;
}

static void init_driver(void)
{
    memset(&test_driver, 0, sizeof(test_driver));
    memset(&tx_message_frames, 0, sizeof(tx_message_frames));
    memset(&tx_queue_storage, 0, sizeof(tx_queue_storage));
    memset(&rx_queue_storage, 0, sizeof(rx_queue_storage));

    test_driver.tx_message_frames = tx_message_frames;
    test_driver.tx_frame_number = 2U;
    test_driver.rx_frame_number = 2U;

    test_driver.tx_ring_buffer.frame = tx_queue_storage;
    test_driver.tx_ring_buffer.size = 4U;

    test_driver.rx_ring_buffer.frame = rx_queue_storage;

    CAN_set_structures(&test_driver, test_add_to_fifo, (void*)0x1);
}

static CAN_Tx_Message_Frame_t make_frame(uint32_t msg_id, uint8_t payload0)
{
    CAN_Tx_Message_Frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.msg_id = msg_id;
    frame.payload[0] = payload0;
    frame.num_values = 8U;
    return frame;
}

void setUp(void)
{
    fifo_add_call_count = 0U;
    fifo_should_fail = 0U;
    init_driver();
}

void tearDown(void)
{
}

void test_CAN_send_single_frame_should_queue_frame_and_request_drain(void)
{
    CAN_Tx_Message_Frame_t frame = make_frame(0x101U, 0xAAU);
    uint32_t result = CAN_send_single_frame(&test_driver, &frame);

    TEST_ASSERT_EQUAL_UINT32(0U, result);
    TEST_ASSERT_EQUAL_UINT16(1U, test_driver.tx_ring_buffer.count);
    TEST_ASSERT_EQUAL_UINT16(1U, test_driver.tx_ring_buffer.head);
    TEST_ASSERT_EQUAL_UINT16(0U, test_driver.tx_ring_buffer.tail);
    TEST_ASSERT_EQUAL_UINT8(1U, test_driver.tx_queue_drain_requested);
    TEST_ASSERT_EQUAL_UINT32(0x101U, test_driver.tx_ring_buffer.frame[0].msg_id);
    TEST_ASSERT_EQUAL_UINT8(0xAAU, test_driver.tx_ring_buffer.frame[0].payload[0]);
}

void test_CAN_send_single_frame_should_return_error_when_tx_buffer_full(void)
{
    CAN_Tx_Message_Frame_t frame1 = make_frame(0x111U, 0x11U);
    CAN_Tx_Message_Frame_t frame2 = make_frame(0x222U, 0x22U);

    test_driver.tx_ring_buffer.size = 1U;

    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &frame1));
    TEST_ASSERT_EQUAL_UINT32(1U, CAN_send_single_frame(&test_driver, &frame2));

    TEST_ASSERT_EQUAL_UINT16(1U, test_driver.tx_ring_buffer.count);
    TEST_ASSERT_EQUAL_UINT32(0x111U, test_driver.tx_ring_buffer.frame[0].msg_id);
}

void test_CAN_send_single_frame_should_replace_existing_frame_with_same_id(void)
{
    CAN_Tx_Message_Frame_t original = make_frame(0x321U, 0x01U);
    CAN_Tx_Message_Frame_t updated = make_frame(0x321U, 0xFEU);

    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &original));
    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &updated));

    TEST_ASSERT_EQUAL_UINT16(1U, test_driver.tx_ring_buffer.count);
    TEST_ASSERT_EQUAL_UINT16(1U, test_driver.tx_ring_buffer.head);
    TEST_ASSERT_EQUAL_UINT16(0U, test_driver.tx_ring_buffer.tail);
    TEST_ASSERT_EQUAL_UINT8(0xFEU, test_driver.tx_ring_buffer.frame[0].payload[0]);
}

void test_CAN_process_tx_queue_should_drain_all_when_amount_is_zero(void)
{
    CAN_Tx_Message_Frame_t frame1 = make_frame(0x201U, 0x10U);
    CAN_Tx_Message_Frame_t frame2 = make_frame(0x202U, 0x20U);

    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &frame1));
    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &frame2));

    TEST_ASSERT_EQUAL_UINT16(2U, test_driver.tx_ring_buffer.count);

    TEST_ASSERT_EQUAL_UINT16(2U, CAN_process_tx_queue(&test_driver, 0U));

    TEST_ASSERT_EQUAL_UINT32(2U, fifo_add_call_count);
    TEST_ASSERT_EQUAL_UINT16(0U, test_driver.tx_ring_buffer.count);
    TEST_ASSERT_EQUAL_UINT8(0U, test_driver.tx_queue_drain_requested);
}

void test_CAN_process_tx_queue_should_keep_drain_requested_when_fifo_add_fails(void)
{
    CAN_Tx_Message_Frame_t frame1 = make_frame(0x401U, 0x44U);
    CAN_Tx_Message_Frame_t frame2 = make_frame(0x402U, 0x55U);

    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &frame1));
    TEST_ASSERT_EQUAL_UINT32(0U, CAN_send_single_frame(&test_driver, &frame2));

    fifo_should_fail = 1U;

    TEST_ASSERT_EQUAL_UINT16(0U, CAN_process_tx_queue(&test_driver, 0U));

    TEST_ASSERT_EQUAL_UINT32(1U, fifo_add_call_count);
    TEST_ASSERT_EQUAL_UINT16(2U, test_driver.tx_ring_buffer.count);
    TEST_ASSERT_EQUAL_UINT8(1U, test_driver.tx_queue_drain_requested);
}

void test_CAN_send_frames_should_enqueue_periodic_message_and_request_drain(void)
{
    tx_message_frames[0] = make_frame(0x777U, 0x77U);
    tx_message_frames[0].scheduler_timer_value = 10U;
    tx_message_frames[0].previous_tick = 0U;

    tx_message_frames[1] = make_frame(0x778U, 0x78U);
    tx_message_frames[1].scheduler_timer_value = CAN_DRIVER_NON_PERIODIC_FRAME;

    CAN_send_frames(&test_driver, 10U);

    TEST_ASSERT_EQUAL_UINT16(1U, test_driver.tx_ring_buffer.count);
    TEST_ASSERT_EQUAL_UINT32(0x777U, test_driver.tx_ring_buffer.frame[0].msg_id);
    TEST_ASSERT_EQUAL_UINT8(1U, test_driver.tx_queue_drain_requested);
}
