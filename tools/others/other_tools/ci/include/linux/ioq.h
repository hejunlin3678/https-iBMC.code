/*
 * Copyright 2009 Novell.  All Rights Reserved.
 *
 * IOQ is a generic shared-memory, lockless queue mechanism. It can be used
 * in a variety of ways, though its intended purpose is to become the
 * asynchronous communication path for virtual-bus drivers.
 *
 * The following are a list of key design points:
 *
 * #) All shared-memory is always allocated on explicitly one side of the
 *    link.  This typically would be the guest side in a VM/VMM scenario.
 * #) Each IOQ has the concept of "north" and "south" locales, where
 *    north denotes the memory-owner side (e.g. guest).
 * #) An IOQ is manipulated using an iterator idiom.
 * #) Provides a bi-directional signaling/notification infrastructure on
 *    a per-queue basis, which includes an event mitigation strategy
 *    to reduce boundary switching.
 * #) The signaling path is abstracted so that various technologies and
 *    topologies can define their own specific implementation while sharing
 *    the basic structures and code.
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

#ifndef _LINUX_IOQ_H
#define _LINUX_IOQ_H

#include <linux/types.h>
#include <linux/shm_signal.h>

/*
 *---------
 * The following structures represent data that is shared across boundaries
 * which may be quite disparate from one another (e.g. Windows vs Linux,
 * 32 vs 64 bit, etc).  Therefore, care has been taken to make sure they
 * present data in a manner that is independent of the environment.
 *-----------
 */
struct ioq_ring_desc {
	aligned_u64           cookie; /* for arbitrary use by north-side */
	aligned_le64          ptr;
	aligned_le64          len;
	__u8                  valid;
	__u8                  sown; /* South owned = 1, North owned = 0 */
};

#define IOQ_RING_MAGIC cpu_to_le32(0x47fa2fe4)
#define IOQ_RING_VER   cpu_to_le32(4)

struct ioq_ring_idx {
	__le32                head;    /* 0 based index to head of ptr array */
	__le32                tail;    /* 0 based index to tail of ptr array */
	__u8                  full;
};

enum ioq_locality {
	ioq_locality_north,
	ioq_locality_south,
};

struct ioq_ring_head {
	__le32                 magic;
	__le32                 ver;
	struct shm_signal_desc signal;
	struct ioq_ring_idx    idx[2];
	__le32                 count;
	struct ioq_ring_desc   ring[1]; /* "count" elements will be allocated */
};

#define IOQ_HEAD_DESC_SIZE(count) \
    (sizeof(struct ioq_ring_head) + sizeof(struct ioq_ring_desc) * (count - 1))

/* --- END SHARED STRUCTURES --- */


#endif /* _LINUX_IOQ_H */
