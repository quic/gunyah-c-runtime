// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include "arch_def.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
typedef register_t (*const sys_call_ptr_t)(register_t arg0, register_t arg1,
					   register_t arg2, register_t arg3,
					   register_t arg4);

#pragma clang diagnostic pop

extern sys_call_ptr_t aarch64_sys_call_table[NR_syscall_max + 1];

register_t
sys_set_tid_address_wrapper(register_t arg0, register_t arg1, register_t arg2,
			    register_t arg3, register_t arg4);

register_t
sys_ppoll_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4);

register_t
sys_openat_wrapper(register_t arg0, register_t arg1, register_t arg2,
		   register_t arg3, register_t arg4);

register_t
sys_clock_nanosleep_wrapper(register_t arg0, register_t arg1, register_t arg2,
			    register_t arg3, register_t arg4);

register_t
sys_tkill_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4);

register_t
sys_exit_group_wrapper(register_t arg0, register_t arg1, register_t arg2,
		       register_t arg3, register_t arg4);

noreturn register_t
sys_exit_wrapper(register_t arg0, register_t arg1, register_t arg2,
		 register_t arg3, register_t arg4);

register_t
sys_brk_wrapper(register_t arg0, register_t arg1, register_t arg2,
		register_t arg3, register_t arg4);

register_t
sys_ioctl_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4);

register_t
sys_writev_wrapper(register_t arg0, register_t arg1, register_t arg2,
		   register_t arg3, register_t arg4);

register_t
sys_close_wrapper(register_t arg0, register_t arg1, register_t arg2,
		  register_t arg3, register_t arg4);
