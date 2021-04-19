#pragma once

struct interrupt_frame_t;

__attribute__ ((interrupt))
void isr_generic(struct interrupt_frame_t *frame);

__attribute__ ((interrupt))
void isr_division_by_zero(struct interrupt_frame_t *frame);

__attribute__ ((interrupt))
void isr_gp_fault(struct interrupt_frame_t *frame, unsigned int error);

__attribute__ ((interrupt))
void isr_page_fault(struct interrupt_frame_t *frame, unsigned int error);

__attribute__ ((interrupt))
void isr_double_fault(struct interrupt_frame_t *frame, unsigned int error);

__attribute__ ((interrupt))
void isr_timer(struct interrupt_frame_t *frame);

__attribute__ ((naked))
void isr_preempt(struct interrupt_frame_t *frame);

__attribute__ ((interrupt))
void isr_syscall(struct interrupt_frame_t *frame);