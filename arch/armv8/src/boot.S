// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <asm/asm_defs.inc>
#include <el1.inc>

#define BOOT_STACK_SIZE (4096)
#define CPACR_EL1_FPEN_MASK (0x3UL << 20)
#define CPACR_EL1_FPEN_ENABLE_EL1_SIMD (0x1UL << 20)

	.section .text.boot
function _start section=nosection
	// Entry arguments:
	// x0 = dtb address (currently not a DTB)

	// FIXME: the following are temporary until added to environment
	// x1 = application entry address
	// x2 = runtime base address
	// x3 = application memory base address (heap)
	// x4 = application memory size

	// Save the entry arguments
	adr	x9, dtb_addr
	str	x0, [x9]
	adr	x9, app_addr
	str	x1, [x9]
	adr	x9, runtime_base
	str	x2, [x9]
	adr	x9, app_memory
	str	x3, [x9]
	adr	x9, app_memory_size
	str	x4, [x9]

	// enable floating point and SIMD register access
	mrs	x0, CPACR_EL1
	bic	x0, x0, CPACR_EL1_FPEN_MASK
	orr	x0, x0, CPACR_EL1_FPEN_ENABLE_EL1_SIMD
	msr	CPACR_EL1, x0
	isb

	// assert current EL is el1
	mrs	x0, CurrentEL
	cmp	x0, (0x1UL << 2)
	bne	LOCAL(hang)
	// Set Vector Base Address
	adr	x1, vectors_aarch64
	msr	VBAR_EL1, x1

	// use SP_EL1
	msr	SPSel, 1

	// set initial boot stack
	adrl	x0, aarch64_boot_stack_bottom + BOOT_STACK_SIZE
	mov	sp, x0				// sp = stack_top (align with 4KB page)

	// relocate ourself first before calling C
	adrl	x0, _DYNAMIC
	adr	x9, runtime_base
	ldr	x1, [x9]
	bl	rel_fixup

	// Init runtime
	adr	x9, dtb_addr
	ldr	x0, [x9]
	bl 	interrupt_init
	bl	fs_init
	bl	timer_init

	adr	x9, app_addr
	ldr	x0, [x9]
	adr	x9, dtb_addr
	ldr	x1, [x9]
	// elf_setup returns application entry address
	bl	elf_setup

	// setup application stack
	// Runtime exceptions (syscalls etc) and apps (including resource manager)
	// would use the same stack.
	adr	x9, app_stack
	ldr	x10, [x9]
	mov	sp, x10

	// Enable IRQs
	msr	DAIFClr, 0x7

	// start application
	blr	x0

	// Application should not return here; it should call exit() instead.
	msr	DAIFSet, 0x7
	panic_asm "Returned from app entry"

local hang:
	wfi
	b	LOCAL(hang)
function_end _start

	.global aarch64_boot_stack_bottom
	.section .bss.boot_stack, "aw", @nobits
	.balign 16
	// This stack is used by runtime init. It must be large enough to run
	// all the initialization code. After this the runtime will use the
	// application stack.
aarch64_boot_stack_bottom:
	.space BOOT_STACK_SIZE

	.section ".data.elf_parameters", "aw"
	.align 8
dtb_addr:
	.space 8
app_addr:
	.space 8
app_entry:
	.space 8
runtime_base:
	.space 8
	.global app_memory
app_memory:
	.space 8
	.global app_memory_size
app_memory_size:
	.space 8
	.global app_heap
app_heap:
	.space 8
	.global app_stack
app_stack:
	.space 8
