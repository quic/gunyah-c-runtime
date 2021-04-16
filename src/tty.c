// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <types.h>

#include <arch_def.h>
#include <console.h>
#include <errno.h>
#include <fs.h>
#include <log.h>
#include <syscall_defs.h>
#include <tty.h>

static long
tty_writev(const struct iovec *vecp, unsigned long vlen)
{
	index_t i;
	size_t	len = 0;

	for (i = 0; i < vlen; ++i) {
		const struct iovec *v = vecp + i;
		console_write((const char *)v->iov_base, v->iov_len);
		log_append((const char *)v->iov_base, v->iov_len);

		len += v->iov_len;
	}

	return (long)len;
}

static long
tty_ioctl(unsigned int cmd, unsigned long arg)
{
	long ret;

	switch (cmd) {
	case TIOCGWINSZ: {
		// FIXME: should we refine it with real value & handle
		// it in a separate function?
		struct winsize *w = (struct winsize *)arg;
		w->ws_row	  = 64;
		w->ws_col	  = 96;
		w->ws_xpixel	  = 1280;
		w->ws_ypixel	  = 760;

		ret = 0L;
		break;
	}

	default:
		LOG(ERROR, MSG, "{:s}: invalid tty ioctl cmd{:#x}\n",
		    (register_t) __func__, cmd);

		ret = -EINVAL;
		break;
	}

	return ret;
}

static struct fs_ops ttyin_ops = {
	.ioctl = &tty_ioctl,
};

static struct fs_ops ttyout_ops = {
	.ioctl	= &tty_ioctl,
	.writev = &tty_writev,
};

static struct file ttyin = {
	.ops = &ttyin_ops,
};

static struct file ttyout = {
	.ops = &ttyout_ops,
};

struct file *stdin_file	 = &ttyin;
struct file *stdout_file = &ttyout;
struct file *stderr_file = &ttyout;
