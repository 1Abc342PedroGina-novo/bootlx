/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _DRIVERS_ADFS_H
#define _DRIVERS_ADFS_H

#include <linux/buffer_head.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/adfs_fs.h>

#define ADFS_FREE_FRAG		 0
#define ADFS_BAD_FRAG		 1
#define ADFS_ROOT_FRAG		 2

#define ADFS_FILETYPE_NONE	((u16)~0)

static inline u16 adfs_filetype(u32 loadaddr)
{
	return (loadaddr & 0xfff00000) == 0xfff00000 ?
	       (loadaddr >> 8) & 0xfff : ADFS_FILETYPE_NONE;
}

struct adfs_inode_info {
	loff_t		mmu_private;
	__u32		parent_id;	/* parent indirect disc address	*/
	__u32		indaddr;	/* object indirect disc address	*/
	__u32		loadaddr;	/* RISC OS load address		*/
	__u32		execaddr;	/* RISC OS exec address		*/
	unsigned int	attr;		/* RISC OS permissions		*/
	struct inode vfs_inode;
};

struct adfs_sb_info {
	union { struct {
		struct adfs_discmap *s_map;	/* bh list containing map */
		const struct adfs_dir_ops *s_dir; /* directory operations */
		};
		struct rcu_head rcu;	/* used only at shutdown time	 */
	};
	kuid_t		s_uid;		/* owner uid */
	kgid_t		s_gid;		/* owner gid */
	umode_t		s_owner_mask;	/* ADFS owner perm -> unix perm */
	umode_t		s_other_mask;	/* ADFS other perm -> unix perm	*/
	int		s_ftsuffix;	/* ,xyz hex filetype suffix option */

	__u32		s_ids_per_zone;	/* max. no ids in one zone */
	__u32		s_idlen;	/* length of ID in map */
	__u32		s_map_size;	/* sector size of a map	*/
	signed int	s_map2blk;	/* shift left by this for map->sector*/
	unsigned int	s_log2sharesize;/* log2 share size */
	unsigned int	s_namelen;	/* maximum number of characters in name	 */
};


struct object_info {
	__u32		parent_id;		/* parent object id	*/
	__u32		indaddr;		/* indirect disc addr	*/
	__u32		loadaddr;		/* load address		*/
	__u32		execaddr;		/* execution address	*/
	__u32		size;			/* size			*/
	__u8		attr;			/* RISC OS attributes	*/
	unsigned int	name_len;		/* name length		*/
	char		name[ADFS_MAX_NAME_LEN];/* file name		*/
};

struct adfs_dir_ops {
	int	(*read)(struct super_block *sb, unsigned int indaddr,
			unsigned int size, struct adfs_dir *dir);
	int	(*iterate)(struct adfs_dir *dir, struct dir_context *ctx);
	int	(*setpos)(struct adfs_dir *dir, unsigned int fpos);
	int	(*getnext)(struct adfs_dir *dir, struct object_info *obj);
	int	(*update)(struct adfs_dir *dir, struct object_info *obj);
	int	(*create)(struct adfs_dir *dir, struct object_info *obj);
	int	(*remove)(struct adfs_dir *dir, struct object_info *obj);
	int	(*commit)(struct adfs_dir *dir);
};

struct adfs_discmap {
	struct buffer_head	*dm_bh;
	__u32			dm_startblk;
	unsigned int		dm_startbit;
	unsigned int		dm_endbit;
};

#endif
