// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <uapi/exit_dev.h>

void
exit_trigger(int exit_code);

int
exit_register(exit_t exit_func);

int
exit_deregister(void);

long
exit_open(int flags);
