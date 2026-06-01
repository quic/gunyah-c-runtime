// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

struct timespec;

int32_t
sys_set_tid_address(int32_t *tid_ptr);

int32_t
sys_ppoll(uintptr_t ufds, uint32_t nfds, const struct timespec *timeout,
	  uintptr_t sigmask, size_t sigsetsize);

int32_t
sys_openat(int32_t dirfd, const char *pathname, uint32_t flags, uint32_t mode);

int32_t
sys_clock_nanosleep(int32_t clock_id, int32_t flags,
		    const struct timespec *request, struct timespec *remain);

int32_t
sys_tkill(int32_t tid, int32_t sig);

noreturn void
sys_exit_group(int32_t ec);

noreturn void
sys_exit(int32_t ec);

uintptr_t
sys_brk(uintptr_t brk);

int32_t
sys_ioctl(uint32_t fd, uint32_t cmd, uintptr_t arg);

ssize_t
sys_writev(uint32_t fd, uintptr_t vec, size_t vlen);

int32_t
sys_close(uint32_t fd);

intptr_t
sys_mmap(uintptr_t addr, size_t len, uint32_t prot, uint32_t flags, int32_t fd,
	 size_t off);

int32_t
sys_munmap(uintptr_t addr, size_t len);
