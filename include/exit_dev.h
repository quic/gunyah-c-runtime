// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define EXIT_MAGIC 0x44U

#define IOCTL_REGISTER_EXIT   IOW(EXIT_MAGIC, 0U, struct register_exit_req)
#define IOCTL_DEREGISTER_EXIT IOW(EXIT_MAGIC, 1U, int)

typedef void (*exit_t)(int exit_code);

struct register_exit_req {
	exit_t exit_func;
};

void
exit_trigger(int exit_code);

int
exit_register(exit_t new_exit_func);

int
exit_deregister(void);

long
exit_open(int flags);
