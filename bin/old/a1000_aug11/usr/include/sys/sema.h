#ifndef SYS_SEMA_H
#define SYS_SEMA_H

/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) sema.h: version 2.1 created on 5/22/89 at 19:10:24	*/
/*							*/
/*	Copyright (c) 1988 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)sema.h	2.1	5/22/89 Copyright (c) 1988 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"kern-port:sys/sema.h	10.3"		*/

#ident	"@(#)uts/head/sys:sema.h	2.1"

/*
 * defines for semaphore mapping
 */

typedef int	sema_t;

#define ALLOC_LOCK(X)
#define EXTERN_LOCK(X)
#define INITLOCK(X,Y)
#define SPSEMA(X)
#define SVSEMA(X)
#define	PSEMA(X,Y)
#define	VSEMA(X,Y)

#define appsema(a,b)	1
#define apvsema(a,b)	1
#define psema(a,b)	sleep(a,b)
#define vsema(a,b)	wakeup(a)
#define initsema(a,b)	*a = b
#define initlock(a,b)	*a = b
#define cvsema(a)	wakeup(a)
#define splrf()		spl1()
#define ptob(x)		x

#endif /* SYS_SEMA_H */
