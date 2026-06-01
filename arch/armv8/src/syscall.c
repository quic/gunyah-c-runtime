// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <time.h>

#include <guest_types.h>

#include <arch_def.h>
#include <compiler.h>
#include <errno.h>
#include <log.h>
#include <syscall.h>
#include <syscall_defs.h>

extern register_t
aarch64_do_syscall(register_t arg0, register_t arg1, register_t arg2,
		   register_t arg3, register_t arg4, register_t arg5,
		   register_t sys_num);

register_t
aarch64_do_syscall(register_t arg0, register_t arg1, register_t arg2,
		   register_t arg3, register_t arg4, register_t arg5,
		   register_t sys_num)
{
	register_t ret;

	switch (sys_num) {
	case SYS_writev:
		ret = (register_t)sys_writev((uint32_t)arg0, (uintptr_t)arg1,
					     (size_t)arg2);
		break;
	case SYS_ppoll:
		ret = (register_t)sys_ppoll((uintptr_t)arg0, (uint32_t)arg1,
					    (const struct timespec *)arg2,
					    (uintptr_t)arg3, (size_t)arg4);
		break;
	case SYS_clock_nanosleep:
		ret = (register_t)sys_clock_nanosleep(
			(int32_t)arg0, (int32_t)arg1,
			(const struct timespec *)arg2, (struct timespec *)arg3);
		break;
	case SYS_ioctl:
		ret = (register_t)sys_ioctl((uint32_t)arg0, (uint32_t)arg1,
					    (uintptr_t)arg2);
		break;
	case SYS_openat:
		ret = (register_t)sys_openat((int32_t)arg0, (const char *)arg1,
					     (uint32_t)arg2, (uint32_t)arg3);
		break;
	case SYS_close:
		ret = (register_t)sys_close((uint32_t)arg0);
		break;
	case SYS_brk:
		ret = (register_t)sys_brk((uintptr_t)arg0);
		break;
	case SYS_mmap:
		ret = (register_t)sys_mmap((uintptr_t)arg0, (size_t)arg1,
					   (uint32_t)arg2, (uint32_t)arg3,
					   (int32_t)arg4, (size_t)arg5);
		break;
	case SYS_munmap:
		ret = (register_t)sys_munmap((uintptr_t)arg0, (size_t)arg1);
		break;
	case SYS_set_tid_address:
		ret = (register_t)sys_set_tid_address((int32_t *)arg0);
		break;
	case SYS_tkill:
		ret = (register_t)sys_tkill((int32_t)arg0, (int32_t)arg1);
		break;
	case SYS_exit:
		sys_exit((int32_t)arg0);
	case SYS_exit_group:
		sys_exit_group((int32_t)arg0);
	default:
		LOG(ERROR, MSG, "Unhandled syscall {:d}\n", sys_num);
		ret = (register_t)-ENOSYS;
		break;
	}

	return ret;
}
