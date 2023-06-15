// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>

#include <asm-generic/vector.h>

#include <guest_types.h>

#include <arch_def.h>
#include <console.h>
#include <log.h>
#include <panic.h>
#include <string_util.h>

#define SINGLE_LOG_MSG_SIZE (256)

noreturn void
assert_failed(const char *file, int line, const char *func, const char *err);

void
log_msg(uint64_t level, uint64_t action, const char *a1, size_t a2,
	register_t a3, register_t a4, register_t a5, register_t a6)
{
	char tmp_log[SINGLE_LOG_MSG_SIZE];

	// FIXME: ignore level and action
	(void)level;
	(void)action;
	size_result_t ret =
		snprint(tmp_log, SINGLE_LOG_MSG_SIZE, a1, a2, a3, a4, a5, a6);

	// send to console and log
	console_write(tmp_log, ret.r);
	log_append(tmp_log, ret.r);
}

#if !defined(NDEBUG)
noreturn void
assert_failed(const char *file, int line, const char *func, const char *err)
{
	LOG(ERROR, PANIC, "assertion: {:s} in func({:s}) file({:s}:{:d})\n",
	    (register_t)(uintptr_t)err, (register_t)(uintptr_t)func,
	    (register_t)(uintptr_t)file, (register_t)line);

	panic("Failure for assertion\n");
}
#endif
