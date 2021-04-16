// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <types.h>

#include <arch_def.h>
#include <console.h>
#include <log.h>

#define MSG_SZ (2048)

extern struct log log_buf;

struct log {
	index_t write_idx;
	char	buffer[MSG_SZ];
} log_buf;

extern size_t log_buf_size;

size_t log_buf_size = sizeof(log_buf);

void
log_append(const char *msg, size_t sz)
{
	if (sz >= MSG_SZ) {
		msg = msg + sz - (MSG_SZ - 1);
		sz  = MSG_SZ - 1;
	}
	if (log_buf.write_idx + sz < MSG_SZ) {
		memcpy(log_buf.buffer + log_buf.write_idx, msg, sz);
		log_buf.write_idx += sz;
	} else {
		size_t first_len, second_len;

		first_len  = MSG_SZ - log_buf.write_idx;
		second_len = sz - first_len;
		memcpy(log_buf.buffer + log_buf.write_idx, msg, first_len);

		if (second_len) {
			memcpy(log_buf.buffer, msg + first_len, second_len);
		}

		log_buf.write_idx = (index_t)second_len;
	}
	log_buf.buffer[log_buf.write_idx] = '\0';
}
