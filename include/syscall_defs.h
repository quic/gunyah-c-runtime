// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

// FIXME: need to share certain definitions between musl and run time, should
// we just copy from musl or there's another way.

// FIXME: refine NR_syscall_max to maximum number (+1) of syscall

#define NR_syscall_max 434

#if !defined(__ASSEMBLER__)
#define asmlinkage

struct timespec;

asmlinkage long
sys_set_tid_address(int *tid_ptr);

// Need to share
asmlinkage long
sys_ppoll(void *ufds, unsigned int nfds, const struct timespec *time,
	  void *sigmask, uint32_t sigsetsize);

asmlinkage long
sys_openat(int dirfd, const char *pathname, int flags, umode_t mode);

asmlinkage long
sys_clock_nanosleep(long clock_id, int flags, const struct timespec *request,
		    struct timespec *remain);

asmlinkage int
sys_tkill(int tid, int sig);

asmlinkage void
sys_exit_group(int ec);

asmlinkage noreturn void
sys_exit(int ec);

asmlinkage long
sys_brk(unsigned long brk);

asmlinkage long
sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);

asmlinkage long
sys_writev(unsigned long fd, uintptr_t vec, unsigned long vlen);

asmlinkage long
sys_close(unsigned int fd);
#endif
