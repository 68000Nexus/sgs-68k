/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) temp.c: version 25.1 created on 12/2/91 at 14:57:02	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)temp.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*	ATT: #ident	"efl:temp.c	1.3"			*/

#ident	"@(#)efl:temp.c	25.1"

#include "defs"

ptr gentemp(t)
ptr t;
{
register ptr oldp;
register ptr p;
register ptr q;
int ttype;
ptr ttypep, tdim;

/* search the temporary list for a matching type */

ttype = t->vtype;
ttypep = t->vtypep;
tdim = t->vdim;

for(oldp = &tempvarlist ; p = oldp->nextp ; oldp = p)
	if( (q = p->datap) && (q->vtype == ttype) &&
	  (q->vtypep == ttypep) && eqdim(q->vdim,tdim) )
		{
		oldp->nextp = p->nextp;
		break;
		}

if(p == PNULL)
	{
	q = allexpblock();
	q->tag = TTEMP;
	q->subtype = t->subtype;
	q->vtype = ttype;
	q->vclass = t->vclass;
	q->vtypep = ( ttypep ? cpexpr(ttypep) : PNULL);
	q->vdim = tdim;
	mkftnp(q);	/* assign fortran types */

	p = mkchain(q, CHNULL);
	p->datap = q;
	}

p->nextp = thisexec->temps;
thisexec->temps = p;

return( cpexpr(q) );
/* need a copy of the block for the temporary list and another for use */
}


ptr gent(t,tp)  /* make a temporary of type t, typepointer tp */
int t;
ptr tp;
{
static struct varblock model;

model.vtype = t;
model.vtypep = tp;

return( gentemp(&model) );
}
