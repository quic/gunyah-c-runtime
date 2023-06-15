// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <time.h>

#include <asm/unistd.h>

#include <syscall_defs.h>

#include "arch_syscall.h"

#define SYSCALL(x, y) [SYS_##x] = (sys_call_ptr_t)sys_##y##_wrapper

sys_call_ptr_t aarch64_sys_call_table[NR_syscall_max + 1] = {
	SYSCALL(openat, openat),
	SYSCALL(ppoll, ppoll),
	SYSCALL(set_tid_address, set_tid_address),
	SYSCALL(exit_group, exit_group),
	SYSCALL(exit, exit),
	SYSCALL(brk, brk),
	SYSCALL(ioctl, ioctl),
	SYSCALL(writev, writev),
	SYSCALL(clock_nanosleep, clock_nanosleep),
	SYSCALL(tkill, tkill),
	SYSCALL(close, close),
};

register_t
sys_set_tid_address_wrapper(register_t arg0, register_t arg1, register_t arg2,
			    register_t arg3, register_t arg4)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;

	return (register_t)sys_set_tid_address((int *)arg0);
}

register_t
sys_ppoll_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4)
{
	return (register_t)sys_ppoll((char *)arg0, (unsigned int)arg1,
				     (const struct timespec *)arg2,
				     (char *)arg3, (uint32_t)arg4);
}

register_t
sys_openat_wrapper(register_t arg0, register_t arg1, register_t arg2,
		   register_t arg3, register_t arg4)
{
	(void)arg4;

	return (register_t)sys_openat((int)arg0, (const char *)arg1, (int)arg2,
				      (umode_t)arg3);
}

register_t
sys_clock_nanosleep_wrapper(register_t arg0, register_t arg1, register_t arg2,
			    register_t arg3, register_t arg4)
{
	(void)arg4;

	return (register_t)sys_clock_nanosleep((long)arg0, (int)arg1,
					       (const struct timespec *)arg2,
					       (struct timespec *)arg3);
}

register_t
sys_tkill_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4)
{
	(void)arg2;
	(void)arg3;
	(void)arg4;

	return (register_t)sys_tkill((int)arg0, (int)arg1);
}

register_t
sys_exit_group_wrapper(register_t arg0, register_t arg1, register_t arg2,
		       register_t arg3, register_t arg4)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;

	sys_exit_group((int)arg0);

	return (register_t)0UL;
}

noreturn register_t
sys_exit_wrapper(register_t arg0, register_t arg1, register_t arg2,
		 register_t arg3, register_t arg4)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;

	sys_exit((int)arg0);
}

register_t
sys_brk_wrapper(register_t arg0, register_t arg1, register_t arg2,
		register_t arg3, register_t arg4)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;

	return (register_t)sys_brk((unsigned long)arg0);
}

register_t
sys_ioctl_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4)
{
	(void)arg3;
	(void)arg4;

	return (register_t)sys_ioctl((unsigned int)arg0, (unsigned int)arg1,
				     (unsigned long)arg2);
}

register_t
sys_writev_wrapper(register_t arg0, register_t arg1, register_t arg2,
		   register_t arg3, register_t arg4)
{
	(void)arg3;
	(void)arg4;

	return (register_t)sys_writev((unsigned long)arg0, (uintptr_t)arg1,
				      (unsigned long)arg2);
}

register_t
sys_close_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4)
{
	(void)arg1;
	(void)arg2;
	(void)arg3;
	(void)arg4;

	return (register_t)sys_close((unsigned int)arg0);
}
