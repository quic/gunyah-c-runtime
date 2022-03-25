// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define R_TYPE(r_info) ((r_info)&0x7fffffff)
#define R_SYM(r_info)  ((r_info) >> 32)

// only record DT type up to DT_RELSZ == 18
#define DT_CNT 19

void
rel_fixup(Elf64_Dyn *dyni, boot_env_data_t *env_data);
