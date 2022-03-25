// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>

#include <console.h>
#include <errno.h>
#include <fs.h>

static char    prefix[16];
static ssize_t prefix_len = 0;

static console_t console;

void
console_write(const char *msg, size_t sz)
{
	if (console != NULL) {
		if (msg == NULL) {
			if (prefix_len > 0) {
				console(prefix, (size_t)prefix_len);
			}
		} else {
			console(msg, sz);
		}
	}
}

int
console_register(console_t new_console)
{
	int ret;

	if (console != NULL) {
		ret = -EBUSY;
		goto out;
	}

	console = new_console;
	ret	= 0;

out:
	return ret;
}

int
console_deregister(void)
{
	int ret;

	if (console == NULL) {
		ret = -ENOENT;
		goto out;
	}

	console = NULL;
	ret	= 0;

out:
	return ret;
}

static long
console_ioctl(unsigned int cmd, unsigned long arg)
{
	long ret;

	switch (cmd) {
	case IOCTL_REGISTER_CONSOLE: {
		struct register_console_req *data =
			(struct register_console_req *)arg;
		ret = console_register(data->console);
		break;
	}
	case IOCTL_DEREGISTER_CONSOLE: {
		ret = console_deregister();
		break;
	}
	case IOCTL_SET_PREFIX_CONSOLE: {
		const char *uprefix = (char *)arg;
		prefix_len	    = strscpy(prefix, uprefix, 16);
		if (prefix_len < 0) {
			ret	   = prefix_len;
			prefix_len = 0;
		} else {
			ret = 0;
		}
		break;
	}
	default:
		ret = -ENOSYS;
		break;
	}

	return ret;
}

static struct fs_ops console_ops = {
	.ioctl = console_ioctl,
};

static struct file console_file = {
	.ops = &console_ops,
};

long
console_open(int flags)
{
	return fs_alloc_fd(&console_file, flags);
}
