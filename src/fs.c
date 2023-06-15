// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
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

#include <types.h>

#include <guest_types.h>

#include <arch_def.h>
#include <compiler.h>
#include <console.h>
#include <errno.h>
#include <exit_dev.h>
#include <fcntl.h>
#include <fs.h>
#include <interrupt.h>
#include <syscall_defs.h>
#include <tty.h>
#include <unistd.h>

#define NUM_FDS 32U

static register_t fd_alloc;
static_assert(sizeof(register_t) * CHAR_BIT >= NUM_FDS,
	      "Too many FDs for allocator");

static struct file_p file_table[NUM_FDS];

void
fs_init(void)
{
	fd_alloc = (1U << STDIN_FILENO) | (1U << STDOUT_FILENO) |
		   (1U << STDERR_FILENO);
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

long
fs_alloc_fd(struct file *f, int o_flags)
{
	long	      ret;
	unsigned long fd;

	assert(f != NULL);

	if (fd_alloc == 0U) {
		fd = 0;
	} else {
		fd = compiler_ctz(~fd_alloc);
	}

	if (fd >= NUM_FDS) {
		ret = -ENFILE;
		goto out;
	}

	fd_alloc |= 1UL << fd;

	uint64_t flags	 = 0U;
	uint32_t accmode = (uint32_t)o_flags & (uint32_t)O_ACCMODE;
	if ((accmode == (uint32_t)O_RDONLY) || (accmode == (uint32_t)O_RDWR)) {
		flags |= (uint64_t)FS_READ;
	}
	if ((accmode == (uint32_t)O_WRONLY) || (accmode == (uint32_t)O_RDWR)) {
		flags |= (uint64_t)FS_WRITE;
	}

	file_table[fd].file  = f;
	file_table[fd].flags = (long)flags;
	ret		     = (long)fd;

out:
	return ret;
}

struct file_p *
fs_lookup_file(unsigned long fd)
{
	struct file_p *fp = NULL;

	if (fd < NUM_FDS && file_table[fd].file != NULL) {
		fp = &file_table[fd];
	}

	return fp;
}

asmlinkage long
sys_openat(int dirfd, const char *pathname, int flags, umode_t mode)
{
	long ret;

	if (dirfd != AT_FDCWD) {
		ret = -EINVAL;
		goto out;
	}

	if (((uint32_t)flags &
	     ~((uint32_t)O_ACCMODE | (uint32_t)O_LARGEFILE)) != 0U ||
	    mode != 0U) {
		ret = -EINVAL;
		goto out;
	}

	if (strcmp(pathname, PLATFORM_IRQ_DEV_PATH) == 0) {
		ret = interrupt_open(flags);
	} else if (strcmp(pathname, PLATFORM_CONSOLE_DEV_PATH) == 0) {
		ret = console_open(flags);
	} else if (strcmp(pathname, PLATFORM_EXIT_DEV_PATH) == 0) {
		ret = exit_open(flags);
	} else {
		ret = -ENOENT;
	}

out:
	return ret;
}

asmlinkage long
sys_close(unsigned int fd)
{
	long ret;

	struct file_p *fp = fs_lookup_file(fd);
	if (fp == NULL) {
		ret = -EBADF;
		goto out;
	}

	(void)memset(fp, 0, sizeof(*fp));
	fd_alloc &= ~((register_t)1U << fd);
	ret = 0;

out:
	return ret;
}
