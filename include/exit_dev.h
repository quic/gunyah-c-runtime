// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#define EXIT_MAGIC 0x44U

#define IOCTL_REGISTER_EXIT   IOW(EXIT_MAGIC, 0U, struct register_exit_req)
#define IOCTL_DEREGISTER_EXIT IOW(EXIT_MAGIC, 1U, int32_t)

typedef void (*exit_t)(int32_t exit_code);

struct register_exit_req {
	exit_t exit_func;
};

void
exit_trigger(int32_t exit_code);

int32_t
exit_register(exit_t new_exit_func);

int32_t
exit_deregister(void);

int32_t
exit_open(uint32_t flags);
