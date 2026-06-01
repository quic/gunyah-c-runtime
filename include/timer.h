// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

struct timespec;

void
timer_init(void);

int32_t
timer_set_and_wait(bool relative, const struct timespec *req,
		   struct timespec *remain);
