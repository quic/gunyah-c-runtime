// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define IOC(a, b, c, d) (((a) << 30) | ((b) << 8) | (c) | ((d) << 16))
#define IOC_NONE	0U
#define IOC_WRITE	1U
#define IOC_READ	2U

#define IO(a, b)      IOC(IOC_NONE, (a), (b), 0)
#define IOW(a, b, c)  IOC(IOC_WRITE, (a), (b), sizeof(c))
#define IOR(a, b, c)  IOC(IOC_READ, (a), (b), sizeof(c))
#define IOWR(a, b, c) IOC(IOC_READ | IOC_WRITE, (a), (b), sizeof(c))

#define TIOCGWINSZ 0x5413
#define TIOCSETBUF 0x547f // Non-standard IOCTL!!

struct winsize {
	uint16_t ws_row;
	uint16_t ws_col;
	uint16_t ws_xpixel;
	uint16_t ws_ypixel;
};

// Our non-standard buffer control message
struct tty_set_buffer_req {
	uintptr_t buffer;
	size_t	  size;
};
#ifdef HYPVM_WITH_COVERAGE
#define TIOCGETCOV 0x548f // Non-standard IOCTL!!

struct tty_cov_buffer_req {
	uintptr_t buffer;
	uint32_t  size;
};
#endif
