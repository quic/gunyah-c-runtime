// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
typedef long (*const sys_call_ptr_t)();
#pragma clang diagnostic pop

extern sys_call_ptr_t aarch64_sys_call_table[NR_syscall_max + 1];
