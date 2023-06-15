// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define TRAP_FRAME_FULL_SIZE    (18 * 16)
#define TRAP_FRAME_SYSCALL_SIZE (11 * 16)
#define TRAP_FRAME_IRQ_SIZE     (12 * 16)

// Use this macro ONLY if jumping to panic afterwards.
//
// There is no macro to return from a full stack frame, but dump_trap_frame()
// prints it to the console.
.macro	save_runtime_context_full
	// Check for stack overflow, and branch to handle_stack_fault
	// FIXME:

	// SP (SP_EL1) points to the kernel stack
	sub	sp, sp, TRAP_FRAME_FULL_SIZE

	stp	x0, x1, [sp, 0 * 16]
	stp	x2, x3, [sp, 1 * 16]
	stp	x4, x5, [sp, 2 * 16]
	stp	x6, x7, [sp, 3 * 16]
	stp	x8, x9, [sp, 4 * 16]
	stp	x10, x11, [sp, 5 * 16]
	stp	x12, x13, [sp, 6 * 16]
	stp	x14, x15, [sp, 7 * 16]
	stp	x16, x17, [sp, 8 * 16]
	stp	x18, x19, [sp, 9 * 16]
	stp	x20, x21, [sp, 10 * 16]
	stp	x22, x23, [sp, 11 * 16]
	stp	x24, x25, [sp, 12 * 16]
	stp	x26, x27, [sp, 13 * 16]
	stp	x28, x30, [sp, 14 * 16]
	mrs	x0, SPSR_EL1
	mrs	x1, ESR_EL1
	stp	x0, x1, [sp, 15 * 16]
	mrs	x0, SP_EL0
	add	x1, sp, TRAP_FRAME_FULL_SIZE	// original SP_EL1
	stp	x0, x1, [sp, 16 * 16]
	mrs	x1, ELR_EL1
	stp	x29, x1, [sp, 17 * 16]
	add     x29, sp, 17 * 16
.endm

// Save an exception stack frame containing fp, lr, spsr, elr, and x1-x18.
// The caller should have decremented SP already.
.macro save_runtime_context_x1_x18
	// This macro must not touch any of the syscall argument registers
	// x0-x7 other than to save them. Also, ELR_EL1 needs to be saved via
	// x30 so we can use the hint-encoded PACIASP instruction, which
	// executes as NOP on older CPUs.
	stp	x17, x18, [sp, 1 * 16]
	stp	x1, x2, [sp, 2 * 16]
	stp	x3, x4, [sp, 3 * 16]
	mrs	x17, SPSR_EL1
	stp	x5, x6, [sp, 4 * 16]
	stp	x30, x17, [sp, 5 * 16]
	stp	x7, x8, [sp, 6 * 16]
	mrs	x30, ELR_EL1
	stp	x9, x10, [sp, 7 * 16]
	paciasp
	stp	x11, x12, [sp, 8 * 16]
	stp	x13, x14, [sp, 9 * 16]
	stp	x15, x16, [sp, 10 * 16]
	stp	x29, x30, [sp]
	mov     x29, sp
.endm

// Restore an exception stack frame saved by save_runtime_context_x1_x18.
// The caller should increment SP afterwards.
.macro restore_runtime_context_x1_x18
	// ELR_EL1 needs to be restored via x30 so we can use the hint-encoded
	// AUTIASP instruction, which executes as NOP on older CPUs.
	ldp	x29, x30, [sp]
	ldp	x17, x18, [sp, 1 * 16]
	ldp	x1, x2, [sp, 2 * 16]
	autiasp
	ldp	x3, x4, [sp, 3 * 16]
	msr	ELR_EL1, x30
	ldp	x5, x6, [sp, 4 * 16]
	ldp	x30, x15, [sp, 5 * 16]
	ldp	x7, x8, [sp, 6 * 16]
	ldp	x9, x10, [sp, 7 * 16]
	ldp	x11, x12, [sp, 8 * 16]
	msr	SPSR_EL1, x15
	ldp	x13, x14, [sp, 9 * 16]
	ldp	x15, x16, [sp, 10 * 16]
.endm

.macro save_runtime_context_syscall
	sub	sp, sp, TRAP_FRAME_SYSCALL_SIZE
	save_runtime_context_x1_x18
	// x0 is not saved because it is used for the syscall return value.
.endm

.macro restore_runtime_context_syscall
	restore_runtime_context_x1_x18
	// x0 is not restored because it contains the syscall return value.
	add	sp, sp, TRAP_FRAME_SYSCALL_SIZE
.endm

.macro save_runtime_context_irq
	sub	sp, sp, TRAP_FRAME_IRQ_SIZE
	save_runtime_context_x1_x18
	stp 	x0, xzr, [sp, 11 * 16]
.endm

.macro restore_runtime_context_irq
	restore_runtime_context_x1_x18
	ldr 	x0, [sp, 11 * 16]
	add sp, sp, TRAP_FRAME_IRQ_SIZE
.endm

.macro panic_asm, panic_str:req
.pushsection .rodata.panic_str\@, "aMS", @progbits, 1
local panic_str\@:
	.asciz	"\panic_str"
.popsection
	adr 	x0, LOCAL(panic_str\@)
	bl	panic
.endm
