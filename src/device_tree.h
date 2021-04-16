// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

typedef struct {
	size_t argc;
	char **argv;
	size_t envc;
	char **envp;
} device_tree_info_t;

device_tree_info_t
parse_device_tree(uintptr_t dtb);
