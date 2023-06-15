// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/ioctl.h>

#include <guest_types.h>

#include <asm/interrupt.h>

#include <errno.h>
#include <fs.h>
#include <interrupt.h>
#include <platform_irq.h>

// FIXME: should be generated into include/guest_types.h
#define VIRQ_INVALID ~(virq_t)0U

static char irq_ordering;

struct isr_entry {
	isr_t isr;
	void *data;
};

static struct isr_entry isr_table[PLATFORM_NUM_IRQS];

void
interrupt_init(void)
{
	platform_irq_init();
}

static bool
have_isr(virq_t irq)
{
	return isr_table[irq].isr != NULL;
}

int
interrupt_register_isr(virq_t irq, isr_t isr, void *data)
{
	int ret;

	if (irq >= PLATFORM_NUM_IRQS) {
		ret = -EINVAL;
		goto out;
	}

	if (have_isr(irq)) {
		ret = -EBUSY;
		goto out;
	}

	isr_table[irq].isr  = isr;
	isr_table[irq].data = data;
	ret		    = 0;

out:
	return ret;
}

int
interrupt_deregister_isr(virq_t irq)
{
	int ret;

	if (irq >= PLATFORM_NUM_IRQS) {
		ret = -EINVAL;
		goto out;
	}

	if (!have_isr(irq)) {
		ret = -ENOENT;
		goto out;
	}

	isr_table[irq].isr  = NULL;
	isr_table[irq].data = NULL;
	ret		    = 0;

out:
	return ret;
}

void
interrupt_dispatch(void)
{
	while (true) {
		virq_t irq = platform_irq_acknowledge();

		if (irq == VIRQ_INVALID) {
			break;
		}

		struct isr_entry *i = &isr_table[irq];
		if (i->isr == NULL) {
			platform_irq_disable(irq);
			platform_irq_deactivate(irq);
			continue;
		}

		bool handled = i->isr(irq, i->data);
		platform_irq_priority_drop(irq);
		if (!handled) {
			platform_irq_disable(irq);
		}
		platform_irq_deactivate(irq);
	}
}

static long
update_irq(virq_t irq, bool enable)
{
	long ret;

	if (!have_isr(irq)) {
		ret = -ENOENT;
		goto out;
	}

	if (enable) {
		platform_irq_enable(irq);
	} else {
		platform_irq_disable(irq);
	}

	ret = 0;

out:
	return ret;
}

static long
assert_irq(virq_t irq, bool assert)
{
	long ret;

	if (!have_isr(irq)) {
		ret = -ENOENT;
		goto out;
	}

	if (assert) {
		platform_irq_assert(irq);
	} else {
		platform_irq_clear(irq);
	}

	ret = 0;

out:
	return ret;
}

static long
interrupt_ioctl(unsigned int cmd, unsigned long arg)
{
	long ret;

	switch (cmd) {
	case IOCTL_ENABLE_IRQ:
		ret = update_irq(*(virq_t *)arg, true);
		break;
	case IOCTL_DISABLE_IRQ:
		ret = update_irq(*(virq_t *)arg, false);
		break;
	case IOCTL_ASSERT_IRQ:
		ret = assert_irq(*(virq_t *)arg, true);
		break;
	case IOCTL_CLEAR_IRQ:
		ret = assert_irq(*(virq_t *)arg, false);
		break;
	case IOCTL_REGISTER_ISR: {
		struct register_isr_req *data = (struct register_isr_req *)arg;
		ret = interrupt_register_isr(data->irq, data->isr, data->data);
		break;
	}
	case IOCTL_DEREGISTER_ISR: {
		ret = interrupt_deregister_isr(*(virq_t *)arg);
		break;
	}
	case IOCTL_SET_IRQ_TRIGGER: {
		struct irq_set_trigger_req *data =
			(struct irq_set_trigger_req *)arg;
		ret = (platform_irq_set_trigger(data->irq, data->trigger))
			      ? 0
			      : -EINVAL;
		break;
	}
	default:
		ret = -ENOSYS;
		break;
	}

	return ret;
}

static struct fs_ops irq_ops = {
	.ioctl = interrupt_ioctl,
};

static struct file irq_file = {
	.ops = &irq_ops,
};

long
interrupt_open(int flags)
{
	return fs_alloc_fd(&irq_file, flags);
}

bool
interrupt_wait(void)
{
	asm_wait_for_interrupt(&irq_ordering);

	bool pending = asm_interrupt_pending(&irq_ordering);

	if (pending) {
		interrupt_dispatch();
	}

	return pending;
}
