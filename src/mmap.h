// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

int32_t
mmap_add_heap(uintptr_t base, size_t size, bool is_brk);

int32_t
mmap_remove_heap(uintptr_t base, size_t size);

int32_t
mmap_heap_is_free(uintptr_t base, size_t size);

int32_t
mmap_heap_get_stats(allocator_stats_t *stats);
