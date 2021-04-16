// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>

#include <elf.h>

#include <arch_reloc.h>
#include <reloc.h>

void
rel_fixup(Elf64_Dyn *dyni, Elf64_Addr base)
{
	Elf64_Addr  dyn[DT_CNT];
	Elf64_Rel * rel	 = NULL;
	Elf64_Rela *rela = NULL;
	Elf64_Word  sz	 = 0;

	for (int i = 0; i < DT_CNT; ++i) {
		dyn[i] = 0;
	}
	for (; dyni->d_tag != DT_NULL; dyni += 1) {
		if (dyni->d_tag < DT_CNT) {
			dyn[dyni->d_tag] = dyni->d_un.d_ptr;
		}
	}

	rel = (Elf64_Rel *)(dyn[DT_REL] + base);
	sz  = (Elf64_Word)dyn[DT_RELSZ];
	for (; sz > 0; sz -= sizeof(*rel), ++rel) {
		if (!ARCH_CAN_PATCH(rel->r_info)) {
			continue;
		}
		Elf64_Addr *r = (Elf64_Addr *)(rel->r_offset + base);
		*r += base;
	}

	rela = (Elf64_Rela *)(dyn[DT_RELA] + base);
	sz   = (Elf64_Word)dyn[DT_RELASZ];
	for (; sz > 0; sz -= sizeof(*rela), ++rela) {
		if (!ARCH_CAN_PATCH(rela->r_info)) {
			continue;
		}
		Elf64_Addr *r = (Elf64_Addr *)(rela->r_offset + base);
		*r	      = base + (Elf64_Addr)rela->r_addend;
	}
}
