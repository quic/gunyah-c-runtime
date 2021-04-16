// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define _IOC(a, b, c, d) (((a) << 30) | ((b) << 8) | (c) | ((d) << 16))
#define _IOC_NONE	 0U
#define _IOC_WRITE	 1U
#define _IOC_READ	 2U

#define _IO(a, b)      _IOC(_IOC_NONE, (a), (b), 0)
#define _IOW(a, b, c)  _IOC(_IOC_WRITE, (a), (b), sizeof(c))
#define _IOR(a, b, c)  _IOC(_IOC_READ, (a), (b), sizeof(c))
#define _IOWR(a, b, c) _IOC(_IOC_READ | _IOC_WRITE, (a), (b), sizeof(c))

#define TIOCGWINSZ 0x5413

struct winsize {
	uint16_t ws_row;
	uint16_t ws_col;
	uint16_t ws_xpixel;
	uint16_t ws_ypixel;
};
