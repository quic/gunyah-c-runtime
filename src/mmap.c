// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <guest_types.h>

#include <arch_def.h>
#include <bitmap.h>
#include <compiler.h>
#include <errno.h>
#include <list.h>
#include <log.h>
#include <syscall_defs.h>
#include <util.h>

#include "mmap.h"

// This enables tracking of mmap allocations to help detect invalid frees. Note
// that this prevents partial unmappings which munmap does technically support,
// but there aren't any current use-cases which require it (and we likely don't
// want to support it anyway) so it's ok to be strict about it.
#define MMAP_ALLOC_TRACK 0

// This enables sanitising of free & uninitialised memory to help detect
// use-after-free.
#define MMAP_SANITISE 0

typedef struct mmap_node_s mmap_node_t;

struct mmap_node_s {
	uintptr_t base;
	size_t	  size;

	mmap_node_t *next;
	mmap_node_t *prev;

	register_t *avail_blocks;
	register_t *free_pages;
#if MMAP_ALLOC_TRACK
	register_t *alloc_start;
	register_t *alloc_end;
#endif

	size_t meta_size;
};

static mmap_node_t *mmap_node_list;
static mmap_node_t *mmap_brk_node;

static size_t mmap_total_size;
static size_t mmap_alloc_size;

static uintptr_t brk_base;
static uintptr_t brk_top;
static uintptr_t cur_brk;

#if MMAP_SANITISE
#define MMAP_FREE_BYTE 0xf3
#define MMAP_BRK_BYTE  0xb8
#endif

uintptr_t
sys_brk(uintptr_t brk)
{
	uintptr_t new_brk = util_balign_up(brk, PAGE_SIZE);

	if ((new_brk >= brk_base) && (new_brk <= brk_top)) {
#if MMAP_SANITISE
		if (new_brk > cur_brk) {
			(void)memset((void *)cur_brk, MMAP_BRK_BYTE,
				     new_brk - cur_brk);
		} else {
			(void)memset((void *)new_brk, MMAP_FREE_BYTE,
				     cur_brk - new_brk);
		}
#endif
		cur_brk = new_brk;
	} else if (brk != 0UL) {
		// brk not in heap range
		LOG(ERROR, MSG, "{:s}: invalid brk {:#x}\n",
		    (register_t) __func__, brk);
	} else {
		// It's just query the heap top with brk == 0
	}

	return cur_brk;
}

static void
update_mmap_range(mmap_node_t *node, index_t start_bit, count_t num_bits,
		  bool set)
{
	index_t curr_bit = start_bit;
	count_t rem_bits = num_bits;

	while (rem_bits > 0U) {
		// If we are updating across bitmap words, only the first update
		// is allowed to start at a non-zero index within the word.
		index_t i = curr_bit % BITMAP_WORD_BITS;
		index_t j = curr_bit / BITMAP_WORD_BITS;
		assert((curr_bit == start_bit) || (i == 0U));

		// We can only update one bitmap word at a time, so truncate the
		// width to the maximum allowed.
		count_t	   width = util_min(rem_bits, BITMAP_WORD_BITS - i);
		register_t mask	 = util_mask_safe(width);
		if (set) {
			// The pages must not be set in the bitmap.
			assert(bitmap_extract(node->free_pages, curr_bit,
					      width) == 0U);

			// Add the pages to the bitmap, and ensure the block is
			// marked as having available pages.
			bitmap_insert(node->free_pages, curr_bit, width, mask);
			bitmap_set(node->avail_blocks, j);
		} else {
			// The pages must be set in the bitmap.
			assert(bitmap_isset(node->avail_blocks, j));
			assert(bitmap_extract(node->free_pages, curr_bit,
					      width) == mask);

			// Remove pages from the bitmap. If there are no pages
			// remaining in the block, update the block bitmap too.
			bitmap_insert(node->free_pages, curr_bit, width, 0U);
			if (node->free_pages[j] == 0U) {
				bitmap_clear(node->avail_blocks, j);
			}
		}

		curr_bit += width;
		rem_bits -= width;
	}

	assert(curr_bit == (start_bit + num_bits));
	assert(rem_bits == 0U);
}

static bool
mmap_range_is_free(const mmap_node_t *node, index_t start_bit, count_t num_bits,
		   index_t *next_bit)
{
	index_t curr_bit = start_bit;
	count_t rem_bits = num_bits;

	while (rem_bits > 0U) {
		index_t i = curr_bit % BITMAP_WORD_BITS;
		assert((curr_bit == start_bit) || (i == 0U));

		// We can only check one bitmap word at a time, so truncate the
		// width to the maximum allowed.
		count_t width = util_min(rem_bits, BITMAP_WORD_BITS - i);

		// Check if all bits are set for the range.
		register_t mask = util_mask_safe(width);
		register_t val =
			bitmap_extract(node->free_pages, curr_bit, width);
		if (val != mask) {
			// Return the bit after the highest allocated bit found
			// for the next search.
			register_t alloc_bits = ~val & mask;
			assert(alloc_bits != 0U);

			*next_bit = curr_bit +
				    (index_t)compiler_msb(alloc_bits) + 1U;
			break;
		}

		curr_bit += width;
		rem_bits -= width;
	}

	return rem_bits == 0U;
}

static bool
search_free_mmap_range(const mmap_node_t *node, index_t *ret_bit,
		       count_t num_bits)
{
	bool	ret	  = false;
	count_t node_bits = (count_t)(node->size / PAGE_SIZE);

	if (num_bits > node_bits) {
		goto out;
	}

	index_t curr_bit  = 0U;
	count_t end_bit	  = node_bits - num_bits;
	count_t end_block = end_bit / BITMAP_WORD_BITS;

	do {
		// Check the available block bitmap, and skip over blocks with
		// no pages available.
		index_t curr_block = curr_bit / BITMAP_WORD_BITS;
		index_t next_block = 0U;
		if (!bitmap_ffs(node->avail_blocks, curr_block, end_block,
				&next_block)) {
			break;
		}
		curr_bit = util_max(curr_bit, next_block * BITMAP_WORD_BITS);

		// Find the next free page for the range check.
		index_t next_bit = 0U;
		if (!bitmap_ffs(node->free_pages, curr_bit, end_bit,
				&next_bit)) {
			break;
		}

		// Check if the next range of pages is free. If it isn't free,
		// continue the search after the last allocated page found.
		if (mmap_range_is_free(node, next_bit, num_bits, &curr_bit)) {
			*ret_bit = next_bit;
			ret	 = true;
			break;
		}
	} while (curr_bit <= end_bit);

out:
	return ret;
}

static intptr_t
alloc_mmap(size_t len)
{
	intptr_t ret;

	assert(util_is_baligned(len, PAGE_SIZE));
	assert(cur_brk <= brk_top);

	if (len > mmap_total_size) {
		ret = -ENOMEM;
		goto out;
	}

	index_t start_bit = 0U;
	count_t num_bits  = (count_t)(len / PAGE_SIZE);

	mmap_node_t *node = NULL;
	loop_list(node, &mmap_node_list, )
	{
		// Search for a free range in the bitmaps.
		if (search_free_mmap_range(node, &start_bit, num_bits)) {
			update_mmap_range(node, start_bit, num_bits, false);
			break;
		}
	}

	if ((node == NULL) && (len <= (brk_top - cur_brk))) {
		// Steal some memory from the top of the break.
		node = mmap_brk_node;
		brk_top -= len;
		start_bit = (index_t)((brk_top - node->base) / PAGE_SIZE);
	}

	if (node == NULL) {
		LOG(ERROR, MSG, "sys_mmap: out of memory {:#x} {:#x} {:#x}\n",
		    len, mmap_total_size, mmap_alloc_size);
		ret = -ENOMEM;
		goto out;
	}

	uintptr_t addr = node->base + ((size_t)start_bit * PAGE_SIZE);
	assert(util_is_baligned(addr, PAGE_SIZE));
	assert(addr >= node->base);
	assert((addr + len) <= (node->base + node->size));

#if MMAP_ALLOC_TRACK
	// There should be no allocations within the returned range.
	index_t end_bit = start_bit + num_bits - 1U;
	index_t ffs_ret = 0U;
	assert(!bitmap_ffs(node->alloc_start, start_bit, end_bit, &ffs_ret));
	assert(!bitmap_ffs(node->alloc_end, start_bit, end_bit, &ffs_ret));

	// Record start and end pages of the the allocation.
	bitmap_set(node->alloc_start, start_bit);
	bitmap_set(node->alloc_end, end_bit);
#endif

	mmap_alloc_size += len;
	(void)memset((void *)addr, 0, len);
	ret = (intptr_t)addr;

out:
	return ret;
}

static int32_t
free_mmap(uintptr_t addr, size_t len)
{
	int32_t ret;

	assert(!util_add_overflows(addr, len));
	assert(util_is_baligned(addr, PAGE_SIZE));
	assert(util_is_baligned(len, PAGE_SIZE));

	mmap_node_t *node = NULL;
	loop_list(node, &mmap_node_list, )
	{
		if ((addr >= node->base) &&
		    ((addr + len) <= (node->base + node->size))) {
			break;
		}
	}

	if (node == NULL) {
		ret = -EINVAL;
		goto out;
	}

	index_t start_bit = (index_t)((addr - node->base) / PAGE_SIZE);
	count_t num_bits  = (count_t)(len / PAGE_SIZE);

#if MMAP_ALLOC_TRACK
	// Confirm that the free matches a valid start and end point.
	index_t end_bit = start_bit + num_bits - 1U;
	assert(bitmap_isset(node->alloc_start, start_bit));
	assert(bitmap_isset(node->alloc_end, end_bit));

	// For multi-page allocations, confirm that there are no other
	// allocations within the range.
	if (start_bit != end_bit) {
		index_t ffs_ret = 0U;
		assert(!bitmap_ffs(node->alloc_start, start_bit + 1U, end_bit,
				   &ffs_ret));
		assert(!bitmap_ffs(node->alloc_end, start_bit, end_bit - 1U,
				   &ffs_ret));
	}

	// Remove the allocation record.
	bitmap_clear(node->alloc_start, start_bit);
	bitmap_clear(node->alloc_end, end_bit);
#endif

	update_mmap_range(node, start_bit, num_bits, true);

#if MMAP_SANITISE
	(void)memset((void *)addr, MMAP_FREE_BYTE, len);
#endif

	mmap_alloc_size -= len;
	ret = 0;

out:
	return ret;
}

intptr_t
sys_mmap(uintptr_t addr, size_t len, uint32_t prot, uint32_t flags, int32_t fd,
	 size_t off)
{
	intptr_t ret;

	// We currently only support anonymous private mappings with RW access.
	if ((addr != 0U) || (len == 0U) || !util_is_baligned(len, PAGE_SIZE) ||
	    (flags != ((uint32_t)MAP_ANONYMOUS | (uint32_t)MAP_PRIVATE)) ||
	    (prot != ((uint32_t)PROT_READ | (uint32_t)PROT_WRITE)) ||
	    (fd != -1) || (off != 0U)) {
		LOG(ERROR, MSG, "sys_mmap invalid: {:#x} {:#x} {:#x} {:#x} ",
		    addr, len, prot, flags);
		LOG(ERROR, MSG, "{:#x} {:#x}\n", (register_t)fd, off);
		ret = -EINVAL;
		goto out;
	}

	ret = alloc_mmap(len);

out:
	return ret;
}

int32_t
sys_munmap(uintptr_t addr, size_t len)
{
	int32_t ret;

	if ((addr == 0U) || (len == 0U) || util_add_overflows(addr, len) ||
	    !util_is_baligned(addr, PAGE_SIZE) ||
	    !util_is_baligned(len, PAGE_SIZE)) {
		ret = -EINVAL;
		goto out;
	}

	ret = free_mmap(addr, len);

out:
	return ret;
}

int32_t
mmap_add_heap(uintptr_t base, size_t size, bool is_brk)
{
	int32_t ret;

	assert(size != 0U);
	assert(util_is_baligned(base, PAGE_SIZE));
	assert(util_is_baligned(size, PAGE_SIZE));
	assert(!util_add_overflows(base, size));

	const count_t num_pages	 = (count_t)(size / PAGE_SIZE);
	const count_t num_blocks = BITMAP_NUM_WORDS(num_pages);

	count_t total_words =
		BITMAP_NUM_WORDS(num_blocks) + BITMAP_NUM_WORDS(num_pages);
#if MMAP_ALLOC_TRACK
	total_words += 2U * BITMAP_NUM_WORDS(num_pages);
#endif
	size_t meta_size =
		sizeof(mmap_node_t) + (total_words * sizeof(register_t));

	size_t meta_size_align = util_balign_up(meta_size, PAGE_SIZE);
	if (meta_size_align >= size) {
		ret = -ENOMEM;
		goto out;
	}

	mmap_node_t *node = (mmap_node_t *)base;
	(void)memset(node, 0, meta_size);

	node->base = base;
	node->size = size;

	node->avail_blocks = (register_t *)(base + sizeof(*node));
	node->free_pages   = node->avail_blocks + BITMAP_NUM_WORDS(num_blocks);
#if MMAP_ALLOC_TRACK
	node->alloc_start = node->free_pages + BITMAP_NUM_WORDS(num_pages);
	node->alloc_end	  = node->alloc_start + BITMAP_NUM_WORDS(num_pages);
#endif
	node->meta_size = meta_size_align;

	if (is_brk) {
		assert(mmap_brk_node == NULL);
		mmap_brk_node = node;

		// Add free pages to the program break.
		brk_base = base + meta_size_align;
		cur_brk	 = base + meta_size_align;
		brk_top	 = base + size;
	} else {
		// Add free pages to the bitmaps.
		count_t meta_bits = (count_t)(meta_size_align / PAGE_SIZE);
		update_mmap_range(node, meta_bits, num_pages - meta_bits, true);
	}

#if MMAP_SANITISE
	(void)memset((void *)(base + meta_size), MMAP_FREE_BYTE,
		     size - meta_size);
#endif

	mmap_total_size += size;
	list_append(mmap_node_t, &mmap_node_list, node, );
	ret = 0;

out:
	return ret;
}

static int32_t
is_mmap_node_free(uintptr_t base, size_t size, mmap_node_t **node_ret)
{
	int32_t ret;

	assert(size != 0U);
	assert(util_is_baligned(base, PAGE_SIZE));
	assert(util_is_baligned(size, PAGE_SIZE));
	assert(!util_add_overflows(base, size));

	mmap_node_t *node = NULL;
	loop_list(node, &mmap_node_list, )
	{
		if ((base == node->base) && (size == node->size)) {
			break;
		}
	}

	if ((node == NULL) || (node == mmap_brk_node)) {
		ret = -EINVAL;
		goto out;
	}

	// Check if any pages are still allocated, excluding metadata pages.
	index_t start_bit = (index_t)(node->meta_size / PAGE_SIZE);
	index_t end_bit	  = (index_t)(size / PAGE_SIZE) - 1U;
	index_t alloc_bit = 0U;
	if (bitmap_ffc(node->free_pages, start_bit, end_bit, &alloc_bit)) {
		ret = -EBUSY;
		goto out;
	}

	if (node_ret != NULL) {
		*node_ret = node;
	}

	ret = 0;

out:
	return ret;
}

int32_t
mmap_remove_heap(uintptr_t base, size_t size)
{
	int32_t	     ret;
	int32_t	     err;
	mmap_node_t *node = NULL;

	err = is_mmap_node_free(base, size, &node);
	if (err != 0) {
		ret = err;
		goto out;
	}

#if MMAP_ALLOC_TRACK
	// There should be no tracked allocations remaining.
	index_t start_bit = 0U;
	index_t end_bit	  = (index_t)(size / PAGE_SIZE) - 1U;
	index_t ffs_ret	  = 0U;
	assert(!bitmap_ffs(node->alloc_start, start_bit, end_bit, &ffs_ret));
	assert(!bitmap_ffs(node->alloc_end, start_bit, end_bit, &ffs_ret));
#endif

	mmap_total_size -= size;
	list_remove(mmap_node_t, &mmap_node_list, node, );
	ret = 0;

out:
	return ret;
}

int32_t
mmap_heap_is_free(uintptr_t base, size_t size)
{
	return is_mmap_node_free(base, size, NULL);
}

int32_t
mmap_heap_get_stats(allocator_stats_t *stats)
{
	size_t	reserved_size = 0U;
	count_t free_bits     = 0U;

	assert(stats != NULL);

	mmap_node_t *node = NULL;
	loop_list(node, &mmap_node_list, )
	{
		index_t curr_bit = 0U;
		index_t end_bit	 = (index_t)(node->size / PAGE_SIZE) - 1U;
		while (curr_bit <= end_bit) {
			// Find the next free page.
			index_t next_bit = 0U;
			if (!bitmap_ffs(node->free_pages, curr_bit, end_bit,
					&next_bit)) {
				// No free ranges left.
				break;
			}

			// Find the next allocated page after it.
			if (!bitmap_ffc(node->free_pages, next_bit, end_bit,
					&curr_bit)) {
				// Free range ends at the end of the bitmap.
				curr_bit = end_bit + 1U;
			}

			// Record the largest free range of pages.
			assert(curr_bit > next_bit);
			free_bits = util_max(curr_bit - next_bit, free_bits);
		}

		reserved_size += node->meta_size;
	}

	stats->info	    = allocator_stats_info_default();
	stats->total	    = mmap_total_size;
	stats->allocated    = mmap_alloc_size + (cur_brk - brk_base);
	stats->reserved	    = reserved_size;
	stats->largest_free = (size_t)free_bits * PAGE_SIZE;

	return 0;
}
