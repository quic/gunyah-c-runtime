// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define TRAP_FRAME_FULL_SIZE (17 * 16)
#define TRAP_FRAME_SYNC_SIZE (11 * 16)
#define TRAP_FRAME_IRQ_SIZE  (12 * 16)

// Use this macro ONLY if jumping to panic or the kernel debugger afterwards.
.macro	save_runtime_context_full
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
	stp	x28, x29, [sp, 14 * 16]
	mrs	x0, SPSR_EL1
	stp	x30, x0, [sp, 15 * 16]
	mrs	x1, ELR_EL1
	mrs	x2, ESR_EL1
	stp	x1, x2, [sp, 16 * 16]
.endm

.macro save_runtime_context_sync
	sub	sp, sp, TRAP_FRAME_SYNC_SIZE

	stp	x1, x2, [sp, 0 * 16]
	stp	x3, x4, [sp, 1 * 16]
	stp	x5, x6, [sp, 2 * 16]
	stp	x7, x8, [sp, 3 * 16]
	stp	x9, x10, [sp, 4 * 16]
	stp	x11, x12, [sp, 5 * 16]
	stp	x13, x14, [sp, 6 * 16]
	stp	x15, x16, [sp, 7 * 16]
	stp	x17, x18, [sp, 8 * 16]
	stp	x29, x30, [sp, 9 * 16]
	mrs	x10, SPSR_EL1
	mrs	x9, ELR_EL1
	stp	x9, x10, [sp, 10 * 16]
.endm

.macro restore_runtime_context_sync
	ldp	x9, x10, [sp, 10 * 16]
	msr	ELR_EL1, x9
	msr	SPSR_EL1, x10

	ldp	x1, x2, [sp, 0 * 16]
	ldp	x3, x4, [sp, 1 * 16]
	ldp	x5, x6, [sp, 2 * 16]
	ldp	x7, x8, [sp, 3 * 16]
	ldp	x9, x10, [sp, 4 * 16]
	ldp	x11, x12, [sp, 5 * 16]
	ldp	x13, x14, [sp, 6 * 16]
	ldp	x15, x16, [sp, 7 * 16]
	ldp	x17, x18, [sp, 8 * 16]
	ldp	x29, x30, [sp, 9 * 16]

	add	sp, sp, TRAP_FRAME_SYNC_SIZE
.endm

.macro save_runtime_context_irq
	sub	sp, sp, TRAP_FRAME_IRQ_SIZE

	stp	x0, x1, [sp, 0 * 16]
	stp	x2, x3, [sp, 1 * 16]
	stp	x4, x5, [sp, 2 * 16]
	stp	x6, x7, [sp, 3 * 16]
	mrs 	x0, SPSR_EL1
	stp	x8, x9, [sp, 4 * 16]
	stp	x10, x11, [sp, 5 * 16]
	stp	x12, x13, [sp, 6 * 16]
	mrs 	x1, ELR_EL1
	stp	x14, x15, [sp, 7 * 16]
	stp	x16, x17, [sp, 8 * 16]
	stp	x18, x29, [sp, 9 * 16]
	stp 	x30, x0, [sp, 10 * 16]
	str 	x1, [sp, 11 * 16]
.endm

.macro restore_runtime_context_irq
	ldp	x30, x16, [sp, 10 * 16]
	ldr	x17, [sp, 11 * 16]
	ldp	x0, x1, [sp, 0 * 16]
	ldp	x2, x3, [sp, 1 * 16]
	ldp	x4, x5, [sp, 2 * 16]
	msr	SPSR_EL1, x16
	ldp	x6, x7, [sp, 3 * 16]
	ldp	x8, x9, [sp, 4 * 16]
	ldp	x10, x11, [sp, 5 * 16]
	ldp	x12, x13, [sp, 6 * 16]
	msr	ELR_EL1, x17
	ldp	x14, x15, [sp, 7 * 16]
	ldp	x16, x17, [sp, 8 * 16]
	ldp	x18, x29, [sp, 9 * 16]

	add sp, sp, TRAP_FRAME_IRQ_SIZE
.endm

.macro panic_asm, panic_str:req
.pushsection .rodata.panic_str\@, "aMS", @progbits, 1
local panic_str\@:
	.asciz	"\panic_str"
.popsection
	adr 	x0, LOCAL(panic_str\@)
	mov	x1, sp
	bl	panic
.endm
