// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

struct iovec;

typedef ssize_t (*fs_writev_t)(const struct iovec *vec, size_t vlen);
typedef int32_t (*fs_ioctl_t)(uint32_t cmd, uintptr_t arg);

struct fs_ops {
	fs_writev_t writev;
	fs_ioctl_t  ioctl;
};

struct file_s {
	struct fs_ops *ops;
};

struct file_p {
	struct file_s *file;
	uint64_t       flags;
};

#define FS_READ	 0x1U
#define FS_WRITE 0x2U

void
fs_init(void);

int32_t
fs_alloc_fd(struct file_s *f, uint32_t o_flags);

struct file_p *
fs_lookup_file(uint32_t fd);
