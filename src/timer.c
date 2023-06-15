// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <types.h>

#include <guest_types.h>

#include <compiler.h>
#include <cpulocal.h>
#include <errno.h>
#include <interrupt.h>
#include <platform_timer.h>
#include <timer.h>
#include <util.h>

#define NS_PER_S 1000000000

#define TIMESPEC_MAX_NSEC (NS_PER_S - 1)

extern rt_env_data_t *env_data;

static uint64_t timer_freq;
static long	timespec_max_sec;

CPULOCAL_DECLARE_STATIC(bool, timer_fired);

static bool
timer_isr(virq_t irq, void *data)
{
	CPULOCAL(timer_fired) = true;
	platform_timer_cancel_timeout();

	(void)irq;
	(void)data;

	return true;
}

void
timer_init(void)
{
	timer_freq = env_data->timer_freq;
	uint64_t u_timespec_max_sec =
		(UINT64_MAX - (uint64_t)TIMESPEC_MAX_NSEC) / timer_freq;
	assert(u_timespec_max_sec > 0U);
	timespec_max_sec =
		(long)util_min(u_timespec_max_sec, (uint64_t)LONG_MAX);

	platform_timer_init(timer_freq);
	(void)interrupt_register_isr(PLATFORM_TIMER_IRQ, &timer_isr, NULL);
}

static bool
timespec_valid(const struct timespec *ts)
{
	return (ts->tv_sec >= 0) && (ts->tv_nsec >= 0) &&
	       (ts->tv_sec <= timespec_max_sec) &&
	       (ts->tv_nsec <= TIMESPEC_MAX_NSEC);
}

static uint64_t
timespec_to_ticks(const struct timespec *ts)
{
	return ((uint32_t)ts->tv_sec * timer_freq) +
	       (((uint32_t)ts->tv_nsec * timer_freq) / (uint32_t)NS_PER_S);
}

extern void
ticks_to_timespec(uint64_t ticks, struct timespec *ts);

void
ticks_to_timespec(uint64_t ticks, struct timespec *ts)
{
	assert(ts != NULL);

	ts->tv_sec  = (long)ticks / (long)timer_freq;
	ts->tv_nsec = (((long)ticks % (long)timer_freq) * NS_PER_S) /
		      (long)timer_freq;

	assert(timespec_valid(ts));
}

long
timer_set_and_wait(bool relative, const struct timespec *req,
		   struct timespec *remain)
{
	long ret;

	if (compiler_unexpected(!timespec_valid(req))) {
		ret = -EINVAL;
		goto out;
	}

	uint64_t curticks = platform_timer_get_current_ticks();
	uint64_t timeout  = timespec_to_ticks(req);

	if (relative) {
		timeout += curticks;
	}

	if (timeout <= curticks) {
		ret = 0;
		goto out;
	}

	platform_timer_set_timeout(timeout);

	(void)interrupt_wait();

	if (CPULOCAL(timer_fired)) {
		CPULOCAL(timer_fired) = false;
		ret		      = 0;
	} else {
		platform_timer_cancel_timeout();
		ret = -EINTR;
	}

	if (remain != NULL) {
		curticks	  = platform_timer_get_current_ticks();
		uint64_t remticks = (timeout > curticks) ? (timeout - curticks)
							 : 0U;
		ticks_to_timespec(remticks, remain);
	}
out:
	return ret;
}
