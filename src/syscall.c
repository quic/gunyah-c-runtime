// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdbool.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include <guest_types.h>

#include <arch_def.h>
#include <errno.h>
#include <fcntl.h>
#include <interrupt.h>
#include <syscall_defs.h>
#include <timer.h>

#define APP_TID 1234

int32_t
sys_set_tid_address(int32_t *tid_ptr)
{
	(void)tid_ptr;
	return APP_TID;
}

int32_t
sys_ppoll(uintptr_t ufds, uint32_t nfds, const struct timespec *timeout,
	  uintptr_t sigmask, size_t sigsetsize)
{
	int32_t ret;

	(void)ufds;
	(void)sigmask;
	(void)sigsetsize;

	if (nfds != 0U) {
		ret = -EINVAL;
		goto out;
	}

	if (timeout != NULL) {
		ret = timer_set_and_wait(true, timeout, NULL);
	} else {
		(void)interrupt_wait();
		ret = -EINTR;
	}

out:
	return ret;
}

int32_t
sys_clock_nanosleep(int32_t clock_id, int32_t flags,
		    const struct timespec *request, struct timespec *remain)
{
	int32_t ret;

	if (clock_id != CLOCK_MONOTONIC) {
		// FIXME: Also accept CLOCK_REALTIME?
		ret = -EINVAL;
		goto out;
	}

	if (flags == 0) {
		ret = timer_set_and_wait(true, request, remain);
	} else if (flags == TIMER_ABSTIME) {
		ret = timer_set_and_wait(false, request, NULL);
	} else {
		ret = -EINVAL;
	}

out:
	return ret;
}

int32_t
sys_tkill(int32_t tid, int32_t sig)
{
	int32_t ret;

	(void)sig;

	if (tid == APP_TID) {
		sys_exit(1);
	} else {
		ret = -EINVAL;
	}

	return ret;
}
