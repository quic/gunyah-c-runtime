// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

void
platform_irq_init(void);

void
platform_irq_enable(virq_t irq);

void
platform_irq_disable(virq_t irq);

void
platform_irq_disable_all(void);

bool
platform_irq_set_trigger(virq_t irq, int32_t trigger);

virq_t
platform_irq_acknowledge(void);

void
platform_irq_priority_drop(virq_t irq);

void
platform_irq_deactivate(virq_t irq);

void
platform_irq_assert(virq_t irq);

void
platform_irq_clear(virq_t irq);
