// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define WORKAROUND_UNEXPECTED_ACTIVE_IRQ 1

static inline __attribute__((always_inline)) void
asm_wait_for_interrupt(char *asm_ordering)
{
#if defined(WORKAROUND_UNEXPECTED_ACTIVE_IRQ) &&                               \
	WORKAROUND_UNEXPECTED_ACTIVE_IRQ
	uint64_t rpr;
	__asm__("isb; mrs %[r], ICC_RPR_EL1"
		: "+m"(*asm_ordering), [r] "=r"(rpr));
	if (__builtin_expect((rpr != 0xffU), 0)) {
		volatile static count_t rpr_non_idle_count = 0U;
		// We have a stray active priority. Log and clear it.
		rpr_non_idle_count++;
		__asm__ volatile("msr ICC_AP1R0_EL1, xzr; isb"
				 : "+m"(*asm_ordering));
	}
#endif

	__asm__ volatile("dsb ish; wfi; isb" : "+m"(*asm_ordering));
}

static inline __attribute__((always_inline)) void
asm_wait_for_event(int *asm_ordering)
{
	__asm__ volatile("dsb ish; wfe; isb" : "+m"(*asm_ordering));
}

static inline __attribute__((always_inline)) bool
asm_interrupt_pending(char *asm_ordering)
{
	uint64_t isr;
	__asm__ volatile("mrs %[r], ISR_EL1"
			 : "+m"(*asm_ordering), [r] "=r"(isr));
	return (isr & 0x80U) != 0U;
}
