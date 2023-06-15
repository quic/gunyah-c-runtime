// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <asm/asm_defs.inc>
#include <el1.inc>

#define BOOT_STACK_SIZE (1024)
#define BOOT_STACK_CANARY 0xeb3a23d54a5267ab

#define CPACR_EL1_FPEN_MASK (0x3UL << 20)
#define CPACR_EL1_FPEN_ENABLE_EL1_SIMD (0x1UL << 20)
#define SCTLR_EL1_ENIA_MASK (0x1U << 31)

#define SMCCC_FAST_CALL_SHIFT	31
#define SMCCC_64BIT_CALL_SHIFT	30
#define SMCCC_SERVICE_SHIFT	24
#define SMCCC_FUNCTION_SHIFT	0

#define SMCCC_SERVICE_ARM		0
#define SMCCC_SERVICE_CPU		1
#define SMCCC_SERVICE_SIP		2
#define SMCCC_SERVICE_OEM		3
#define SMCCC_SERVICE_STANDARD_SECURE	4
#define SMCCC_SERVICE_STANDARD_HYP	5
#define SMCCC_SERVICE_VENDOR_HYP	6

#define SMCCC_FUNCTION_ID(fast, call64, service, function)	\
	((((fast) != 0) << SMCCC_FAST_CALL_SHIFT) |		\
	(((call64) != 0) << SMCCC_64BIT_CALL_SHIFT) |	\
	((service) << SMCCC_SERVICE_SHIFT) |			\
	((function) << SMCCC_FUNCTION_SHIFT))

	.section .text.boot
function _start section=nosection
	// Entry arguments:
	// x0 = boot_env_data

	// Save the entry arguments
	adr	x9, env_data
	str	x0, [x9]

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
	adrl	x0, aarch64_boot_stack_bottom
	add	sp, x0, BOOT_STACK_SIZE

	// Set a canary at the end of the stack
	abs64	x1, BOOT_STACK_CANARY
	stp	x1, x1, [x0]

	// relocate ourself first before calling C
	adrl	x0, _DYNAMIC
	adr	x9, env_data
	ldr	x1, [x9]
	bl	rel_fixup

        // Set up the PAuth IA key (the only one currently used) and enable
        // PAuth, if the CPU supports it. Failure to obtain random data is
	// not tolerated.
	mrs	x0, ID_AA64ISAR1_EL1
	tst	x0, 0xff0 // API and APA fields; both 0 means no PAC support
        b.eq    LOCAL(pac_done)

	mov	x0, 16	// num_bytes: 16 bytes / 128-bits
	mov	x1, 0	// res0
	// gunyah_hyp_prng_get_entropy(0x6057, num_bytes, res0)
	hvc	0x6057
	cbnz	x0, LOCAL(hang)

	bfi	x1, x2, 32, 32
	bfi	x3, x4, 32, 32

.arch armv8.3-a // LLVM doesn't understand .arch_extension pauth yet
	msr	APIAKeyLo_EL1, x1
	msr	APIAKeyHi_EL1, x3

	mrs	x0, SCTLR_EL1
	orr	x0, x0, SCTLR_EL1_ENIA_MASK
	msr	SCTLR_EL1, x0
	isb
.arch armv8.2-a
local pac_done:

	// Init runtime
	bl	runtime_init

	// elf_setup returns application entry address
	// returned x0 is app entry. It will blr to that entry after
	// everything is done
	bl	elf_setup

	// Check boot stack canary
	adrl	x10, aarch64_boot_stack_bottom
	abs64	x11, BOOT_STACK_CANARY
	ldp	x12, x13, [x10]
	cmp	x11, x12
	ccmp	x11, x13, #0, eq
	b.ne	LOCAL(boot_stack_overflow)

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

local boot_stack_overflow:
	panic_asm "Detected boot stack overflow"
local hang:
	// FIXME:
	wfe
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
	.global env_data
env_data:
	.space 8
	.global app_stack
app_stack:
	.space 8
