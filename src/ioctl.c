// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <arch_def.h>
#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <log.h>
#include <syscall_defs.h>

asmlinkage long
sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg)
{
	long ret;

	struct file_p *fp = fs_lookup_file(fd);
	if (fp == NULL) {
		ret = -EBADF;
		goto out;
	}

	struct file *f = fp->file;
	assert(f != NULL);
	if (f->ops->ioctl == NULL) {
		ret = -EINVAL;
		goto out;
	}

	ret = f->ops->ioctl(cmd, arg);

out:
	return ret;
}
