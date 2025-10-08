// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <guest_types.h>

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
	assert(area_sz > (sizeof(size_t) * 2U));
	assert(area_sz <= 524288U);

	log_buf		   = (struct log_s *)new_addr;
	log_buf->write_idx = 0U;
	// Update size of log_buf->buffer[]
	log_buf->size =
		(uint32_t)(area_sz - sizeof(uint32_t) - sizeof(uint32_t));
}

void
log_append(const char *msg, size_t sz)
{
	uint32_t    checked_sz;
	const char *msg_buf = msg;
	assert(log_buf != NULL);
	assert(log_buf->size != 0U);

	if ((sz >= log_buf->size) && (log_buf->size != 0U)) {
		// truncate the log message
		msg_buf	   = msg_buf + (sz - ((size_t)log_buf->size - 1U));
		checked_sz = log_buf->size - (uint32_t)1;
	} else {
		checked_sz = (uint32_t)sz;
	}

	size_t first_len =
		memscpy(log_buf->buffer + log_buf->write_idx,
			(size_t)(log_buf->size - (size_t)log_buf->write_idx),
			msg_buf, (size_t)checked_sz);
	size_t second_len = (size_t)checked_sz - first_len;

	if (second_len > 0U) {
		(void)memscpy(log_buf->buffer, log_buf->size,
			      msg_buf + first_len, second_len);
	}

	log_buf->write_idx += checked_sz;
	if (log_buf->write_idx >= log_buf->size) {
		log_buf->write_idx -= log_buf->size;
	}

	// Ensure log is always NULL terminated.
	log_buf->buffer[log_buf->write_idx] = '\0';
}
