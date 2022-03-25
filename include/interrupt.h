// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <uapi/interrupt.h>

void
interrupt_init(void);

int
interrupt_register_isr(int irq, isr_t isr, void *data);

int
interrupt_deregister_isr(int irq);

void
interrupt_dispatch(void);

long
interrupt_open(int flags);

bool
interrupt_wait(void);
