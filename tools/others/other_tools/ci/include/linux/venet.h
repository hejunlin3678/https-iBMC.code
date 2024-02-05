/*
 * Copyright 2009 Novell.  All Rights Reserved.
 *
 * Virtual-Ethernet adapter
 *
 * Author:
 *      Gregory Haskins <ghaskins@novell.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _LINUX_VENET_H
#define _LINUX_VENET_H

#include <linux/types.h>

#define VENET_VERSION 1

#define VENET_TYPE "virtual-ethernet"

#define VENET_QUEUE_RX 0
#define VENET_QUEUE_TX 1

struct venet_capabilities {
	__u32 gid;
	__u32 bits;
};

/* DEPRECATED: 0, 2 */
#define VENET_CAP_GROUP_EVENTQ 1
#define VENET_CAP_GROUP_SG     3
#define VENET_CAP_GROUP_L4RO   4 /* layer-4 reassem offloading */

/* CAPABILITIES-GROUP SG */
#define VENET_CAP_SG     (1 << 0)
#define VENET_CAP_TSO4   (1 << 1)
#define VENET_CAP_TSO6   (1 << 2)
#define VENET_CAP_ECN    (1 << 3)
#define VENET_CAP_UFO    (1 << 4)
#define VENET_CAP_PMTD   (1 << 5) /* pre-mapped tx desc */

/* CAPABILITIES-GROUP EVENTQ */
#define VENET_CAP_EVQ_LINKSTATE  (1 << 0)
#define VENET_CAP_EVQ_TXC        (1 << 1) /* tx-complete */

struct venet_iov {
	__u32       len;
	aligned_u64 ptr;
};

#define VENET_SG_FLAG_NEEDS_CSUM (1 << 0)
#define VENET_SG_FLAG_GSO        (1 << 1)
#define VENET_SG_FLAG_ECN        (1 << 2)

struct venet_sg {
	aligned_u64      cookie;
	__u32            flags;
	__u32            len;     /* total length of all iovs */
	struct {
		__u16    start;	  /* csum starting position */
		__u16    offset;  /* offset to place csum */
	} csum;
	struct {
#define VENET_GSO_TYPE_TCPV4	0	/* IPv4 TCP (TSO) */
#define VENET_GSO_TYPE_UDP	1	/* IPv4 UDP (UFO) */
#define VENET_GSO_TYPE_TCPV6	2	/* IPv6 TCP */
		__u8     type;
		__u16    hdrlen;
		__u16    size;
	} gso;
	struct {
		__u32    mac;         /* mac offset */
		__u32    network;     /* network offset */
		__u32    transport;   /* transport offset */
	} phdr;
	__u32            pad[32]; /* pad area for future use */
	__u32            count;   /* nr of iovs */
	struct venet_iov iov[1];
};

struct venet_eventq_query {
	__u32 flags;
	__u32 evsize;  /* size of each event */
	__u32 dpid;    /* descriptor pool-id */
	__u32 qid;
	__u8  pad[16];
};

#define VENET_EVENT_LINKSTATE 0
#define VENET_EVENT_TXC       1

struct venet_event_header {
	__u32 flags;
	__u32 size;
	__u32 id;
};

struct venet_event_linkstate {
	struct venet_event_header header;
	__u8                      state; /* 0 = down, 1 = up */
};

struct venet_event_txc {
	struct venet_event_header header;
	__u32                     txqid;
	aligned_u64               cookie;
};

struct venet_l4ro_query {
	__u32 flags;
	__u32 dpid;    /* descriptor pool-id */
	__u32 pqid;    /* page queue-id */
	__u8  pad[20];
};


#define VSG_DESC_SIZE(count) (sizeof(struct venet_sg) + \
			      sizeof(struct venet_iov) * ((count) - 1))

#define VENET_FUNC_LINKUP    0
#define VENET_FUNC_LINKDOWN  1
#define VENET_FUNC_MACQUERY  2
#define VENET_FUNC_NEGCAP    3 /* negotiate capabilities */
#define VENET_FUNC_FLUSHRX   4
#define VENET_FUNC_PMTDQUERY 5
#define VENET_FUNC_EVQQUERY  6
#define VENET_FUNC_L4ROQUERY  7

#endif /* _LINUX_VENET_H */
