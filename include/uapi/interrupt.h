// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#define IRQ_MAGIC 0x49U

#define IOCTL_ENABLE_IRQ      _IOW(IRQ_MAGIC, 0U, int)
#define IOCTL_DISABLE_IRQ     _IOW(IRQ_MAGIC, 1U, int)
#define IOCTL_REGISTER_ISR    _IOW(IRQ_MAGIC, 2U, struct register_isr_req)
#define IOCTL_SET_IRQ_TRIGGER _IOW(IRQ_MAGIC, 3U, struct irq_set_trigger_req)
#define IOCTL_DEREGISTER_ISR  _IOW(IRQ_MAGIC, 4U, int)
#define IOCTL_ASSERT_IRQ      _IOW(IRQ_MAGIC, 5U, int)
#define IOCTL_CLEAR_IRQ	      _IOW(IRQ_MAGIC, 6U, int)

typedef bool (*isr_t)(virq_t, void *);

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
