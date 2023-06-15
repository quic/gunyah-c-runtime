// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <types.h>

#include <arch_def.h>
#include <console.h>
#include <log.h>

static struct log_s early_log_buf = { 0, EARLY_SIZE, { 0 } };

struct log_s *log_buf = &early_log_buf;

extern size_t early_log_buf_size;
size_t	      early_log_buf_size = sizeof(early_log_buf);

void
log_set_buffer(uintptr_t new_addr, size_t area_sz)
{
	assert(new_addr != 0U);
	assert(area_sz > (sizeof(size_t) * 2));
	assert(area_sz <= 524288);

	log_buf		   = (struct log_s *)new_addr;
	log_buf->write_idx = 0U;
	// Update size of log_buf->buffer[]
	log_buf->size =
		(uint32_t)(area_sz - sizeof(uint32_t) - sizeof(uint32_t));
}

void
log_append(const char *msg, size_t sz)
{
	assert(log_buf != NULL);

	if (sz >= log_buf->size) {
		// truncate the log message
		msg = msg + sz - (log_buf->size - 1U);
		sz  = log_buf->size - 1UL;
	}
	if (log_buf->write_idx + sz < log_buf->size) {
		(void)memcpy(log_buf->buffer + log_buf->write_idx, msg, sz);
		log_buf->write_idx += (uint32_t)sz;
	} else {
		size_t first_len, second_len;

		first_len =
			(size_t)(log_buf->size) - (size_t)log_buf->write_idx;
		second_len = sz - first_len;
		(void)memcpy(log_buf->buffer + log_buf->write_idx, msg,
			     first_len);

		if (second_len > 0U) {
			(void)memcpy(log_buf->buffer, msg + first_len,
				     second_len);
		}

		log_buf->write_idx = (uint32_t)second_len;
	}
	// Ensure log is always NULL terminated.
	log_buf->buffer[log_buf->write_idx] = '\0';
}
