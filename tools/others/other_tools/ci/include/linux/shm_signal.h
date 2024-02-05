/*
 * Copyright 2009 Novell.  All Rights Reserved.
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

#ifndef _LINUX_SHM_SIGNAL_H
#define _LINUX_SHM_SIGNAL_H

#include <linux/types.h>

/*
 *---------
 * The following structures represent data that is shared across boundaries
 * which may be quite disparate from one another (e.g. Windows vs Linux,
 * 32 vs 64 bit, etc).  Therefore, care has been taken to make sure they
 * present data in a manner that is independent of the environment.
 *-----------
 */

#define SHM_SIGNAL_MAGIC 0x58fa39df
#define SHM_SIGNAL_VER   1

struct shm_signal_irq {
	__u8                  enabled;
	__u8                  pending;
	__u8                  dirty;
};

enum shm_signal_locality {
	shm_locality_north,
	shm_locality_south,
};

struct shm_signal_desc {
	__u32                 magic;
	__u32                 ver;
	struct shm_signal_irq irq[2];
};

/* --- END SHARED STRUCTURES --- */


#endif /* _LINUX_SHM_SIGNAL_H */
