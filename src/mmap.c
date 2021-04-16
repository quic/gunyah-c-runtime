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

static uintptr_t g_heap_bottom;
static size_t	 g_heap_size;
static uintptr_t g_cur_heap;

void
init_heap(uintptr_t heap_bottom, size_t size)
{
	// heap bottom should be PAGE aligned, musl aligns it if it's not.
	assert(util_is_baligned(heap_bottom, PAGE_SIZE));

	g_heap_bottom = heap_bottom;
	g_cur_heap    = g_heap_bottom;
	g_heap_size   = size;
}

asmlinkage long
sys_brk(unsigned long brk)
{
	if ((brk < g_heap_bottom + g_heap_size) && (brk >= g_heap_bottom)) {
		g_cur_heap = util_balign_up(brk, PAGE_SIZE);
	} else if (brk != 0UL) {
		// It's just query the heap top with brk == 0
		LOG(ERROR, MSG, "{:s}: invalid brk {:#x}\n",
		    (register_t) __func__, brk);
	}

	return (long)g_cur_heap;
}
