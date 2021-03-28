/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) t_sndrel.c: version 25.1 created on 12/2/91 at 20:18:32	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)t_sndrel.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT;#ident	"libnsl:nsl/t_sndrel.c	1.3.1.1"	*/

#ident	"@(#)lib/nsl:t_sndrel.c	25.1"

#include "sys/param.h"
#include "sys/types.h"
#include "sys/errno.h"
#include "sys/stream.h"
#include "sys/stropts.h"
#include "sys/tihdr.h"
#include "sys/timod.h"
#include "sys/tiuser.h"
#include "_import.h"


extern int t_errno;
extern int errno;
extern struct _ti_user *_t_checkfd();
extern int putmsg();


t_sndrel(fd)
int fd;
{
	struct T_ordrel_req orreq;
	struct strbuf ctlbuf;
	register struct _ti_user *tiptr;


	if ((tiptr = _t_checkfd(fd)) == NULL)
		return(-1);

	if (tiptr->ti_servtype != T_COTS_ORD) {
		t_errno = TNOTSUPPORT;
		return(-1);
	}


	orreq.PRIM_type = T_ORDREL_REQ;
	ctlbuf.maxlen = sizeof(struct T_ordrel_req);
	ctlbuf.len = sizeof(struct T_ordrel_req);
	ctlbuf.buf = (caddr_t)&orreq;

	if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
		if (errno == EAGAIN)
			t_errno = TFLOW;
		else
			t_errno = TSYSERR;
		return(-1);
	}

	tiptr->ti_state = TLI_NEXTSTATE(T_SNDREL, tiptr->ti_state);
	return(0);
}
