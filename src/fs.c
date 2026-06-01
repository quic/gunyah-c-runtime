// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/types.h>

#include <guest_types.h>

#include <arch_def.h>
#include <bitmap.h>
#include <compiler.h>
#include <console.h>
#include <errno.h>
#include <exit_dev.h>
#include <fcntl.h>
#include <fs.h>
#include <interrupt.h>
#include <mem.h>
#include <syscall_defs.h>
#include <tty.h>
#include <unistd.h>

#define NUM_FDS 32U

static BITMAP_DECLARE(NUM_FDS, fd_alloc);
static struct file_p file_table[NUM_FDS];

void
fs_init(void)
{
	bitmap_set(fd_alloc, STDIN_FILENO);
	bitmap_set(fd_alloc, STDOUT_FILENO);
	bitmap_set(fd_alloc, STDERR_FILENO);

	file_table[STDIN_FILENO] = (struct file_p){
		.file  = stdin_file,
		.flags = FS_READ,
	};
	file_table[STDOUT_FILENO] = (struct file_p){
		.file  = stdout_file,
		.flags = FS_WRITE,
	};
	file_table[STDERR_FILENO] = (struct file_p){
		.file  = stderr_file,
		.flags = FS_WRITE,
	};
}

int32_t
fs_alloc_fd(struct file_s *f, uint32_t o_flags)
{
	int32_t	 ret;
	uint32_t fd;

	assert(f != NULL);

	if (!bitmap_ffc(fd_alloc, 0U, NUM_FDS - 1U, &fd)) {
		ret = -ENFILE;
		goto out;
	}

	bitmap_set(fd_alloc, fd);

	uint64_t flags	 = 0U;
	uint32_t accmode = o_flags & (uint32_t)O_ACCMODE;
	if ((accmode == (uint32_t)O_RDONLY) || (accmode == (uint32_t)O_RDWR)) {
		flags |= FS_READ;
	}
	if ((accmode == (uint32_t)O_WRONLY) || (accmode == (uint32_t)O_RDWR)) {
		flags |= FS_WRITE;
	}

	file_table[fd].file  = f;
	file_table[fd].flags = flags;

	ret = (int32_t)fd;

out:
	return ret;
}

struct file_p *
fs_lookup_file(uint32_t fd)
{
	struct file_p *fp = NULL;

	if ((fd < NUM_FDS) && (file_table[fd].file != NULL)) {
		fp = &file_table[fd];
	}

	return fp;
}

int32_t
sys_openat(int32_t dirfd, const char *pathname, uint32_t flags, uint32_t mode)
{
	int32_t ret;

	if (dirfd != AT_FDCWD) {
		ret = -EINVAL;
		goto out;
	}

	if (((flags & ~((uint32_t)O_ACCMODE | (uint32_t)O_LARGEFILE)) != 0U) ||
	    (mode != 0U)) {
		ret = -EINVAL;
		goto out;
	}

	if (strcmp(pathname, PLATFORM_IRQ_DEV_PATH) == 0) {
		ret = interrupt_open(flags);
	} else if (strcmp(pathname, PLATFORM_CONSOLE_DEV_PATH) == 0) {
		ret = console_open(flags);
	} else if (strcmp(pathname, PLATFORM_EXIT_DEV_PATH) == 0) {
		ret = exit_open(flags);
	} else if (strcmp(pathname, MEM_DEV_PATH) == 0) {
		ret = mem_open(flags);
	} else {
		ret = -ENOENT;
	}

out:
	return ret;
}

int32_t
sys_close(uint32_t fd)
{
	int32_t ret;

	struct file_p *fp = fs_lookup_file(fd);
	if (fp == NULL) {
		ret = -EBADF;
		goto out;
	}

	(void)memset(fp, 0, sizeof(*fp));
	bitmap_clear(fd_alloc, fd);
	ret = 0;

out:
	return ret;
}
