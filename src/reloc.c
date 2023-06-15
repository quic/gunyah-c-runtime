// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>

#include <elf.h>

#include <guest_types.h>

#include <reloc.h>

#include <arch_reloc.h>

// We must disable stack protection for this function, because the compiler
// might use a relocated absolute pointer to load the stack cookie in the
// function prologue, which will crash because this function hasn't run yet.
__attribute__((no_stack_protector)) void
rel_fixup(Elf64_Dyn *dyni, rt_env_data_t *env_data)
{
	Elf64_Addr base = env_data->runtime_ipa;

	Elf64_Addr  dyn[DT_CNT] = { 0 };
	Elf64_Rel  *rel		= NULL;
	Elf64_Rel  *rel_end	= NULL;
	Elf64_Rela *rela	= NULL;
	Elf64_Rela *rela_end	= NULL;

	for (; dyni->d_tag != DT_NULL; dyni += 1) {
		if (dyni->d_tag < DT_CNT) {
			dyn[dyni->d_tag] = dyni->d_un.d_ptr;
		}
	}

	rel	= (Elf64_Rel *)(dyn[DT_REL] + base);
	rel_end = (Elf64_Rel *)(dyn[DT_REL] + base + dyn[DT_RELSZ]);
	// Loop will not execute if the toolchain does not generate DT_REL
	for (; rel < rel_end; rel++) {
		if (!ARCH_CAN_PATCH(rel->r_info)) {
			continue;
		}
		Elf64_Addr *r = (Elf64_Addr *)(rel->r_offset + base);
		*r += base;
	}

	rela	 = (Elf64_Rela *)(dyn[DT_RELA] + base);
	rela_end = (Elf64_Rela *)(dyn[DT_RELA] + base + dyn[DT_RELASZ]);
	// Loop will not execute if the toolchain does not generate DT_RELA
	for (; rela < rela_end; rela++) {
		if (!ARCH_CAN_PATCH(rela->r_info)) {
			continue;
		}
		Elf64_Addr *r = (Elf64_Addr *)(rela->r_offset + base);
		*r	      = base + (Elf64_Addr)rela->r_addend;
	}
}
