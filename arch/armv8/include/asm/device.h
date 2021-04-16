// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

// The hypervisor cannot easily trap load and store instructions that update
// their base registers. So, for devices that might be virtualised, we must
// use asm directives to generate non-writeback load and store instructions
// for relaxed accesses.
//
// This is not necessary for acquire / release accesses because there are no
// writeback forms of those in AArch64.

// clang-format off
#define device_load_relaxed(p) _Generic( \
	(p), \
	_Atomic int8_t *: device_load8_relaxed, \
	_Atomic uint8_t *: device_load8_relaxed, \
	_Atomic uint16_t *: device_load16_relaxed, \
	_Atomic int16_t *: device_load16_relaxed, \
	_Atomic uint32_t *: device_load32_relaxed, \
	_Atomic int32_t *: device_load32_relaxed, \
	_Atomic uint64_t *: device_load64_relaxed, \
	_Atomic int64_t *: device_load64_relaxed)(p)

#define device_store_relaxed(p, v) _Generic( \
	(p), \
	_Atomic int8_t *: device_store8_relaxed, \
	_Atomic uint8_t *: device_store8_relaxed, \
	_Atomic uint16_t *: device_store16_relaxed, \
	_Atomic int16_t *: device_store16_relaxed, \
	_Atomic uint32_t *: device_store32_relaxed, \
	_Atomic int32_t *: device_store32_relaxed, \
	_Atomic uint64_t *: device_store64_relaxed, \
	_Atomic int64_t *: device_store64_relaxed)(p, v)
// clang-format off

static inline __attribute__((always_inline)) uint8_t
device_load8_relaxed(_Atomic uint8_t *p)
{
	uint8_t ret;
	__asm__ volatile("ldrb %w0, %1" : "=r"(ret) : "m"(*p));
	return ret;
}

static inline __attribute__((always_inline)) uint16_t
device_load16_relaxed(_Atomic uint16_t *p)
{
	uint16_t ret;
	__asm__ volatile("ldrh %w0, %1" : "=r"(ret) : "m"(*p));
	return ret;
}

static inline __attribute__((always_inline)) uint32_t
device_load32_relaxed(_Atomic uint32_t *p)
{
	uint32_t ret;
	__asm__ volatile("ldr %w0, %1" : "=r"(ret) : "m"(*p));
	return ret;
}

static inline __attribute__((always_inline)) uint64_t
device_load64_relaxed(_Atomic uint64_t *p)
{
	uint64_t ret;
	__asm__ volatile("ldr %0, %1" : "=r"(ret) : "m"(*p));
	return ret;
}

static inline __attribute__((always_inline)) void
device_store8_relaxed(_Atomic uint8_t *p, uint8_t v)
{
	__asm__ volatile("strb %w1, %0" : "=m"(*p) : "rz"(v));
}

static inline __attribute__((always_inline)) void
device_store16_relaxed(_Atomic uint16_t *p, uint16_t v)
{
	__asm__ volatile("strh %w1, %0" : "=m"(*p) : "rz"(v));
}

static inline __attribute__((always_inline)) void
device_store32_relaxed(_Atomic uint32_t *p, uint32_t v)
{
	__asm__ volatile("str %w1, %0" : "=m"(*p) : "rz"(v));
}

static inline __attribute__((always_inline)) void
device_store64_relaxed(_Atomic uint64_t *p, uint64_t v)
{
	__asm__ volatile("str %1, %0" : "=m"(*p) : "rz"(v));
}
