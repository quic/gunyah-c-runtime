// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

// Dummy implementation of cpulocal, supporting only a single VCPU.

#define PLATFORM_MAX_CORES 1U

// Declarators for CPU-local data
#define CPULOCAL_DECLARE(type, name) type cpulocal_##name[PLATFORM_MAX_CORES]
#define CPULOCAL_DECLARE_STATIC(type, name)                                    \
	static type cpulocal_##name[PLATFORM_MAX_CORES]

// Accessors for CPU-local data
#define CPULOCAL(name) cpulocal_##name[cpulocal_get_index()]
#define CPULOCAL_BY_INDEX(name, index)                                         \
	cpulocal_##name[cpulocal_check_index(index)]

static inline bool
cpulocal_index_valid(cpu_index_t index)
{
	return index < PLATFORM_MAX_CORES;
}

static inline cpu_index_t
cpulocal_check_index(cpu_index_t index)
{
	assert(cpulocal_index_valid(index));
	return index;
}

static inline cpu_index_t
cpulocal_get_index(void)
{
	return cpulocal_check_index(0U);
}
