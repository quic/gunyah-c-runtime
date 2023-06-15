// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define asm_wait_for_interrupt(asm_ordering)                                   \
	__asm__ volatile("dsb ish; wfi; isb" : "+m"(*(asm_ordering)))

#define asm_wait_for_event(asm_ordering)                                       \
	__asm__ volatile("dsb ish; wfe; isb" : "+m"(*(asm_ordering)))

static inline __attribute__((always_inline)) bool
asm_interrupt_pending(void *asm_ordering)
{
	uint64_t isr;
	__asm__ volatile("mrs %[r], ISR_EL1"
			 : "+m"(asm_ordering), [r] "=r"(isr));
	return (isr & 0x80U) != 0U;
}
