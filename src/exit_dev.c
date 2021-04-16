// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stddef.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <exit_dev.h>
#include <fs.h>

static exit_t exit_func;

void
exit_trigger(int exit_code)
{
	if (exit_func != NULL) {
		exit_func(exit_code);
	}
}

int
exit_register(exit_t new_exit_func)
{
	int ret;

	if (exit_func != NULL) {
		ret = -EBUSY;
	} else {
		if (new_exit_func != NULL) {
			exit_func = new_exit_func;
			ret	  = 0;
		} else {
			ret = -EINVAL;
		}
	}

	return ret;
}

int
exit_deregister(void)
{
	int ret;

	if (exit_func == NULL) {
		ret = -ENOENT;
	} else {
		exit_func = NULL;
		ret	  = 0;
	}

	return ret;
}

static long
exit_ioctl(unsigned int cmd, unsigned long arg)
{
	long ret;

	switch (cmd) {
	case IOCTL_REGISTER_EXIT: {
		struct register_exit_req *data =
			(struct register_exit_req *)arg;
		ret = exit_register(data->exit_func);
		break;
	}
	case IOCTL_DEREGISTER_EXIT: {
		ret = exit_deregister();
		break;
	}
	default:
		ret = -ENOSYS;
		break;
	}

	return ret;
}

static struct fs_ops exit_ops = {
	.ioctl = exit_ioctl,
};

static struct file exit_file = {
	.ops = &exit_ops,
};

long
exit_open(int flags)
{
	return fs_alloc_fd(&exit_file, flags);
}
