// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <guest_types.h>

#include <arch_def.h>
#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <log.h>
#include <syscall_defs.h>

int32_t
sys_ioctl(uint32_t fd, uint32_t cmd, uintptr_t arg)
{
	int32_t ret;

	struct file_p *fp = fs_lookup_file(fd);
	if (fp == NULL) {
		ret = -EBADF;
		goto out;
	}

	struct file_s *f = fp->file;
	assert(f != NULL);
	if (f->ops->ioctl == NULL) {
		ret = -EINVAL;
		goto out;
	}

	ret = f->ops->ioctl(cmd, arg);

out:
	return ret;
}
