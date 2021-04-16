// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

// log level definition
#define INFO	(0)
#define DEBUG	(1)
#define WARNING (2)
#define ERROR	(3)

// action definition, unused yet
#define PANIC (0)
// just print message
#define MSG (1)

#define LOG_FUNC_I(level, action, a0, a1, a2, a3, a4, a5, n, ...)              \
	LOG_ADD##n(level, action, a0, a1, a2, a3, a4, a5, __VA_ARGS__)

#define LOG_FUNC(...)                                                          \
	LOG_FUNC_I(__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0, _unspecified_id)

#define LOG_ADD0(level, action, ...) log_msg(level, action, 0, 0, 0, 0, 0, 0)

#define LOG_ADD1(level, a1, action, ...)                                       \
	log_msg(level, a1, action, 0, 0, 0, 0, 0)

#define LOG_ADD2(level, action, a1, a2, ...)                                   \
	log_msg(level, action, a1, a2, 0, 0, 0, 0)

#define LOG_ADD3(level, action, a1, a2, a3, ...)                               \
	log_msg(level, action, a1, a2, a3, 0, 0, 0)

#define LOG_ADD4(level, action, a1, a2, a3, a4, ...)                           \
	log_msg(level, action, a1, a2, a3, a4, 0, 0)

#define LOG_ADD5(level, action, a1, a2, a3, a4, a5, ...)                       \
	log_msg(level, action, a1, a2, a3, a4, a5, 0)

#define LOG_ADD6(level, action, a1, a2, a3, a4, a5, a6, ...)                   \
	log_msg(level, action, a1, a2, a3, a4, a5, a6)

#define LOG(level, action, ...) LOG_FUNC(level, action, __VA_ARGS__)

void
log_msg(uint64_t level, uint64_t action, const char *a1, size_t a2,
	register_t a3, register_t a4, register_t a5, register_t a6);

extern void
log_append(const char *msg, size_t sz);
