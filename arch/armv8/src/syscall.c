// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>

#include <asm/unistd.h>

#include <syscall_defs.h>

#include "arch_syscall.h"

#define SYSCALL(x, y) [SYS_##x] = (sys_call_ptr_t)sys_##y

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type"

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

#pragma clang diagnostic pop
