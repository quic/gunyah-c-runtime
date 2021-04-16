// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>

int
strcmp(const char *s1, const char *s2)
{
	const char *l = s1, *r = s2;

	while (*l && (*l == *r)) {
		l++;
		r++;
	}

	return *l - *r;
}
