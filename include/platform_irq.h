// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

void
platform_irq_init(void);

void
platform_irq_enable(int irq);

void
platform_irq_disable(int irq);

void
platform_irq_disable_all(void);

bool
platform_irq_set_trigger(int irq, int trigger);

int
platform_irq_acknowledge(void);

void
platform_irq_priority_drop(int irq);

void
platform_irq_deactivate(int irq);

void
platform_irq_assert(int irq);

void
platform_irq_clear(int irq);
