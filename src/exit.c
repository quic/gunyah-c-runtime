// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>

#include <guest_types.h>

#include <asm/interrupt.h>

#include <arch_def.h>
#include <exit_dev.h>
#include <log.h>
#include <platform_irq.h>
#include <syscall_defs.h>

static noreturn void
exit_loop(int ec)
{
	LOG(ERROR, PANIC, "sys_exit/sys_exit_group: called\n");

	platform_irq_disable_all();
	exit_trigger(ec);

	for (;;) {
		// FIXME:
		asm_wait_for_event(&ec);
	}
}

asmlinkage noreturn void
sys_exit(int ec)
{
	exit_loop(ec);
}

asmlinkage noreturn void
sys_exit_group(int ec)
{
	exit_loop(ec);
}
