// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <types.h>

#include <guest_types.h>

#include <atomic.h>
#include <cpulocal.h>
#include <device.h>
#include <interrupt.h>
#include <platform_irq.h>
#include <util.h>

#define GICD_SIZE 0x10000U
#define GICR_SIZE 0x40000U

// Set to 0 to use GICD_I[SC]PENDR writes; 1 to use SETSPI / CLRSPI
#define PLATFORM_IRQ_USE_MSI 0

// FIXME: should be generated into include/guest_types.h
#define VIRQ_INVALID ~(virq_t)0U

extern rt_env_data_t *env_data;

struct gicd_s {
	uint32_t _Atomic ctlr;
	uint32_t _Atomic typer;
	uint32_t _Atomic iidr;
	uint8_t		 pad_to_statusr_[4];
	uint32_t _Atomic statusr;
	uint8_t		 pad_to_setspi_nsr_[44];
	uint32_t _Atomic setspi_nsr;
	uint8_t		 pad_to_clrspi_nsr_[4];
	uint32_t _Atomic clrspi_nsr;
	// ignore {set,clr}spi_sr which are WI when DS=1
	uint8_t		 pad_to_igroupr_[52];
	uint32_t _Atomic igroupr[32];
	uint32_t _Atomic isenabler[32];
	uint32_t _Atomic icenabler[32];
	uint32_t _Atomic ispendr[32];
	uint32_t _Atomic icpendr[32];
	uint32_t _Atomic isactiver[32];
	uint32_t _Atomic icactiver[32];
	uint8_t _Atomic	 ipriorityr[1020];
	// Ignore itargetsr
	uint8_t		 pad_to_icfgr_[1028];
	uint32_t _Atomic icfgr[64];
	uint32_t _Atomic igrpmodr[32];
	// Ignore nascr, sgir,
	// {c,s}pendsgir, extended regs
	uint8_t		 pad_to_irouter_[21376];
	uint64_t _Atomic irouter[988];
	// Ignore extended irouter
	uint8_t pad_to_end_[32800];
};

struct gicr_s {
	// RD_base
	uint32_t _Atomic ctlr;
	uint32_t _Atomic iidr;
	uint64_t _Atomic typer;
	uint32_t _Atomic statusr;
	uint32_t _Atomic waker;
	// Ignore LPI regs
	uint8_t pad_to_sgi_base_[65512];
	// SGI_base
	uint8_t pad_to_igroupr0_[128];
	// Extended regs are ignored
	uint32_t _Atomic igroupr0;
	uint8_t		 pad_to_isenabler0_[124];
	uint32_t _Atomic isenabler0;
	uint8_t		 pad_to_icenabler0_[124];
	uint32_t _Atomic icenabler0;
	uint8_t		 pad_to_ispendr0_[124];
	uint32_t _Atomic ispendr0;
	uint8_t		 pad_to_icpendr0_[124];
	uint32_t _Atomic icpendr0;
	uint8_t		 pad_to_isactiver0_[124];
	uint32_t _Atomic isactiver0;
	uint8_t		 pad_to_icactiver0_[124];
	uint32_t _Atomic icactiver0;
	uint8_t		 pad_to_ipriorityr_[124];
	uint8_t _Atomic	 ipriorityr[32];
	uint8_t		 pad_to_icfgr0_[2016];
	uint32_t _Atomic icfgr0;
	uint32_t _Atomic icfgr1;
	uint8_t		 pad_to_igrpmodr0_[248];
	uint32_t _Atomic igrpmodr0;
	// Ignore nsacr
	uint8_t pad_to_vlpi_base_[62204];
	// VLPI_base ignored
	uint8_t pad_to_end_[131072];
};

static_assert(sizeof(struct gicd_s) == GICD_SIZE, "gicd not sized correctly");
static_assert(sizeof(struct gicr_s) == GICR_SIZE, "gicr not sized correctly");

#define GIC_SGI_BASE 0U
#define GIC_SGI_NUM  16U
#define GIC_SGI_END  (GIC_SGI_BASE + GIC_SGI_NUM)
#define GIC_PPI_BASE 16U
#define GIC_PPI_NUM  16U
#define GIC_PPI_END  (GIC_PPI_BASE + GIC_PPI_NUM)
#define GIC_SPI_BASE 32U
#define GIC_SPI_NUM  988U
#define GIC_SPI_END  (GIC_SPI_BASE + GIC_SPI_NUM)

static_assert(GIC_SPI_END >= PLATFORM_NUM_IRQS, "Bad num irqs");

#define GIC_PRIORITY_DEFAULT 0xA0U

#define GICD_CTLR_RWP	     (1U << 31)
#define GICD_CTLR_ARE	     (1U << 4)
#define GICD_CTLR_ENABLEGRP1 (1U << 1)

#define GICR_CTLR_RWP (1U << 3)

#define ICC_CTLR_EOIMODE (1U << 1)

#define MPIDR_EL1_AFF0123_MASK (0xff00ffffffU)

#define GICD_ENABLE_GET_N(x) ((x) >> 5)
#define GIC_ENABLE_BIT(x)    ((uint32_t)1U << (index_t)(x))

static char gic_ordering;

static struct gicd_s *gicd;
CPULOCAL_DECLARE_STATIC(struct gicr_s *, gicr);

static uint32_t
gicd_wait_for_write(void)
{
	atomic_thread_fence(memory_order_acquire);
	uint32_t ctlr = device_load_relaxed(&gicd->ctlr);

	while ((ctlr & GICD_CTLR_RWP) != 0U) {
		ctlr = device_load_relaxed(&gicd->ctlr);
	}

	return ctlr;
}

static void
gicr_wait_for_write(struct gicr_s *gicr)
{
	uint32_t ctlr = device_load_relaxed(&gicr->ctlr);

	while ((ctlr & GICR_CTLR_RWP) != 0U) {
		ctlr = device_load_relaxed(&gicr->ctlr);
	}
}

#define ASM_MRS_MPIDR_EL1(a) __asm__("mrs %0, MPIDR_EL1" : "=r"((a)))
#define ASM_MSR_ICC_SRE_EL1(a)                                                 \
	__asm__ volatile("msr ICC_SRE_EL1, %0" ::"r"((a)))
#define ASM_MSR_ICC_PMR_EL1(a)                                                 \
	__asm__ volatile("msr ICC_PMR_EL1, %0" ::"r"((a)))
#define ASM_MSR_ICC_BPR1_EL1(a)                                                \
	__asm__ volatile("msr ICC_BPR1_EL1, %0" ::"r"((a)))
#define ASM_MRS_ICC_CTLR_EL1(a) __asm__("mrs %0, ICC_CTLR_EL1" : "=r"((a)))
#define ASM_MSR_ICC_CTLR_EL1(a) __asm__("msr %0, ICC_CTLR_EL1" : "=r"((a)))
#define ASM_MSR_ICC_IGRPEN1_EL1(a)                                             \
	__asm__ volatile("msr ICC_IGRPEN1_EL1, %0" ::"r"((a)))

void
platform_irq_init(void)
{
	// FIXME:
	count_t num_irqs = PLATFORM_NUM_IRQS;

	// FIXME: map if using page table & not already mapped
	gicd = (struct gicd_s *)env_data->gicd_base;

	// FIXME: do proper typer lookup
	for (cpu_index_t i = 0U; cpulocal_index_valid(i); i++) {
		CPULOCAL_BY_INDEX(gicr, i) =
			((struct gicr_s *)env_data->gicr_base) + i;
	}

	// Disable distributor
	uint32_t ctlr = 0U;
	device_store_relaxed(&gicd->ctlr, ctlr);
	ctlr = gicd_wait_for_write();

	// Enable affinity routing
	ctlr |= GICD_CTLR_ARE;
	device_store_relaxed(&gicd->ctlr, ctlr);
	ctlr = gicd_wait_for_write();

	// Disable all SPIs
	for (index_t i = 1U; i < (num_irqs / 32U); i++) {
		device_store_relaxed(&gicd->icenabler[i], 0xffffffffU);
	}
	ctlr = gicd_wait_for_write();

	// Set SPI default priority
	for (index_t i = GIC_SPI_BASE; i < num_irqs; i++) {
		device_store_relaxed(&gicd->ipriorityr[i],
				     GIC_PRIORITY_DEFAULT);
	}

	// Set SPI default trigger as edge
	for (index_t i = 2U; i < (num_irqs / 16U); i++) {
		device_store_relaxed(&gicd->icfgr[i], 0xaaaaaaaaU);
	}

	// Set SPIs as group 1
	for (index_t i = 1U; i < (num_irqs / 32U); i++) {
		device_store_relaxed(&gicd->igroupr[i], 0xffffffffU);
	}

	// Set SPI default route
	uint64_t mpidr;
	// System Register MPIDR_EL1 must be accessed via MRS/MSR
	ASM_MRS_MPIDR_EL1(mpidr);
	mpidr &= MPIDR_EL1_AFF0123_MASK;

	for (index_t i = 0U; i < (num_irqs - (index_t)GIC_SPI_BASE); i++) {
		device_store_relaxed(&gicd->irouter[i], mpidr);
	}

	// Enable Group 1 IRQs
	ctlr |= GICD_CTLR_ENABLEGRP1;
	device_store_relaxed(&gicd->ctlr, ctlr);

	// FIXME: per vcpu cold init
	struct gicr_s *gicr = CPULOCAL(gicr);

	// Disable SGIs/PPIs
	device_store_relaxed(&gicr->icenabler0, 0xffffffffU);
	gicr_wait_for_write(gicr);

	// Set SGI/PPI default priority
	for (index_t i = 0U; i < util_array_size(gicr->ipriorityr); i++) {
		device_store_relaxed(&gicr->ipriorityr[i],
				     GIC_PRIORITY_DEFAULT);
	}

	// Set PPI default trigger as level
	device_store_relaxed(&gicr->icfgr1, 0U);

	// Set SGI/PPI group 1
	device_store_relaxed(&gicr->igroupr0, 0xffffffffU);

	// FIXME: warm init

	// Disable IRQ/FIQ bypass, sysreg enable
	uint64_t icc_sre = 0x7U;
	// System Register ICC_SRE_EL1 must be accessed via MRS/MSR
	ASM_MSR_ICC_SRE_EL1(icc_sre);

	// Allow all priorities
	uint64_t icc_pmr = 0xffU;
	// System Register ICC_PMR_EL1 must be accessed via MRS/MSR
	ASM_MSR_ICC_PMR_EL1(icc_pmr);

	// Prevent preemption during IRQ handling
	uint64_t icc_bpr = 0x7U;
	// System Register ICC_BPR1_EL1 must be accessed via MRS/MSR
	ASM_MSR_ICC_BPR1_EL1(icc_bpr);

	// Clear EOImode
	uint64_t icc_ctlr;
	// System Register ICC_CTLR_EL1 must be accessed via MRS/MSR
	ASM_MRS_ICC_CTLR_EL1(icc_ctlr);
	icc_ctlr &= ~(uint64_t)ICC_CTLR_EOIMODE;
	// System Register ICC_CTLR_EL1 must be accessed via MRS/MSR
	ASM_MSR_ICC_CTLR_EL1(icc_ctlr);

	// Enable group 1 IRQs
	uint64_t icc_igrpen1 = 0x1U;
	// System Register ICC_IGRPEN1_EL1 must be accessed via MRS/MSR
	ASM_MSR_ICC_IGRPEN1_EL1(icc_igrpen1);
}

void
platform_irq_enable(virq_t irq)
{
	if ((irq >= GIC_SGI_BASE) && (irq < GIC_PPI_END)) {
		struct gicr_s *gicr = CPULOCAL(gicr);
		device_store_release(&gicr->isenabler0, GIC_ENABLE_BIT(irq));
	} else if ((irq >= GIC_SPI_BASE) && (irq < GIC_SPI_END)) {
		device_store_release(&gicd->isenabler[GICD_ENABLE_GET_N(irq)],
				     GIC_ENABLE_BIT(irq));
	} else {
		// Ignore others
	}
}

void
platform_irq_disable(virq_t irq)
{
	if ((irq >= GIC_SGI_BASE) && (irq < GIC_PPI_END)) {
		struct gicr_s *gicr = CPULOCAL(gicr);
		device_store_relaxed(&gicr->icenabler0, GIC_ENABLE_BIT(irq));
		(void)gicr_wait_for_write(gicr);
	} else if ((irq >= GIC_SPI_BASE) && (irq < GIC_SPI_END)) {
		device_store_relaxed(&gicd->icenabler[GICD_ENABLE_GET_N(irq)],
				     GIC_ENABLE_BIT(irq));
		(void)gicd_wait_for_write();
	} else {
		// Ignore others
	}
}

void
platform_irq_assert(virq_t irq)
{
	if ((irq >= GIC_SGI_BASE) && (irq < GIC_PPI_END)) {
		struct gicr_s *gicr = CPULOCAL(gicr);
		device_store_release(&gicr->ispendr0, GIC_ENABLE_BIT(irq));
	} else if ((irq >= GIC_SPI_BASE) && (irq < GIC_SPI_END)) {
#if PLATFORM_IRQ_USE_MSI
		device_store_relaxed(&gicd->setspi_nsr, irq);
#else
		device_store_relaxed(&gicd->ispendr[GICD_ENABLE_GET_N(irq)],
				     GIC_ENABLE_BIT(irq));
#endif
	} else {
		// Ignore others
	}
}

void
platform_irq_clear(virq_t irq)
{
	if ((irq >= GIC_SGI_BASE) && (irq < GIC_PPI_END)) {
		struct gicr_s *gicr = CPULOCAL(gicr);
		device_store_relaxed(&gicr->icpendr0, GIC_ENABLE_BIT(irq));
	} else if ((irq >= GIC_SPI_BASE) && (irq < GIC_SPI_END)) {
#if PLATFORM_IRQ_USE_MSI
		device_store_relaxed(&gicd->clrspi_nsr, irq);
#else
		device_store_relaxed(&gicd->icpendr[GICD_ENABLE_GET_N(irq)],
				     GIC_ENABLE_BIT(irq));
#endif
	} else {
		// Ignore others
	}
}

void
platform_irq_disable_all(void)
{
	// Disable Group 1 IRQs
	uint64_t icc_igrpen1 = 0U;
	__asm__ volatile("msr ICC_IGRPEN1_EL1, %0" ::"r"(icc_igrpen1));
}

bool
platform_irq_set_trigger(virq_t irq, int trigger)
{
	bool edge, valid;

	switch (trigger) {
	case IRQ_TRIGGER_LEVEL_HIGH:
		edge  = false;
		valid = true;
		break;
	case IRQ_TRIGGER_EDGE_RISING:
		edge  = true;
		valid = true;
		break;
	case IRQ_TRIGGER_LEVEL_LOW:
	case IRQ_TRIGGER_EDGE_FALLING:
	case IRQ_TRIGGER_EDGE_BOTH:
	case IRQ_TRIGGER_MESSAGE:
	default:
		edge  = false;
		valid = false;
		break;
	}

	if (!valid) {
		goto out;
	}

	if ((irq >= GIC_SGI_BASE) && (irq <= GIC_SGI_END)) {
		// SGIs always edge triggered
		valid = edge;
	} else if ((irq >= GIC_PPI_BASE) && (irq < GIC_PPI_END)) {
		struct gicr_s *gicr = CPULOCAL(gicr);
		uint32_t isenabler  = device_load_relaxed(&gicr->isenabler0);
		bool	 enabled    = (isenabler & GIC_ENABLE_BIT(irq)) != 0U;
		if (enabled) {
			platform_irq_disable(irq);
		}

		uint32_t icfgr = device_load_relaxed(&gicr->icfgr1);
		uint32_t index = (((uint32_t)irq % 16U) * 2U) + 1U;
		if (edge) {
			icfgr |= (uint32_t)1U << index;
		} else {
			icfgr &= ~(1U << index);
		}
		device_store_relaxed(&gicr->icfgr1, icfgr);

		if (enabled) {
			platform_irq_enable(irq);
		}
	} else if ((irq >= GIC_SPI_BASE) && (irq < GIC_SPI_END)) {
		uint32_t isenabler = device_load_relaxed(
			&gicd->isenabler[GICD_ENABLE_GET_N(irq)]);
		bool enabled = (isenabler & GIC_ENABLE_BIT(irq)) != 0U;

		if (enabled) {
			platform_irq_disable(irq);
		}

		uint32_t icfgr = device_load_relaxed(&gicd->icfgr[irq / 16]);
		uint32_t index = (((uint32_t)irq % 16U) * 2U) + 1U;
		if (edge) {
			icfgr |= (uint32_t)1U << index;
		} else {
			icfgr &= ~(1U << index);
		}
		device_store_relaxed(&gicd->icfgr[irq / 16], icfgr);

		if (enabled) {
			platform_irq_enable(irq);
		}
	} else {
		valid = false;
	}

out:
	return valid;
}

virq_t
platform_irq_acknowledge(void)
{
	uint64_t icc_iar1;

	__asm__ volatile("mrs %[r], ICC_IAR1_EL1"
			 : [r] "=r"(icc_iar1), "+m"(gic_ordering));

	// Ensure IRQ is not handled before it is acknowledged in the GICD
	// (as that could cause loss of edge-triggered IRQs, especially SGIs)
	__asm__ volatile("isb; dsb sy" ::: "memory");

	return (icc_iar1 < (uint64_t)GIC_SPI_END) ? (virq_t)icc_iar1
						  : VIRQ_INVALID;
}

void
platform_irq_priority_drop(virq_t irq)
{
	(void)irq;
}

void
platform_irq_deactivate(virq_t irq)
{
	assert((irq >= 0) && (irq < GIC_SPI_END));

	// An ISR may have unmasked IRQs, ensure they are masked
	__asm__ volatile("msr DAIFSet, 0x7" : "+m"(gic_ordering));

	// Ensure irq handling is complete
	__asm__ volatile("dsb sy; isb" ::: "memory");

	uint64_t icc_eoir1 = (uint64_t)irq;
	__asm__ volatile("msr ICC_EOIR1_EL1, %[r]"
			 : "+m"(gic_ordering)
			 : [r] "r"(icc_eoir1));
}
