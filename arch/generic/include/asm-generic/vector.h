// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

typedef struct trap_frame {
	uint64_t x[31];
	uint64_t spsr;
	uint64_t elr;
	uint64_t esr;
} trap_frame_t;
