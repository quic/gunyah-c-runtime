// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#define BITMAP_WORD_BITS    ((count_t)(sizeof(register_t) * (size_t)CHAR_BIT))
#define BITMAP_NUM_WORDS(x) (((x) + BITMAP_WORD_BITS - 1U) / BITMAP_WORD_BITS)

#define BITMAP_DECLARE(bits, name) register_t name[BITMAP_NUM_WORDS(bits)]

bool
bitmap_isset(const register_t *bitmap, index_t bit);

void
bitmap_set(register_t *bitmap, index_t bit);

void
bitmap_clear(register_t *bitmap, index_t bit);

register_t
bitmap_extract(const register_t *bitmap, index_t bit, index_t width);

void
bitmap_insert(register_t *bitmap, index_t bit, index_t width, register_t value);

bool
bitmap_ffs(const register_t *bitmap, index_t start_bit, index_t end_bit,
	   index_t *ret_bit);

bool
bitmap_ffc(const register_t *bitmap, index_t start_bit, index_t end_bit,
	   index_t *ret_bit);
