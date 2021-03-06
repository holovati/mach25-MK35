/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log:	dir.h,v $
 * Revision 2.8  89/05/30  10:42:42  rvb
 * 	Uniformed mips to the CS environment.
 * 	[89/05/20            af]
 * 
 * Revision 2.7  89/05/11  14:57:39  gm0w
 * 	Updated to use dynamically allocated buffer for _dirdesc
 * 	struct.  Changed rewinddir from macro to routine.
 * 	[89/05/05            gm0w]
 * 
 * Revision 2.6  89/04/22  15:31:26  gm0w
 * 	Removed MACH_VFS code.
 * 	[89/04/14            gm0w]
 * 
 * Revision 2.5  89/03/09  22:03:09  rpd
 * 	More cleanup.
 * 
 * Revision 2.4  89/02/25  17:52:39  gm0w
 * 	Changed EXL dependencies depend on exl and removed include
 * 	of cputypes.h. Made d_fileno which was dependent on MACH_VFS
 * 	be unconditionally defined.
 * 	[89/02/13            mrt]
 * 
 * Revision 2.3  89/01/23  22:25:31  af
 * 	MIPS: More stuff in dirdesc
 * 	[89/01/10            af]
 * 	
 * 	Changes for I386: EXL has a 1024 DEV_BSIZE
 * 	[89/01/09            rvb]
 * 
 * Revision 2.2  89/01/18  01:14:43  jsb
 * 	Added vnode support.
 * 	[88/12/17  16:50:54  jsb]
 * 
 * 06-Jan-88  Jay Kistler (jjk) at Carnegie Mellon University
 *	Added declarations for __STDC__.
 *
 * 24-Jul-86  Michael Young (mwyoung) at Carnegie-Mellon University
 *	Prevent repeated inclusion.
 *
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dir.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_DIR_H_
#define _SYS_DIR_H_

#include <sys/types.h>
#include <sys/param.h>		/* for DEV_BSIZE */

/*
 * A directory consists of some number of blocks of DIRBLKSIZ
 * bytes, where DIRBLKSIZ is chosen such that it can be transferred
 * to disk in a single atomic operation (e.g. 512 bytes on most machines).
 *
 * Each DIRBLKSIZ byte block contains some number of directory entry
 * structures, which are of variable length.  Each directory entry has
 * a struct direct at the front of it, containing its inode number,
 * the length of the entry, and the length of the name contained in
 * the entry.  These are followed by the name padded to a 4 byte boundary
 * with null bytes.  All names are guaranteed null terminated.
 * The maximum length of a name in a directory is MAXNAMLEN.
 *
 * The macro DIRSIZ(dp) gives the amount of space required to represent
 * a directory entry.  Free space in a directory is represented by
 * entries which have dp->d_reclen > DIRSIZ(dp).  All DIRBLKSIZ bytes
 * in a directory block are claimed by the directory entries.  This
 * usually results in the last entry in a directory having a large
 * dp->d_reclen.  When entries are deleted from a directory, the
 * space is returned to the previous entry in the same directory
 * block by increasing its dp->d_reclen.  If the first entry of
 * a directory block is free, then its dp->d_ino is set to 0.
 * Entries other than the first in a directory do not normally have
 * dp->d_ino set to 0.
 */

#define DIRBLKSIZ	DEV_BSIZE
#define MAXNAMLEN	255

struct	direct {
	u_long	d_ino;			/* inode number of entry */
	u_short	d_reclen;		/* length of this record */
	u_short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name must be no longer than this */
};


/*
 * The DIRSIZ macro gives the minimum record length which will hold
 * the directory entry.  This requires the amount of space in struct direct
 * without the d_name field, plus enough space for the name with a terminating
 * null byte (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */

#define DIRSIZ(dp) \
    ((sizeof (struct direct) - (MAXNAMLEN+1)) + (((dp)->d_namlen+1 + 3) &~ 3))

#ifndef	KERNEL
/*
 * Definitions for library routines operating on directories.
 */
typedef struct _dirdesc {
	int	dd_fd;
	long	dd_loc;
	long	dd_size;
#if	CMU
	char	*dd_buf;
	int	dd_bufsiz;
#else	/* CMU */
	char	dd_buf[DIRBLKSIZ];
#endif	/* CMU */
} DIR;

#ifndef	NULL
#define NULL 0
#endif	NULL

#ifdef	__STDC__
extern	DIR *opendir(const char *);
extern	struct direct *readdir(DIR *);
extern	long telldir(DIR *);
extern	void seekdir(DIR *, long);
#if	CMU
extern	void rewinddir(DIR *);
#else	/* CMU */
#define rewinddir(dirp)	seekdir((dirp), (long)0)
#endif	/* CMU */
extern	void closedir(DIR *);
extern	int scandir(const char *, struct direct ***, int (*)(), int (*)());
#else	__STDC__
extern	DIR *opendir();
extern	struct direct *readdir();
extern	long telldir();
extern	void seekdir();
#if	CMU
extern	void rewinddir();
#else	/* CMU */
#define rewinddir(dirp)	seekdir((dirp), (long)0)
#endif	/* CMU */
extern	void closedir();
#endif	__STDC__
#endif	KERNEL

#ifdef	KERNEL
/*
 * Template for manipulating directories.
 * Should use struct direct's, but the name field
 * is MAXNAMLEN - 1, and this just won't do.
 */
struct dirtemplate {
	u_long	dot_ino;
	short	dot_reclen;
	short	dot_namlen;
	char	dot_name[4];		/* must be multiple of 4 */
	u_long	dotdot_ino;
	short	dotdot_reclen;
	short	dotdot_namlen;
	char	dotdot_name[4];		/* ditto */
};
#endif	KERNEL
#endif	_SYS_DIR_H_
