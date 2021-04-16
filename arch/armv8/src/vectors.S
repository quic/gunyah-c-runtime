// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <asm/asm_defs.inc>
#include <el1.inc>
#include <errno.h>
#include <syscall_defs.h>

#define ESR_EC_SHIFT (26)
#define ESR_EC_SVC64 (0b010101)

#define ESR_ISS_SVC_IMM16_MASK (0xffff)

function handle_el1h_sync
	save_runtime_context_sync
	// Check if it's svc #0
	mrs	x9, ESR_EL1
	lsr	x10, x9, #ESR_EC_SHIFT
	and	x11, x9, #ESR_ISS_SVC_IMM16_MASK
	cmp	x10, #ESR_EC_SVC64
	ccmp	x11, #0, #0, eq
	// FIXME: might need a proper handling for this case
	b.ne	LOCAL(el1h_invalid)
	// check syscall table
	cmp	x8, NR_syscall_max
	b.hi	LOCAL(svc_not_implemented)
	adrl	x11, aarch64_sys_call_table
	ldr	x12, [x11, x8, lsl #3]
	cbz	x12, LOCAL(svc_not_implemented)
	blr	x12
local svc_return:
	restore_runtime_context_sync
	eret
local svc_not_implemented:
	mov	x0, #-ENOSYS
	b	LOCAL(svc_return)
local el1h_invalid:
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "EL1h sync exception not handled"
function_end handle_el1h_sync

function handle_el1h_irq
	save_runtime_context_irq
	bl 	interrupt_dispatch
	restore_runtime_context_irq
	eret
function_end handle_el1h_irq

	.section .text.vectors
	.balign 2048
.global vectors_aarch64
vectors_aarch64:

// only use Current EL with SPx sync vector,
// other's are jump print registers
// for general exception handling:
// save kernel context full
// panic
vector vector_el1t_sync
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "Current sp0 sync vector"
vector_end vector_el1t_sync

vector vector_el1t_irq
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "Current sp0 irq vector"
vector_end vector_el1t_irq

vector vector_el1t_fiq
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "Current sp0 fiq vector"
vector_end vector_el1t_fiq

vector vector_el1t_serror
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "Current sp0 serror vector"
vector_end vector_el1t_serror

vector vector_el1h_sync
	b	handle_el1h_sync
vector_end vector_el1h_sync

vector vector_el1h_irq
	b	handle_el1h_irq
vector_end vector_el1h_irq

vector vector_el1h_fiq
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "Current spx fiq vector"
vector_end vector_el1h_fiq

vector vector_el1h_serror
	save_runtime_context_full
	mov	x0, sp
	bl	dump_trap_frame
	panic_asm "Current spx serror vector"
vector_end vector_el1h_serror

vector vector_guest64_sync_panic
	save_runtime_context_full
	panic_asm "Guest 64 sync vector"
vector_end vector_guest64_sync_panic

vector vector_guest64_irq_panic
	save_runtime_context_full
	panic_asm "Guest 64 ieq vector"
vector_end vector_guest64_irq_panic

vector vector_guest64_fiq_panic
	save_runtime_context_full
	panic_asm "Guest 64 fiq vector"
vector_end vector_guest64_fiq_panic

vector vector_guest64_serror_panic
	save_runtime_context_full
	panic_asm "Guest 64 serror vector"
vector_end vector_guest64_serror_panic

vector vector_guest32_sync_panic
	save_runtime_context_full
	panic_asm "Guest 32 sync vector"
vector_end vector_guest32_sync_panic

vector vector_guest32_irq_panic
	save_runtime_context_full
	panic_asm "Guest 32 irq vector"
vector_end vector_guest32_irq_panic

vector vector_guest32_fiq_panic
	save_runtime_context_full
	panic_asm "Guest 32 fiq vector"
vector_end vector_guest32_fiq_panic

vector vector_guest32_serror_panic
	save_runtime_context_full
	panic_asm "Guest 32 serror vector"
vector_end vector_guest32_serror_panic
