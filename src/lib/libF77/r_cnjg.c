/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) r_cnjg.c: version 25.1 created on 12/2/91 at 18:35:19	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)r_cnjg.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libF77:r_cnjg.c	1.3"	*/
	
#ident	"@(#)libF77:r_cnjg.c	25.1"

#include "complex"

r_cnjg(r, z)
complex *r, *z;
{
r->real = z->real;
r->imag = - z->imag;
}
