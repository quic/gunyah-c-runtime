// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#define list_append(type, headp, node, prefix)                                 \
	do {                                                                   \
		if (*(headp) == NULL) {                                        \
			(node)->prefix##next = NULL;                           \
			(node)->prefix##prev = (node);                         \
			*(headp)	     = (node);                         \
		} else {                                                       \
			type *tail__p		 = (*(headp))->prefix##prev;   \
			(node)->prefix##next	 = NULL;                       \
			(node)->prefix##prev	 = tail__p;                    \
			tail__p->prefix##next	 = (node);                     \
			(*(headp))->prefix##prev = (node);                     \
		}                                                              \
	} while (0)

#define list_remove(type, headp, node, prefix)                                 \
	do {                                                                   \
		type *n__next = (node)->prefix##next;                          \
		type *n__prev = (node)->prefix##prev;                          \
		assert(*(headp) != NULL);                                      \
		assert(n__prev != NULL);                                       \
		if (n__next != NULL) {                                         \
			n__next->prefix##prev = n__prev;                       \
		}                                                              \
		if (*(headp) == (node)) {                                      \
			*(headp) = (n__prev == (node)) ? NULL : n__next;       \
		} else {                                                       \
			n__prev->prefix##next = n__next;                       \
			if ((*(headp))->prefix##prev == (node)) {              \
				(*(headp))->prefix##prev = n__prev;            \
			}                                                      \
		}                                                              \
	} while (0)

#define loop_list(node, headp, prefix)                                         \
	for ((node) = *(headp); (node) != NULL; (node) = (node)->prefix##next)
