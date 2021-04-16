// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

struct iovec;

typedef long (*fs_writev_t)(const struct iovec *vec, unsigned long vlen);
typedef long (*fs_ioctl_t)(unsigned int cmd, unsigned long arg);

struct fs_ops {
	fs_writev_t writev;
	fs_ioctl_t  ioctl;
};

struct file {
	struct fs_ops *ops;
};

struct file_p {
	struct file *file;
	long	     flags;
};

#define FS_READ	 0x1
#define FS_WRITE 0x2

void
fs_init(void);

long
fs_alloc_fd(struct file *f, int o_flags);

struct file_p *
fs_lookup_file(unsigned long fd);
