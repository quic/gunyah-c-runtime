// Automatically generated. Do not modify.
//
// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

// Hypervisor Cap Rights

#define CAP_RIGHTS_GENERIC_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_GENERIC_ALL		   (cap_rights_t)0x80000000U

#define CAP_RIGHTS_ADDRSPACE_ATTACH	     (cap_rights_t)0x1U
#define CAP_RIGHTS_ADDRSPACE_MAP	     (cap_rights_t)0x2U
#define CAP_RIGHTS_ADDRSPACE_LOOKUP	     (cap_rights_t)0x4U
#define CAP_RIGHTS_ADDRSPACE_ADD_VMMIO_RANGE (cap_rights_t)0x8U
#define CAP_RIGHTS_ADDRSPACE_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_ADDRSPACE_ALL	     (cap_rights_t)0x8000000fU

#define CAP_RIGHTS_CSPACE_CAP_CREATE	  (cap_rights_t)0x1U
#define CAP_RIGHTS_CSPACE_CAP_DELETE	  (cap_rights_t)0x2U
#define CAP_RIGHTS_CSPACE_CAP_COPY	  (cap_rights_t)0x4U
#define CAP_RIGHTS_CSPACE_ATTACH	  (cap_rights_t)0x8U
#define CAP_RIGHTS_CSPACE_CAP_REVOKE	  (cap_rights_t)0x10U
#define CAP_RIGHTS_CSPACE_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_CSPACE_ALL		  (cap_rights_t)0x8000001fU

#define CAP_RIGHTS_DOORBELL_SEND	    (cap_rights_t)0x1U
#define CAP_RIGHTS_DOORBELL_RECEIVE	    (cap_rights_t)0x2U
#define CAP_RIGHTS_DOORBELL_BIND	    (cap_rights_t)0x4U
#define CAP_RIGHTS_DOORBELL_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_DOORBELL_ALL		    (cap_rights_t)0x80000007U

#define CAP_RIGHTS_HWIRQ_BIND_VIC	 (cap_rights_t)0x2U
#define CAP_RIGHTS_HWIRQ_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_HWIRQ_ALL		 (cap_rights_t)0x80000002U

#define CAP_RIGHTS_MEMEXTENT_MAP	     (cap_rights_t)0x1U
#define CAP_RIGHTS_MEMEXTENT_DERIVE	     (cap_rights_t)0x2U
#define CAP_RIGHTS_MEMEXTENT_ATTACH	     (cap_rights_t)0x4U
#define CAP_RIGHTS_MEMEXTENT_LOOKUP	     (cap_rights_t)0x8U
#define CAP_RIGHTS_MEMEXTENT_DONATE	     (cap_rights_t)0x10U
#define CAP_RIGHTS_MEMEXTENT_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_MEMEXTENT_ALL	     (cap_rights_t)0x8000001fU

#define CAP_RIGHTS_MSGQUEUE_SEND	    (cap_rights_t)0x1U
#define CAP_RIGHTS_MSGQUEUE_RECEIVE	    (cap_rights_t)0x2U
#define CAP_RIGHTS_MSGQUEUE_BIND_SEND	    (cap_rights_t)0x4U
#define CAP_RIGHTS_MSGQUEUE_BIND_RECEIVE    (cap_rights_t)0x8U
#define CAP_RIGHTS_MSGQUEUE_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_MSGQUEUE_ALL		    (cap_rights_t)0x8000000fU

#define CAP_RIGHTS_PARTITION_OBJECT_CREATE   (cap_rights_t)0x1U
#define CAP_RIGHTS_PARTITION_DONATE	     (cap_rights_t)0x2U
#define CAP_RIGHTS_PARTITION_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_PARTITION_ALL	     (cap_rights_t)0x80000003U

#define CAP_RIGHTS_THREAD_POWER		  (cap_rights_t)0x1U
#define CAP_RIGHTS_THREAD_AFFINITY	  (cap_rights_t)0x2U
#define CAP_RIGHTS_THREAD_PRIORITY	  (cap_rights_t)0x4U
#define CAP_RIGHTS_THREAD_TIMESLICE	  (cap_rights_t)0x8U
#define CAP_RIGHTS_THREAD_YIELD_TO	  (cap_rights_t)0x10U
#define CAP_RIGHTS_THREAD_BIND_VIRQ	  (cap_rights_t)0x20U
#define CAP_RIGHTS_THREAD_STATE		  (cap_rights_t)0x40U
#define CAP_RIGHTS_THREAD_LIFECYCLE	  (cap_rights_t)0x80U
#define CAP_RIGHTS_THREAD_WRITE_CONTEXT	  (cap_rights_t)0x100U
#define CAP_RIGHTS_THREAD_DISABLE	  (cap_rights_t)0x200U
#define CAP_RIGHTS_THREAD_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_THREAD_ALL		  (cap_rights_t)0x800003ffU

#define CAP_RIGHTS_VIC_BIND_SOURCE     (cap_rights_t)0x1U
#define CAP_RIGHTS_VIC_ATTACH_VCPU     (cap_rights_t)0x2U
#define CAP_RIGHTS_VIC_ATTACH_VDEVICE  (cap_rights_t)0x4U
#define CAP_RIGHTS_VIC_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_VIC_ALL	       (cap_rights_t)0x80000007U

#define CAP_RIGHTS_VIRTIO_MMIO_BIND_BACKEND_VIRQ  (cap_rights_t)0x1U
#define CAP_RIGHTS_VIRTIO_MMIO_BIND_FRONTEND_VIRQ (cap_rights_t)0x2U
#define CAP_RIGHTS_VIRTIO_MMIO_ASSERT_VIRQ	  (cap_rights_t)0x4U
#define CAP_RIGHTS_VIRTIO_MMIO_CONFIG		  (cap_rights_t)0x8U
#define CAP_RIGHTS_VIRTIO_MMIO_OBJECT_ACTIVATE	  (cap_rights_t)0x80000000U
#define CAP_RIGHTS_VIRTIO_MMIO_ALL		  (cap_rights_t)0x8000000fU

#define CAP_RIGHTS_VPM_GROUP_ATTACH_VCPU     (cap_rights_t)0x1U
#define CAP_RIGHTS_VPM_GROUP_BIND_VIRQ	     (cap_rights_t)0x2U
#define CAP_RIGHTS_VPM_GROUP_QUERY	     (cap_rights_t)0x4U
#define CAP_RIGHTS_VPM_GROUP_OBJECT_ACTIVATE (cap_rights_t)0x80000000U
#define CAP_RIGHTS_VPM_GROUP_ALL	     (cap_rights_t)0x80000007U

#define CAP_RIGHTS_VRTC_CONFIGURE	 (cap_rights_t)0x1U
#define CAP_RIGHTS_VRTC_ATTACH_ADDRSPACE (cap_rights_t)0x2U
#define CAP_RIGHTS_VRTC_SET_TIME_BASE	 (cap_rights_t)0x4U
#define CAP_RIGHTS_VRTC_OBJECT_ACTIVATE	 (cap_rights_t)0x80000000U
#define CAP_RIGHTS_VRTC_ALL		 (cap_rights_t)0x80000007U
