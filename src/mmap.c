// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>

#include <arch_def.h>
#include <errno.h>
#include <log.h>
#include <syscall_defs.h>
#include <util.h>

#include "mmap.h"

static uintptr_t heap_bottom;
static size_t	 heap_size;
static uintptr_t cur_brk;

void
init_heap(uintptr_t base, size_t size)
{
	// heap bottom should be PAGE aligned, musl aligns it if it's not.
	assert(util_is_baligned(base, PAGE_SIZE));
	assert(util_is_baligned(size, PAGE_SIZE));

	heap_bottom = base;
	cur_brk	    = base;
	heap_size   = size;
}

asmlinkage long
sys_brk(unsigned long brk)
{
	uintptr_t new_brk = util_balign_up(brk, PAGE_SIZE);

	if ((new_brk >= heap_bottom) && (new_brk <= heap_bottom + heap_size)) {
		cur_brk = new_brk;
	} else if (brk != 0UL) {
		// brk not in heap range
		LOG(ERROR, MSG, "{:s}: invalid brk {:#x}\n",
		    (register_t) __func__, brk);
	} else {
		// It's just query the heap top with brk == 0
	}

	return (long)cur_brk;
}
