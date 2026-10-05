// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2017 Oracle.  All Rights Reserved.
 *
 * Author: Darrick J. Wong <darrick.wong@oracle.com>
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/include/linux/minix_fs.h
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#ifndef DRIVERS_EXT4_H
#define DRIVERS_EXT4_H

#include <linux/fs.h>
#include <sys/types.h>

typedef unsigned int ext4_group_t, ext4_fsblk_t, ext4_lblk_t;

enum criteria {
	/*
	 * Used when number of blocks needed is a power of 2. This
	 * doesn't trigger any disk IO except prefetch and is the
	 * fastest criteria.
	 */
	CR_POWER2_ALIGNED,

	/*
	 * Tries to lookup in-memory data structures to find the most
	 * suitable group that satisfies goal request. No disk IO
	 * except block prefetch.
	 */
	CR_GOAL_LEN_FAST,

        /*
	 * Same as CR_GOAL_LEN_FAST but is allowed to reduce the goal
         * length to the best available length for faster allocation.
	 */
	CR_BEST_AVAIL_LEN,

	/*
	 * Reads each block group sequentially, performing disk IO if
	 * necessary, to find suitable block group. Tries to
	 * allocate goal length but might trim the request if nothing
	 * is found after enough tries.
	 */
	CR_GOAL_LEN_SLOW,

	/*
	 * Finds the first free set of blocks and allocates
	 * those. This is only used in rare cases when
	 * CR_GOAL_LEN_SLOW also fails to allocate anything.
	 */
	CR_ANY_FREE,

	/*
	 * Number of criterias defined.
	 */
	EXT4_MB_NUM_CRS
};

struct ext4_allocation_request {
	/* target inode for block we're allocating */
	struct inode *inode;
	/* how many blocks we want to allocate */
	unsigned int len;
	/* logical block in target inode */
	ext4_lblk_t logical;
	/* the closest logical allocated block to the left */
	ext4_lblk_t lleft;
	/* the closest logical allocated block to the right */
	ext4_lblk_t lright;
	/* phys. target (a hint) */
	ext4_fsblk_t goal;
	/* phys. block for the closest logical allocated block to the left */
	ext4_fsblk_t pleft;
	/* phys. block for the closest logical allocated block to the right */
	ext4_fsblk_t pright;
	/* flags. see above EXT4_MB_HINT_* */
	unsigned int flags;
};

struct ext4_fsmap {
	struct list_head	fmr_list;
	dev_t		fmr_device;	/* device id */
	uint32_t	fmr_flags;	/* mapping flags */
	uint64_t	fmr_physical;	/* device offset of segment */
	uint64_t	fmr_owner;	/* owner id */
	uint64_t	fmr_length;	/* length of segment, blocks */
};

struct ext4_fsmap_head {
	uint32_t	fmh_iflags;	/* control flags */
	uint32_t	fmh_oflags;	/* output flags */
	unsigned int	fmh_count;	/* # of entries in array incl. input */
	unsigned int	fmh_entries;	/* # of entries filled in (output). */

	struct ext4_fsmap fmh_keys[2];	/* low and high keys */
};

struct ext4_device {
 dev_t    device;
 uint32_t    flag;
 ext4_fsmap_head    core;
};

typedef struct ext4_io_end {
	struct list_head	list;		/* per-file finished IO list */
	handle_t		*handle;	/* handle reserved for extent
						 * conversion */
	struct inode		*inode;		/* file being written to */
	struct bio		*bio;		/* Linked list of completed
						 * bios covering the extent */
	unsigned int		flag;		/* unwritten or not */
	refcount_t		count;		/* reference counter */
	struct list_head	list_vec;	/* list of ext4_io_end_vec */
} ext4_io_end_t;

#endif
