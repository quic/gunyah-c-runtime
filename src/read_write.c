// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <guest_types.h>

#include <arch_def.h>
#include <errno.h>
#include <fs.h>
#include <log.h>
#include <syscall_defs.h>
#include <unistd.h>

ssize_t
sys_writev(uint32_t fd, uintptr_t vec, size_t vlen)
{
	const struct iovec *vecp = (const struct iovec *)vec;
	int64_t		    ret;

	struct file_p *fp = fs_lookup_file(fd);
	if ((fp == NULL) || ((fp->flags & FS_WRITE) == 0UL)) {
		ret = -EBADF;
		goto out;
	}

	struct file_s *f = fp->file;
	assert(f != NULL);
	if (f->ops->writev == NULL) {
		ret = -EINVAL;
		goto out;
	}

	ret = f->ops->writev(vecp, vlen);

out:
	return ret;
}
