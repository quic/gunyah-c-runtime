// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>

#include <types.h>

#include <arch_def.h>
#include <guest_types.h>
#include <log.h>

#include "device_tree.h"

#define ARGC 4
static char *argv[ARGC];
#define ENVC 1
static char *envp[ENVC] = { "rootvm" };

extern boot_env_data_t *env_data;

extern size_t early_log_buf_size;

device_tree_info_t
parse_device_tree()
{
	device_tree_info_t ret;

	argv[0] = envp[0];

	// FIXME: For now we are getting the boot_env_data pointer instead of a
	// dtb. Eventually we will get a real dtb that will need to be parsed.
	argv[1] = (char *)env_data;

	// The address of the log buffer so RM can report it back to hypervisor
	argv[2] = (char *)log_buf;
	argv[3] = (char *)early_log_buf_size;

	ret.argv = argv;
	ret.argc = ARGC;

	ret.envc = ENVC;
	ret.envp = envp;

	return ret;
}
