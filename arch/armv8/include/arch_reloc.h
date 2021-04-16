// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

// Architecture relocation types
#define R_AARCH64_NONE	   0
#define R_AARCH64_RELATIVE 1027

#define ARCH_CAN_PATCH(r_info)                                                 \
	(((R_TYPE(r_info) == R_AARCH64_NONE) ||                                \
	  (R_TYPE(r_info) == R_AARCH64_RELATIVE)) &&                           \
	 (R_SYM(r_info) == 0))
