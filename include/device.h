// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <asm/device.h>

#if !defined(device_load_relaxed)
#define device_load_relaxed(p, v) atomic_load_relaxed(p, v)
#endif

#if !defined(device_store_relaxed)
#define device_store_relaxed(p, v) atomic_store_relaxed(p, v)
#endif

#if !defined(device_load_acquire)
#define device_load_acquire(p, v) atomic_load_acquire(p, v)
#endif

#if !defined(device_store_release)
#define device_store_release(p, v) atomic_store_release(p, v)
#endif
