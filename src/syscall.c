// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
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

asmlinkage long
sys_set_tid_address(int *tid_ptr)
{
	(void)tid_ptr;
	return 0xdeadbeefL;
}

asmlinkage long
sys_ppoll(void *ufds, unsigned int nfds, const struct timespec *timeout,
	  void *sigmask, uint32_t sigsetsize)
{
	long ret;

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

asmlinkage long
sys_clock_nanosleep(long clock_id, int flags, const struct timespec *request,
		    struct timespec *remain)
{
	long ret;

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

asmlinkage int
sys_tkill(int tid, int sig)
{
	(void)tid;
	(void)sig;

	sys_exit(1);
}
