// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/types.h>

#include <asm-generic/vector.h>

#include <asm/event.h>

#include <arch_def.h>
#include <attributes.h>
#include <log.h>
#include <panic.h>
#include <syscall_defs.h>

void
dump_trap_frame(trap_frame_t *frame)
{
	LOG(ERROR, PANIC, "trap frame dump:\n");
	LOG(ERROR, PANIC, "x0: {:#8x}\t x1: {:#8x}\t x2: {:#8x}\t x3: {:#8x}\n",
	    frame->x[0], frame->x[1], frame->x[2], frame->x[3]);
	LOG(ERROR, PANIC, "x4: {:#8x}\t x5: {:#8x}\t x6: {:#8x}\t x7: {:#8x}\n",
	    frame->x[4], frame->x[5], frame->x[6], frame->x[7]);
	LOG(ERROR, PANIC, "x8: {:#8x}\t x9: {:#8x}\t x10:{:#8x}\t x11:{:#8x}\n",
	    frame->x[8], frame->x[9], frame->x[10], frame->x[11]);
	LOG(ERROR, PANIC, "x12:{:#8x}\t x13:{:#8x}\t x14:{:#8x}\t x15:{:#8x}\n",
	    frame->x[12], frame->x[13], frame->x[14], frame->x[15]);
	LOG(ERROR, PANIC, "x16:{:#8x}\t x17:{:#8x}\t x18:{:#8x}\t x19:{:#8x}\n",
	    frame->x[16], frame->x[17], frame->x[18], frame->x[19]);
	LOG(ERROR, PANIC, "x20:{:#8x}\t x21:{:#8x}\t x22:{:#8x}\t x23:{:#8x}\n",
	    frame->x[20], frame->x[21], frame->x[22], frame->x[23]);
	LOG(ERROR, PANIC, "x24:{:#8x}\t x25:{:#8x}\t x26:{:#8x}\t x27:{:#8x}\n",
	    frame->x[24], frame->x[25], frame->x[26], frame->x[27]);
	LOG(ERROR, PANIC, "x28:{:#8x}\t x29:{:#8x}\t x30:{:#8x}\n",
	    frame->x[28], frame->x[29], frame->x[30]);
	LOG(ERROR, PANIC, "spsr_el1:{:#8x}\t elr_el1:{:#8x}\t esr_el1:{:#8x}\n",
	    frame->spsr, frame->elr, frame->esr);
}

noreturn void NOINLINE
panic(const char *str)
{
	void *from  = __builtin_return_address(0);
	void *frame = __builtin_frame_address(0);

	LOG(ERROR, PANIC, "Panic: {:s} from PC {:#x}, FP {:#x}\n",
	    (register_t)(uintptr_t)str, (register_t)(uintptr_t)from,
	    (register_t)(uintptr_t)frame);

	sys_exit(1);
}
