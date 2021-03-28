#ifndef SYS_STAT_H
#define SYS_STAT_H

/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) stat.h: version 24.1 created on 10/28/91 at 18:38:24	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)stat.h	24.1	10/28/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/

/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"kern-port:sys/stat.h	10.7"		*/

/*
 * stat structure, used by stat(2) and fstat(2)
 */
struct	stat {
	dev_t	st_dev;
	ushort	st_ino;
	ushort 	st_mode;
	short  	st_nlink;
	ushort 	st_uid;
	ushort 	st_gid;
	dev_t	st_rdev;
	off_t	st_size;
	time_t	st_atime;
	time_t	st_mtime;
	time_t	st_ctime;
};

#define	S_IFMT	0170000		/* type of file */
#define		S_IFDIR	0040000	/* directory */
#define		S_IFCHR	0020000	/* character special */
#define		S_IFBLK	0060000	/* block special */
#define		S_IFREG	0100000	/* regular */
#define		S_IFIFO	0010000	/* fifo */
#define		S_IFACL	0050000	/* access control list */
#define		S_IFLNK	0120000	/* symbolic link */
#define	S_ISUID	04000		/* set user id on execution */
#define	S_ISGID	02000		/* set group id on execution */
#define	S_ISVTX	01000		/* save swapped text even after use */
#define	S_IREAD	00400		/* read permission, owner */
#define	S_IWRITE	00200	/* write permission, owner */
#define	S_IEXEC	00100		/* execute/search permission, owner */
#define	S_ENFMT	S_ISGID		/* record locking enforcement flag */
#define	S_IRWXU	00700		/* read, write, execute: owner */
#define	S_IRUSR	00400		/* read permission: owner */
#define	S_IWUSR	00200		/* write permission: owner */
#define	S_IXUSR	00100		/* execute permission: owner */
#define	S_IRWXG	00070		/* read, write, execute: group */
#define	S_IRGRP	00040		/* read permission: group */
#define	S_IWGRP	00020		/* write permission: group */
#define	S_IXGRP	00010		/* execute permission: group */
#define	S_IRWXO	00007		/* read, write, execute: other */
#define	S_IROTH	00004		/* read permission: other */
#define	S_IWOTH	00002		/* write permission: other */
#define	S_IXOTH	00001		/* execute permission: other */

/* macros for xopen */
#define S_ISDIR(X) ((X&S_IFMT) == S_IFDIR)
#define S_ISCHR(X) ((X&S_IFMT) == S_IFCHR)
#define S_ISBLK(X) ((X&S_IFMT) == S_IFBLK)
#define S_ISREG(X) ((X&S_IFMT) == S_IFREG)
#define S_ISIFO(X) ((X&S_IFMT) == S_IFIFO)
#ifdef _POSIX_SOURCE
#define S_ISFIFO(X) ((X&S_IFMT) == S_IFIFO)
#endif

#endif /* SYS_STAT_H */
