// Copyright © Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// SPDX-License-Identifier: BSD-3-Clause

#define IRQ_MAGIC 0x49U

#define IOCTL_ENABLE_IRQ      IOW(IRQ_MAGIC, 0U, int32_t)
#define IOCTL_DISABLE_IRQ     IOW(IRQ_MAGIC, 1U, int32_t)
#define IOCTL_REGISTER_ISR    IOW(IRQ_MAGIC, 2U, struct register_isr_req)
#define IOCTL_SET_IRQ_TRIGGER IOW(IRQ_MAGIC, 3U, struct irq_set_trigger_req)
#define IOCTL_DEREGISTER_ISR  IOW(IRQ_MAGIC, 4U, int32_t)
#define IOCTL_ASSERT_IRQ      IOW(IRQ_MAGIC, 5U, int32_t)
#define IOCTL_CLEAR_IRQ	      IOW(IRQ_MAGIC, 6U, int32_t)

typedef bool (*isr_t)(virq_t irq, void *data);

struct register_isr_req {
	isr_t	isr;
	virq_t	irq;
	int32_t res0;
	void   *data;
};

#define IRQ_TRIGGER_LEVEL_HIGH	 0
#define IRQ_TRIGGER_LEVEL_LOW	 1
#define IRQ_TRIGGER_EDGE_RISING	 2
#define IRQ_TRIGGER_EDGE_FALLING 3
#define IRQ_TRIGGER_EDGE_BOTH	 4
#define IRQ_TRIGGER_MESSAGE	 5

struct irq_set_trigger_req {
	virq_t	irq;
	int32_t trigger;
};

void
interrupt_init(void);

int32_t
interrupt_register_isr(virq_t irq, isr_t isr, void *data);

int32_t
interrupt_deregister_isr(virq_t irq);

void
interrupt_dispatch(void);

int32_t
interrupt_open(uint32_t flags);

bool
interrupt_wait(void);
