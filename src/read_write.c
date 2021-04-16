// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <types.h>

#include <arch_def.h>
#include <errno.h>
#include <fs.h>
#include <log.h>
#include <syscall_defs.h>
#include <unistd.h>

asmlinkage long
sys_writev(unsigned long fd, uintptr_t vec, unsigned long vlen)
{
	const struct iovec *vecp = (const struct iovec *)vec;
	long		    ret;

	struct file_p *fp = fs_lookup_file(fd);
	if (fp == NULL || (fp->flags & FS_WRITE) == 0) {
		ret = -EBADF;
		goto out;
	}

	struct file *f = fp->file;
	assert(f != NULL);
	if (f->ops->writev == NULL) {
		ret = -EINVAL;
		goto out;
	}

	ret = f->ops->writev(vecp, vlen);

out:
	return ret;
}
