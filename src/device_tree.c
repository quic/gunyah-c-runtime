// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>

#include <types.h>

#include <guest_types.h>

#include <arch_def.h>
#include <log.h>

#include "device_tree.h"

#define ARGC 4
#define ENVC 1

extern rt_env_data_t *env_data;

extern size_t early_log_buf_size;

device_tree_info_t
parse_device_tree(void)
{
	static char *argv[ARGC];
	static char  exec_name[] = { "rootvm" };
	static char *envp[ENVC]	 = { exec_name };

	device_tree_info_t ret;

	argv[0] = envp[0];

	argv[1] = (char *)env_data + sizeof(rt_env_data_t);

	// The address of the log buffer so RM can report it back to hypervisor
	argv[2] = (char *)log_buf;
	argv[3] = (char *)early_log_buf_size;

	ret.argv = argv;
	ret.argc = ARGC;

	ret.envc = ENVC;
	ret.envp = envp;

	return ret;
}
