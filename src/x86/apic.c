#include "apic.h"

void apic_enable()
{

}

void apic_eoi()
{
    apic_registers->eoi.value = 0;
}

void apic_send_ipi(
    uint32_t vector,
    enum apic_delivery_mode_t delivery_mode,
    enum apic_destination_mode_t destination_mode,
    enum apic_level_t level,
    enum apic_trigger_mode_t trigger_mode,
    enum apic_destination_shorthand_t shorthand,
    uint32_t destination)
{
    struct apic_icr_t value = {
        .vector = vector,
        .delivery_mode = delivery_mode,
        .destination_mode = destination_mode,
        .level = level,
        .trigger_mode = trigger_mode,
        .destination_shorthand = shorthand,
        .destination = destination
    };
    uint32_t *value_addr = (uint32_t*) &value;
    uint32_t *icr_addr = (uint32_t*)&apic_registers->interrput_command;
    icr_addr[4] = value_addr[4];
    icr_addr[0] = value_addr[0];
}