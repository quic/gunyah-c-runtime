// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include <guest_types.h>

#include <arch_def.h>
#include <errno.h>
#include <fs.h>
#include <mem.h>
#include <util.h>

#include "mmap.h"

static bool
mem_range_is_valid(mem_range_t *range)
{
	assert(range != NULL);

	uintptr_t base = range->base;
	size_t	  size = range->size;

	return (base != 0U) && (size != 0U) &&
	       util_is_baligned(base, PAGE_SIZE) &&
	       util_is_baligned(size, PAGE_SIZE) &&
	       !util_add_overflows(base, size);
}

static int32_t
mem_add_heap(mem_range_t *range)
{
	int32_t ret;

	if (mem_range_is_valid(range)) {
		ret = mmap_add_heap(range->base, range->size, false);
	} else {
		ret = -EINVAL;
	}

	return ret;
}

static int32_t
mem_remove_heap(mem_range_t *range)
{
	int32_t ret;

	if (mem_range_is_valid(range)) {
		ret = mmap_remove_heap(range->base, range->size);
	} else {
		ret = -EINVAL;
	}

	return ret;
}

static int32_t
mem_heap_is_free(mem_range_t *range)
{
	int32_t ret;

	if (mem_range_is_valid(range)) {
		ret = mmap_heap_is_free(range->base, range->size);
	} else {
		ret = -EINVAL;
	}

	return ret;
}

static int32_t
mem_ioctl(uint32_t cmd, uintptr_t arg)
{
	int32_t ret;

	switch (cmd) {
	case IOCTL_ADD_HEAP:
		ret = mem_add_heap((mem_range_t *)arg);
		break;
	case IOCTL_REMOVE_HEAP:
		ret = mem_remove_heap((mem_range_t *)arg);
		break;
	case IOCTL_HEAP_IS_FREE:
		ret = mem_heap_is_free((mem_range_t *)arg);
		break;
	case IOCTL_HEAP_GET_STATS:
		ret = mmap_heap_get_stats((allocator_stats_t *)arg);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static struct fs_ops mem_ops = {
	.ioctl = &mem_ioctl,
};

static struct file_s mem_file = {
	.ops = &mem_ops,
};

int32_t
mem_open(uint32_t flags)
{
	return fs_alloc_fd(&mem_file, flags);
}
