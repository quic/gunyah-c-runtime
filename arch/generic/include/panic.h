// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

noreturn void
panic(const char *str);

void
dump_trap_frame(trap_frame_t *frame);
