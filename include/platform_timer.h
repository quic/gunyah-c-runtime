// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

void
platform_timer_init(uint64_t platform_timer_freq);

void
platform_timer_set_timeout(uint64_t ticks);

void
platform_timer_cancel_timeout(void);

uint64_t
platform_timer_get_current_ticks(void);
