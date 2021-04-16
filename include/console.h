// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <uapi/console.h>

void
console_write(const char *msg, size_t sz);

int
console_register(console_t console);

int
console_deregister(void);

long
console_open(int flags);
