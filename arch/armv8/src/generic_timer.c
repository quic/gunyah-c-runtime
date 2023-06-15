// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <guest_types.h>

#include <interrupt.h>
#include <platform_irq.h>
#include <platform_timer.h>

#define CNTV_CTL_ISTATUS (1U << 2)
#define CNTV_CTL_IMASK	 (1U << 1)
#define CNTV_CTL_ENABLE	 (1U << 0)

static char timer_ordering;

static void
write_cntv_ctl(uint64_t cntv_ctl)
{
	__asm__ volatile("msr CNTV_CTL_EL0, %[r]"
			 : "+m"(timer_ordering)
			 : [r] "r"(cntv_ctl));
}

void
platform_timer_init(uint64_t timer_freq)
{
	uint64_t cntfrq;
	__asm__ volatile("mrs %[r], CNTFRQ_EL0" : [r] "=r"(cntfrq));
	assert(cntfrq == timer_freq);

	uint64_t cntkctl = 0U;
	__asm__ volatile("msr CNTKCTL_EL1, %[r]"
			 : "+m"(timer_ordering)
			 : [r] "r"(cntkctl));

	write_cntv_ctl(CNTV_CTL_IMASK);

	(void)platform_irq_set_trigger(PLATFORM_TIMER_IRQ,
				       IRQ_TRIGGER_LEVEL_HIGH);
	platform_irq_enable(PLATFORM_TIMER_IRQ);
}

void
platform_timer_set_timeout(uint64_t ticks)
{
	__asm__ volatile("msr CNTV_CVAL_EL0, %[r]"
			 : "+m"(timer_ordering)
			 : [r] "r"(ticks));

	write_cntv_ctl(CNTV_CTL_ENABLE);
	__asm__ volatile("isb" : "+m"(timer_ordering));
}

void
platform_timer_cancel_timeout(void)
{
	write_cntv_ctl(CNTV_CTL_IMASK);
	__asm__ volatile("isb" : "+m"(timer_ordering));
}

uint64_t
platform_timer_get_current_ticks(void)
{
	uint64_t ticks;

	__asm__ volatile("mrs %[r], CNTVCT_EL0"
			 : "+m"(timer_ordering), [r] "=r"(ticks));

	return ticks;
}
