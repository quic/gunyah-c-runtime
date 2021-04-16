// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

void
timer_init(void);

long
timer_set_and_wait(bool relative, const struct timespec *req,
		   struct timespec *remain);
