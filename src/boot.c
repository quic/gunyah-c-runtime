// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdint.h>

#include <guest_types.h>

#include <fs.h>
#include <guest_interface.h>
#include <interrupt.h>
#include <timer.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-identifier"
extern uintptr_t	 __stack_chk_guard;
uintptr_t __stack_chk_guard __attribute__((used, visibility("hidden")));
#pragma clang diagnostic pop

void
runtime_init(void);

// We must disable stack protection for this function, as the compiler may not
// optimise the call to gunyah_hyp_prng_get_entropy() and place the result on
// the stack, which will trigger the use of the stack canary before it is
// initialised and lead to a stack check failure.
__attribute__((no_stack_protector)) void
runtime_init(void)
{
	// Get some entropy and init the stack canary.
	gunyah_hyp_prng_get_entropy_result_t ret =
		gunyah_hyp_prng_get_entropy(8U);

	uint64_t guard = (ret.error == OK)
				 ? (ret.data0 | ((uint64_t)ret.data1 << 32))
				 : 0x1f2c38a9b459e37dUL;

	__stack_chk_guard = (uintptr_t)guard;

	// Initialise the rest of the runtime.
	interrupt_init();
	fs_init();
	timer_init();
}
