// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <types.h>

#include <elf.h>

#include <guest_types.h>

#include <arch_def.h>
#include <guest_interface.h>
#include <util.h>

#include "device_tree.h"
#include "mmap.h"

// FIXME: should we expose certain hw caps to app
#define ELF_HWCAP (0x0)

extern uintptr_t app_stack;

extern rt_env_data_t *env_data;

typedef uint64_t elf_addr_t;

// FIXME: test code, remove it
static const char *app_name = "app";

static uint32_t aux_entropy[4];

// return entry start address
uintptr_t
elf_setup(void);

// initial auxiliary vector table
static uintptr_t
setup_auxvector(uint64_t **stack, uint64_t app_addr);

static void
setup_envs(uint64_t **stack, int envc, char **envp);

static void
setup_arguments(uint64_t **stack, int argc, char **arg_values);

static bool
get_aux_entropy(void)
{
	gunyah_hyp_prng_get_entropy_result_t ret;

	ret = gunyah_hyp_prng_get_entropy(16U);
	if (ret.error == OK) {
		aux_entropy[0] = ret.data0;
		aux_entropy[1] = ret.data1;
		aux_entropy[2] = ret.data2;
		aux_entropy[3] = ret.data3;
	}

	return ret.error == OK;
}

static uint64_t *
fill_entry(uint64_t *stack, uint64_t value)
{
	uint64_t *p = stack - 1;
	*p	    = value;
	return p;
}

static uint64_t *
fill_auxv_entry(uint64_t *stack, uint64_t key, uint64_t value)
{
	uint64_t *p = stack;

	p = fill_entry(p, value);
	p = fill_entry(p, key);
	return p;
}

// FIXME: should we use asmlinkage for stack only
static uintptr_t
setup_auxvector(uint64_t **stack, uintptr_t app_addr)
{
	Elf64_Ehdr *ehdr	 = (Elf64_Ehdr *)app_addr;
	uintptr_t   phdr_addr	 = ehdr->e_phoff + app_addr;
	size_t	    ph_entry_cnt = ehdr->e_phnum;
	uintptr_t   entry_addr	 = ehdr->e_entry + app_addr;
	uint64_t   *p		 = *stack;

	p = fill_auxv_entry(p, AT_PAGESZ, PAGE_SIZE);

	p = fill_auxv_entry(p, AT_PHDR, phdr_addr);
	p = fill_auxv_entry(p, AT_PHENT, sizeof(Elf64_Phdr));
	p = fill_auxv_entry(p, AT_PHNUM, ph_entry_cnt);
	p = fill_auxv_entry(p, AT_EXECFN, (uintptr_t)app_name);

	if (get_aux_entropy()) {
		p = fill_auxv_entry(p, AT_RANDOM, (uintptr_t)aux_entropy);
	}

	// update stack pointer
	*stack = p;

	return entry_addr;
}

static void
setup_envs(uint64_t **stack, int envc, char **envp)
{
	uint64_t *p   = *stack;
	int	  idx = 0;

	assert(envc >= 0);

	// set terminator at the bottom
	p = fill_entry(p, 0UL);

	// fill envp
	for (idx = envc - 1; idx >= 0; idx--) {
		p = fill_entry(p, (uintptr_t)envp[idx]);
	}

	// update stack pointer
	*stack = p;
}

static void
setup_arguments(uint64_t **stack, int argc, char **arg_values)
{
	uint64_t *p   = *stack;
	int	  idx = 0;

	assert(argc >= 0);

	// set stack
	p = fill_entry(p, 0UL);
	for (idx = argc - 1; idx >= 0; idx--) {
		p = fill_entry(p, (uintptr_t)arg_values[idx]);
	}
	p = fill_entry(p, (uint64_t)argc);

	// update stack pointer
	*stack = p;
}

// FIXME: we assume 16KiB stack is sufficient
#define STACK_SIZE (16UL * 1024UL)

uintptr_t
elf_setup(void)
{
	uint64_t  app_address	  = env_data->app_ipa;
	size_t	  app_memory_size = env_data->app_heap_size;
	uintptr_t app_memory	  = env_data->app_heap_ipa;

	// Place the stack below the heap to avoid collisions
	assert(app_memory_size > STACK_SIZE);
	app_stack = app_memory + STACK_SIZE;

	// Add canary for overflow detection
	(void)memset((void *)app_memory, 0xeb, 64);

	app_memory += STACK_SIZE;
	app_memory_size -= STACK_SIZE;

	// calculate application stack and heap
	uintptr_t app_heap = app_memory;
	init_heap(app_heap, app_memory_size);

	// parse dtb to get commandline, arg count, env count, envs
	device_tree_info_t device_tree_info;
	device_tree_info = parse_device_tree();

	uint64_t *stack = (uint64_t *)app_stack;

	// Set the end-marker
	stack = fill_entry(stack, 0UL);

	// FIXME: push argument and env strings here
	// ensure stack is aligned down back to uint64_t after this

	// Add padding to ensure we end with a 16-byte aligned stack.
	// argv and envp are the two variables to be concerned with.
	// We don't actually count auxv entries since each is already 16-byte
	// sized.
	uintptr_t aux_size_var =
		(2UL /* argc + NULL */ + device_tree_info.argc /* argv[] */ +
		 device_tree_info.envc + 1UL /* envp[term] */) *
		sizeof(uintptr_t);

	count_t pad = (count_t)(((aux_size_var + (uintptr_t)stack) % 16UL) /
				sizeof(uintptr_t));

	while (pad != 0U) {
		stack = fill_entry(stack, 0UL);
		pad -= 1U;
	}

	uintptr_t app_entry;
	app_entry = setup_auxvector(&stack, app_address);

	setup_envs(&stack, (int)device_tree_info.envc, device_tree_info.envp);

	setup_arguments(&stack, (int)device_tree_info.argc,
			device_tree_info.argv);

	// check stack is aligned to 16-bytes
	assert(util_is_baligned((uintptr_t)stack, 16U));

	app_stack = (uintptr_t)stack;

	return app_entry;
}
