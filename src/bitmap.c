// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <limits.h>

#include <guest_types.h>

#include <bitmap.h>
#include <compiler.h>
#include <util.h>

#define BITMAP_SET_BIT(x) ((register_t)1U << (((x) % BITMAP_WORD_BITS)))
#define BITMAP_WORD(x)	  ((x) / BITMAP_WORD_BITS)
#define BITMAP_SIZE_ASSERT(bitmap, bit)                                        \
	assert((index_t)(compiler_sizeof_object(bitmap) /                      \
			 sizeof(register_t)) > BITMAP_WORD(bit))

bool
bitmap_isset(const register_t *bitmap, index_t bit)
{
	BITMAP_SIZE_ASSERT(bitmap, bit);

	index_t i = BITMAP_WORD(bit);

	return (bitmap[i] & BITMAP_SET_BIT(bit)) != 0U;
}

void
bitmap_set(register_t *bitmap, index_t bit)
{
	BITMAP_SIZE_ASSERT(bitmap, bit);

	index_t i = BITMAP_WORD(bit);

	bitmap[i] |= BITMAP_SET_BIT(bit);
}

void
bitmap_clear(register_t *bitmap, index_t bit)
{
	BITMAP_SIZE_ASSERT(bitmap, bit);

	index_t i = BITMAP_WORD(bit);

	bitmap[i] &= ~BITMAP_SET_BIT(bit);
}

register_t
bitmap_extract(const register_t *bitmap, index_t bit, index_t width)
{
	BITMAP_SIZE_ASSERT(bitmap, bit + width - 1U);
	assert((width <= BITMAP_WORD_BITS) &&
	       (BITMAP_WORD(bit) == BITMAP_WORD(bit + width - 1U)));

	index_t	   i	= BITMAP_WORD(bit);
	register_t mask = util_mask_safe(width);

	return (bitmap[i] >> (bit % BITMAP_WORD_BITS)) & mask;
}

void
bitmap_insert(register_t *bitmap, index_t bit, index_t width, register_t value)
{
	BITMAP_SIZE_ASSERT(bitmap, bit + width - 1U);
	assert((width <= BITMAP_WORD_BITS) &&
	       (BITMAP_WORD(bit) == BITMAP_WORD(bit + width - 1U)));

	index_t	   i	= BITMAP_WORD(bit);
	register_t mask = util_mask_safe(width);

	bitmap[i] &= ~(mask << (bit % BITMAP_WORD_BITS));
	bitmap[i] |= (value & mask) << (bit % BITMAP_WORD_BITS);
}

static bool
bitmap_find_first(const register_t *bitmap, index_t start_bit, index_t end_bit,
		  bool set, index_t *ret_bit)
{
	bool ret = false;

	assert(start_bit <= end_bit);
	BITMAP_SIZE_ASSERT(bitmap, end_bit);

	index_t i     = BITMAP_WORD(start_bit);
	count_t words = BITMAP_NUM_WORDS(end_bit + 1U);

	for (; i < words; i++) {
		register_t x = set ? bitmap[i] : ~bitmap[i];
		if (i == BITMAP_WORD(start_bit)) {
			// Mask out the bits to be skipped.
			x &= ~util_mask(start_bit % BITMAP_WORD_BITS);
		}

		if (x != 0U) {
			index_t j = compiler_ctz(x);
			assert(j < BITMAP_WORD_BITS);

			index_t bit = (i * BITMAP_WORD_BITS) + j;
			if (bit <= end_bit) {
				*ret_bit = bit;
				ret	 = true;
			}
			break;
		}
	}

	return ret;
}

bool
bitmap_ffs(const register_t *bitmap, index_t start_bit, index_t end_bit,
	   index_t *ret_bit)
{
	return bitmap_find_first(bitmap, start_bit, end_bit, true, ret_bit);
}

bool
bitmap_ffc(const register_t *bitmap, index_t start_bit, index_t end_bit,
	   index_t *ret_bit)
{
	return bitmap_find_first(bitmap, start_bit, end_bit, false, ret_bit);
}
