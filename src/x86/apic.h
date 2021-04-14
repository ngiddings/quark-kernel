#pragma once

#include <stdint.h>

enum apic_delivery_mode_t
{
    APIC_DELIVERY_MODE_FIXED = 0b000,
    APIC_DELIVERY_MODE_LOWEST_PRIORITY = 0b001,
    APIC_DELIVERY_MODE_SMI = 0b010,
    APIC_DELIVERY_MODE_NMI = 0b100,
    APIC_DELIVERY_MODE_INIT = 0b101,
    APIC_DELIVERY_MODE_SIPI = 0b110,
    APIV_DELIVERY_MODE_EXTINIT = 0b111
};

enum apic_destination_mode_t
{
    APIC_DESTINATION_MODE_PHYSICAL = 0,
    APIC_DESTINATION_MODE_LOGICAL = 1
};

enum apic_level_t
{
    APIC_LEVEL_DEASSERT = 0,
    APIC_LEVEL_ASSERT = 1
};
enum apic_trigger_mode_t
{
    APIC_TRIGGER_MODE_EDGE = 0,
    APIC_TRIGGER_MODE_LEVEL = 1
};

enum apic_destination_shorthand_t
{
    APIC_DEST_SHORTHAND_NONE = 0,
    APIC_DEST_SHORTHAND_SELF = 1,
    APIC_DEST_SHORTHAND_ALL = 2,
    APIC_DEST_SHORTHAND_OTHERS = 3
};

enum apic_divide_mode_t
{
    APIC_DIVIDE_1 = 0b1011,
    APIC_DIVIDE_2 = 0b0000,
    APIC_DIVIDE_4 = 0b0001,
    APIC_DIVIDE_8 = 0b0010,
    APIC_DIVIDE_16 = 0b0011,
    APIC_DIVIDE_32 = 0b1000,
    APIC_DIVIDE_64 = 0b1001,
    APIC_DIVIDE_128 = 0b1011
};

enum apic_timer_mode_t
{
    APIC_TIMER_ONESHOT = 0,
    APIC_TIMER_PERIODIC = 1,
    APIC_TIMER_TSCDEADLINE = 2
};

struct apic_register_t
{
    uint32_t value;
    uint32_t padding[3];
};

struct apic_lapic_version_t
{
    uint32_t version : 8;
    uint32_t reserved_1 : 8;
    uint32_t max_lvt_entry : 8;
    uint32_t suppress_eoi_broadcast : 1;
    uint32_t reserved_2 : 7;
    uint32_t padding[3];
};

struct apic_lvt_t
{
    uint32_t vector: 8;
    uint32_t delivery_mode : 3;
    uint32_t reserved_1 : 1;
    uint32_t delivery_status : 1;
    uint32_t pin_polarity : 1;
    uint32_t remote_irr : 1;
    uint32_t trigger_mode : 1;
    uint32_t mask : 1;
    uint32_t timer_mode : 2;
    uint32_t reserved_2 : 13;
    uint32_t padding[3];
};

struct apic_icr_t
{
    uint32_t vector : 8;
    uint32_t delivery_mode : 3;
    uint32_t destination_mode : 1;
    uint32_t delivery_status : 1;
    uint32_t reserved_1 : 1;
    uint32_t level : 1;
    uint32_t trigger_mode : 1;
    uint32_t reserved_2 : 2;
    uint32_t destination_shorthand : 2;
    uint32_t reserved_3 : 12;
    uint32_t padding_1[3];
    uint32_t reserved : 24;
    uint32_t destination : 8;
    uint32_t padding_2[3];
};

struct apic_registers_t
{
    struct apic_register_t reserved_1[2];
    struct apic_register_t lapic_id;
    struct apic_lapic_version_t lapic_version;
    struct apic_register_t reserved_2[4];
    struct apic_register_t task_priority;
    struct apic_register_t arbitration_priority;
    struct apic_register_t processor_priority;
    struct apic_register_t eoi;
    struct apic_register_t remote_read;
    struct apic_register_t logical_destination;
    struct apic_register_t destination_format;
    struct apic_register_t spurious_iv;
    struct apic_register_t in_service[8];
    struct apic_register_t trigger_mode[8];
    struct apic_register_t interrupt_request[8];
    struct apic_register_t error_status;
    struct apic_register_t reserved_3[6];
    struct apic_lvt_t lvt_cmci;
    struct apic_icr_t interrput_command;
    struct apic_lvt_t lvt_timer;
    struct apic_lvt_t lvt_thermal_sensor;
    struct apic_lvt_t lvt_performance_counters;
    struct apic_lvt_t lvt_lint0;
    struct apic_lvt_t lvt_lint1;
    struct apic_lvt_t lvt_error;
    struct apic_register_t initial_count;
    struct apic_register_t current_count;
    struct apic_register_t reserved_4[4];
    struct apic_register_t divide_config;
    struct apic_register_t reserved_5;
};

void apic_enable(struct apic_registers_t volatile *apic_registers);

void apic_eoi(struct apic_registers_t volatile *apic_registers);

void apic_send_ipi(struct apic_registers_t volatile *apic_registers,
    uint32_t vector,
    enum apic_delivery_mode_t delivery_mode,
    enum apic_destination_mode_t destination_mode,
    enum apic_level_t level,
    enum apic_trigger_mode_t trigger_mode,
    enum apic_destination_shorthand_t shorthand,
    uint32_t destination);
