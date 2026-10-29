#include "can_driver_cases.h"
#include "can_driver_core.h"

#include <stddef.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { return __LINE__; } } while (0)

_Static_assert(HZ(50U) == 20U, "Hz must work in constant configuration");
_Static_assert(HZ_TO_MS(1000U) == 1U, "One millisecond resolution");
_Static_assert(CAN_HZ_TO_MS(0U) == 0U, "Zero Hz must not divide by zero");
_Static_assert(CAN_HZ_TO_MS(1001U) == 0U, "Reject rates above millisecond resolution");
_Static_assert(CAN_COMBINE_32(0x89U, 0xabU, 0xcdU, 0xefU) == 0x89abcdefU,
    "Byte combining must remain constant and unsigned");

typedef struct {
    can_driver_frame_t sent[16];
    uint16_t used;
    uint16_t capacity;
    can_driver_status_t forced_status;
    can_driver_t *nested_bus;
} test_port_t;

static can_driver_frame_t test_frame(uint32_t id, uint8_t value)
{
    can_driver_frame_t frame = {.id = id, .length = 8U};
    frame.data[0] = value;
    return frame;
}

static can_driver_status_t test_submit(void *context, const can_driver_frame_t *frame)
{
    test_port_t *port = context;
    if (port->forced_status != CAN_DRIVER_OK) {
        return port->forced_status;
    }
    if (port->used >= port->capacity) {
        return CAN_DRIVER_BUSY;
    }
    if (port->nested_bus != NULL) {
        const can_driver_status_t status = can_driver_send_now(port->nested_bus, frame);
        if (status != CAN_DRIVER_OK) {
            return status;
        }
    }
    port->sent[port->used++] = *frame;
    return CAN_DRIVER_OK;
}

static can_driver_config_t test_config(test_port_t *port)
{
    memset(port, 0, sizeof(*port));
    port->capacity = 16U;
    return (can_driver_config_t){.submit = test_submit, .context = port};
}

static uint32_t test_immediate(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    const can_driver_config_t config = test_config(&port);
    can_driver_frame_t frame = test_frame(0x10U, 0x11U);
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_OK);
    CHECK(port.used == 1U && port.sent[0].data[0] == 0x11U);
    frame.data[0] = 0x22U;
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_OK);
    CHECK(port.used == 2U && port.sent[1].data[0] == 0x22U);
    return 0U;
}

static uint32_t test_immediate_backpressure(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    const can_driver_config_t config = test_config(&port);
    const can_driver_frame_t frame = test_frame(0x16U, 1U);
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    port.capacity = 0U;
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_BUSY);
    port.capacity = 16U;
    CHECK(can_driver_send_periodic(&bus, 1000U) == CAN_DRIVER_OK);
    CHECK(port.used == 0U); /* A refused status is never secretly replayed. */
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_OK);
    port.forced_status = CAN_DRIVER_IO_ERROR;
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_IO_ERROR);
    can_driver_stats_t stats;
    CHECK(can_driver_get_stats(&bus, &stats) == CAN_DRIVER_OK);
    CHECK(stats.tx_submitted == 1U && stats.tx_busy == 1U && stats.tx_errors == 1U);
    return 0U;
}

static uint32_t test_periodic_phase(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic = {.frame = test_frame(0x180U, 1U), .period_ms = 100U};
    config.periodic = &periodic;
    config.periodic_count = 1U;
    CHECK(can_driver_init(&bus, &config, 50U) == CAN_DRIVER_OK);
    CHECK(can_driver_send_periodic(&bus, 149U) == CAN_DRIVER_OK && port.used == 0U);
    CHECK(can_driver_send_periodic(&bus, 150U) == CAN_DRIVER_OK && port.used == 1U);
    CHECK(can_driver_send_periodic(&bus, 399U) == CAN_DRIVER_OK && port.used == 2U);
    CHECK(can_driver_send_periodic(&bus, 399U) == CAN_DRIVER_OK && port.used == 2U);
    CHECK(can_driver_send_periodic(&bus, 450U) == CAN_DRIVER_OK && port.used == 3U);
    return 0U;
}

static uint32_t test_periodic_latest_after_busy(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic = {.frame = test_frame(0x180U, 1U), .period_ms = 100U};
    config.periodic = &periodic;
    config.periodic_count = 1U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    port.capacity = 0U;
    CHECK(can_driver_send_periodic(&bus, 100U) == CAN_DRIVER_BUSY);
    can_driver_frame_t latest = test_frame(0x180U, 0U); /* BMS-like STOP replaces demand. */
    CHECK(can_driver_update_periodic(&bus, 0U, &latest) == CAN_DRIVER_OK);
    port.capacity = 16U;
    CHECK(can_driver_send_periodic(&bus, 105U) == CAN_DRIVER_OK);
    CHECK(port.used == 1U && port.sent[0].data[0] == 0U);
    CHECK(can_driver_send_periodic(&bus, 199U) == CAN_DRIVER_OK && port.used == 1U);
    CHECK(can_driver_send_periodic(&bus, 200U) == CAN_DRIVER_OK && port.used == 2U);
    latest.id++;
    CHECK(can_driver_update_periodic(&bus, 0U, &latest) == CAN_DRIVER_INVALID);
    return 0U;
}

static uint32_t test_periodic_budget(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic[3] = {
        {.frame = test_frame(0x10U, 1U), .period_ms = 10U},
        {.frame = test_frame(0x16U, 2U), .period_ms = 10U},
        {.frame = test_frame(0xe5U, 3U), .period_ms = 10U}
    };
    config.periodic = periodic;
    config.periodic_count = 3U;
    config.tx_budget = 1U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    CHECK(can_driver_send_periodic(&bus, 10U) == CAN_DRIVER_OK && port.used == 1U);
    CHECK(can_driver_send_periodic(&bus, 20U) == CAN_DRIVER_OK && port.used == 2U);
    CHECK(can_driver_send_periodic(&bus, 30U) == CAN_DRIVER_OK && port.used == 3U);
    CHECK(port.sent[0].id == 0x10U && port.sent[1].id == 0x16U && port.sent[2].id == 0xe5U);
    return 0U;
}

static uint32_t test_tick_wrap(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic = {.frame = test_frame(0x123U, 1U), .period_ms = 10U};
    config.periodic = &periodic;
    config.periodic_count = 1U;
    CHECK(can_driver_init(&bus, &config, UINT32_MAX - 5U) == CAN_DRIVER_OK);
    CHECK(can_driver_send_periodic(&bus, 3U) == CAN_DRIVER_OK && port.used == 0U);
    CHECK(can_driver_send_periodic(&bus, 4U) == CAN_DRIVER_OK && port.used == 1U);
    CHECK(can_driver_send_periodic(&bus, 14U) == CAN_DRIVER_OK && port.used == 2U);
    return 0U;
}

static uint32_t test_independent_buses(void)
{
    can_driver_t first = {0};
    can_driver_t second = {0};
    test_port_t port1;
    test_port_t port2;
    const can_driver_config_t config1 = test_config(&port1);
    const can_driver_config_t config2 = test_config(&port2);
    CHECK(can_driver_init(&first, &config1, 0U) == CAN_DRIVER_OK);
    CHECK(can_driver_init(&second, &config2, 0U) == CAN_DRIVER_OK);
    port1.nested_bus = &second;
    const can_driver_frame_t frame = test_frame(0x123U, 9U);
    CHECK(can_driver_send_now(&first, &frame) == CAN_DRIVER_OK);
    CHECK(port1.used == 1U && port2.used == 1U);
    return 0U;
}

static uint32_t test_receive_copy_overflow_wrap(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_rx_frame_t storage[3];
    config.rx_storage = storage;
    config.rx_capacity = 3U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    can_driver_frame_t frame = test_frame(0x18ff50e5U, 1U);
    frame.extended = true;
    CHECK(can_driver_receive_isr(&bus, &frame, 100U) == CAN_DRIVER_OK);
    frame.data[0] = 2U;
    CHECK(can_driver_receive_isr(&bus, &frame, 110U) == CAN_DRIVER_OK);
    CHECK(can_driver_receive_isr(&bus, &frame, 120U) == CAN_DRIVER_BUSY);
    can_driver_rx_frame_t received = {0};
    CHECK(can_driver_receive(&bus, &received) == CAN_DRIVER_OK);
    CHECK(received.frame.data[0] == 1U && received.received_ms == 100U && received.frame.extended);
    CHECK(can_driver_receive_isr(&bus, &frame, 130U) == CAN_DRIVER_OK);
    CHECK(can_driver_receive(&bus, &received) == CAN_DRIVER_OK && received.received_ms == 110U);
    CHECK(can_driver_receive(&bus, &received) == CAN_DRIVER_OK && received.received_ms == 130U);
    CHECK(can_driver_receive(&bus, &received) == CAN_DRIVER_EMPTY && received.received_ms == 130U);
    can_driver_stats_t stats;
    CHECK(can_driver_get_stats(&bus, &stats) == CAN_DRIVER_OK);
    CHECK(stats.rx_received == 3U && stats.rx_dropped == 1U);
    return 0U;
}

static uint32_t test_invalid_frames(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_rx_frame_t storage[2];
    config.rx_storage = storage;
    config.rx_capacity = 2U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    can_driver_frame_t frame = test_frame(0x800U, 0U);
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_INVALID);
    CHECK(can_driver_receive_isr(&bus, &frame, 0U) == CAN_DRIVER_INVALID);
    frame.id = 1U;
    frame.length = 9U;
    CHECK(can_driver_receive_isr(&bus, &frame, 0U) == CAN_DRIVER_INVALID);
    CHECK(can_driver_receive_isr(&bus, NULL, 0U) == CAN_DRIVER_INVALID);
    frame.length = 0U;
    CHECK(can_driver_send_now(&bus, &frame) == CAN_DRIVER_OK);
    can_driver_stats_t stats;
    CHECK(can_driver_get_stats(&bus, &stats) == CAN_DRIVER_OK && stats.rx_invalid == 3U);
    return 0U;
}

static uint32_t test_invalid_configuration(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic = {.frame = test_frame(1U, 0U)};
    config.periodic = &periodic;
    config.periodic_count = 1U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_INVALID);
    CHECK(can_driver_send_now(&bus, &periodic.frame) == CAN_DRIVER_NOT_INITIALIZED);
    periodic.period_ms = 0x80000000U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_INVALID);
    periodic.period_ms = 100U;
    config.rx_capacity = 2U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_INVALID);
    config.rx_capacity = 0U;
    config.submit = NULL;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_INVALID);
    CHECK(can_driver_init(NULL, &config, 0U) == CAN_DRIVER_INVALID);
    return 0U;
}

static uint32_t test_standard_extended_identity(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic[2] = {
        {.frame = test_frame(0x123U, 1U), .period_ms = 10U},
        {.frame = test_frame(0x123U, 2U), .period_ms = 10U}
    };
    config.periodic = periodic;
    config.periodic_count = 2U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_INVALID);
    periodic[1].frame.extended = true;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    CHECK(can_driver_send_periodic(&bus, 10U) == CAN_DRIVER_OK && port.used == 2U);
    CHECK(!port.sent[0].extended && port.sent[1].extended);
    return 0U;
}

static uint32_t test_event_before_due_periodic(void)
{
    can_driver_t bus = {0};
    test_port_t port;
    can_driver_config_t config = test_config(&port);
    can_driver_periodic_t periodic = {.frame = test_frame(0xe0U, 20U), .period_ms = 10U};
    config.periodic = &periodic;
    config.periodic_count = 1U;
    CHECK(can_driver_init(&bus, &config, 0U) == CAN_DRIVER_OK);
    const can_driver_frame_t status = test_frame(0x10U, 0xffU);
    CHECK(can_driver_send_now(&bus, &status) == CAN_DRIVER_OK);
    CHECK(can_driver_send_periodic(&bus, 10U) == CAN_DRIVER_OK);
    CHECK(port.used == 2U && port.sent[0].id == 0x10U && port.sent[1].id == 0xe0U);
    return 0U;
}

static uint32_t test_byte_and_frequency_helpers(void)
{
    /* Volatile inputs also exercise shifts/conversion in the ARM fixture. */
    volatile uint32_t word = 0x89abcdefU;
    const uint16_t half = (uint16_t)word;
    const uint8_t byte = (uint8_t)word;
    CHECK(can_u8_low_byte(byte) == 0xefU && can_u8_high_byte(byte) == 0U);
    CHECK(can_u8_high_nibble(byte) == 0x0eU && can_u8_low_nibble(byte) == 0x0fU);
    CHECK(can_u16_high_byte(half) == 0xcdU && can_u16_low_byte(half) == 0xefU);
    CHECK(can_u32_high_byte(word) == 0x89U && can_u32_low_byte(word) == 0xefU);
    CHECK(can_u32_byte(word, 1U) == 0xcdU && can_u32_byte(word, 2U) == 0xabU);
    CHECK(can_u32_byte(word, 0U) == 0xefU && can_u32_byte(word, 3U) == 0x89U);
    CHECK(can_u32_byte(word, 4U) == 0U && can_u32_byte(word, UINT8_MAX) == 0U);
    CHECK(CAN_COMBINE_16(can_u16_high_byte(half), can_u16_low_byte(half)) == half);
    CHECK(CAN_COMBINE_32(can_u32_byte(word, 3U), can_u32_byte(word, 2U),
        can_u32_byte(word, 1U), can_u32_byte(word, 0U)) == word);
    CHECK(HIGH_BYTE(word) == 0xcdU && LOW_BYTE(word) == 0xefU);
    CHECK(CAN_COMBINE_32(0xffU, 0xffU, 0xffU, 0xffU) == UINT32_MAX);
    uint32_t rate = 50U;
    CHECK(can_hz_to_ms(rate++) == 20U && rate == 51U);
    CHECK(can_hz_to_ms(1U) == 1000U && can_hz_to_ms(1000U) == 1U);
    CHECK(can_hz_to_ms(60U) == 16U);
    CHECK(can_hz_to_ms(0U) == 0U && can_hz_to_ms(1001U) == 0U);
    CHECK(can_hz_to_ms(UINT32_MAX) == 0U);
    return 0U;
}

uint32_t can_driver_run_case(uint32_t number)
{
    switch (number) {
        case 1U: return test_immediate();
        case 2U: return test_immediate_backpressure();
        case 3U: return test_periodic_phase();
        case 4U: return test_periodic_latest_after_busy();
        case 5U: return test_periodic_budget();
        case 6U: return test_tick_wrap();
        case 7U: return test_independent_buses();
        case 8U: return test_receive_copy_overflow_wrap();
        case 9U: return test_invalid_frames();
        case 10U: return test_invalid_configuration();
        case 11U: return test_standard_extended_identity();
        case 12U: return test_event_before_due_periodic();
        case 13U: return test_byte_and_frequency_helpers();
        default: return UINT32_MAX;
    }
}
