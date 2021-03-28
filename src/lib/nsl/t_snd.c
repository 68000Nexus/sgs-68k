/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) t_snd.c: version 25.1 created on 12/2/91 at 20:18:25	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)t_snd.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libnsl:nsl/t_snd.c	1.4.1.1"	*/

#ident	"@(#)lib/nsl:t_snd.c	25.1"

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


t_snd(fd, buf, nbytes, flags)
int fd;
register char *buf;
unsigned nbytes;
int flags;
{
	struct strbuf ctlbuf, databuf;
	struct T_data_req *datareq;
	int flg = 0;
	unsigned tmpcnt, tmp;
	char *tmpbuf;
	register struct _ti_user *tiptr;

	if ((tiptr = _t_checkfd(fd)) == NULL)
		return(-1);

	if (tiptr->ti_servtype == T_CLTS) {
		t_errno = TNOTSUPPORT;
		return(-1);
	}


	datareq = (struct T_data_req *)tiptr->ti_ctlbuf;
	if (flags&T_EXPEDITED) {
		datareq->PRIM_type = T_EXDATA_REQ;
	} else
		datareq->PRIM_type = T_DATA_REQ;


	ctlbuf.maxlen = sizeof(struct T_data_req);
	ctlbuf.len = sizeof(struct T_data_req);
	ctlbuf.buf = tiptr->ti_ctlbuf;
	tmp = nbytes;
	tmpbuf = buf;

	while (tmp) {
		if ((tmpcnt = tmp) > tiptr->ti_maxpsz) {
			datareq->MORE_flag = 1;
			tmpcnt = tiptr->ti_maxpsz;
		} else {
			if (flags&T_MORE)
				datareq->MORE_flag = 1;
			else
				datareq->MORE_flag = 0;
		}

		databuf.maxlen = tmpcnt;
		databuf.len = tmpcnt;
		databuf.buf = tmpbuf;
	
		if (putmsg(fd, &ctlbuf, &databuf, flg) < 0) {
			if (nbytes == tmp) {
				if (errno == EAGAIN)
					t_errno = TFLOW;
				else
					t_errno = TSYSERR;
				return(-1);
			} else
				tiptr->ti_state = TLI_NEXTSTATE(T_SND, tiptr->ti_state);
				return(nbytes - tmp);
		}
		tmp = tmp - tmpcnt;
		tmpbuf = tmpbuf + tmpcnt;
	}

	tiptr->ti_state = TLI_NEXTSTATE(T_SND, tiptr->ti_state);
	return(nbytes - tmp);
}
