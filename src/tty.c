// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>

#include <types.h>

#include <arch_def.h>
#include <console.h>
#include <errno.h>
#include <fs.h>
#include <guest_types.h>
#include <log.h>
#include <platform_timer.h>
#include <string_util.h>
#include <syscall_defs.h>
#include <tty.h>

#define TIMESTAMPS 1

extern void
ticks_to_timespec(uint64_t ticks, struct timespec *ts);

static long
tty_writev(const struct iovec *vecp, unsigned long vlen)
{
	index_t i;
	size_t	ret_len = 0;

	for (i = 0; i < vlen; ++i) {
		const struct iovec *v = vecp + i;

		size_t	    remain = v->iov_len;
		const char *head   = v->iov_base;

		do {
			static bool eol = true;

			if (eol) {
#if TIMESTAMPS
				uint64_t curticks =
					platform_timer_get_current_ticks();
				struct timespec ts;
				ticks_to_timespec(curticks, &ts);
				char stamp[16];
				snprint(stamp, sizeof(stamp),
					"{:d}.{:06d}: ", (register_t)ts.tv_sec,
					(register_t)ts.tv_nsec / 1000U, 0U, 0U,
					0U);

				log_append(stamp, strlen(stamp));
#endif
				console_write(NULL, 0);

				eol = false;
			}

			char  *next = memchr(head, '\n', remain);
			size_t len;
			if (next == NULL) {
				len = remain;
			} else {
				len = (size_t)(next - head) + 1;
				eol = true;
			}
			console_write(head, len);
			log_append(head, len);

			remain -= len;
			head = head + len;
		} while (remain > 0U);

		ret_len += v->iov_len;
	}

	return (long)ret_len;
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
	case TIOCSETBUF: { // Non-standard IOCTL!!
		struct tty_set_buffer_req *req =
			(struct tty_set_buffer_req *)arg;
		log_set_buffer(req->buffer, req->size);

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
