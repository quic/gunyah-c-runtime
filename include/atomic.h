// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdatomic.h>

#define atomic_load_relaxed(p) atomic_load_explicit((p), memory_order_relaxed)
#define atomic_load_acquire(p) atomic_load_explicit((p), memory_order_acquire)
#define atomic_store_relaxed(p, v)                                             \
	atomic_store_explicit((p), (v), memory_order_relaxed)
#define atomic_store_release(p, v)                                             \
	atomic_store_explicit((p), (v), memory_order_release)
