// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

typedef struct trap_frame {
	uint64_t x[29];
	uint64_t x30;
	uint64_t spsr;
	uint64_t esr;
	uint64_t sp_el0;
	uint64_t sp_el1;
	uint64_t _Alignas(16) x29;
	uint64_t elr;
} trap_frame_t;
