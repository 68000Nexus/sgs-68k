/*	START NEW ARIX SCCS HEADER			*/
/*							*/
/*	@(#) h_sign.c: version 25.1 created on 12/2/91 at 18:33:10	*/
/*							*/
/*	Copyright (c) 1990 by Arix Corporation		*/
/*	All Rights Reserved				*/
/*							*/
#ident	"@(#)h_sign.c	25.1	12/2/91 Copyright (c) 1990 by Arix Corporation"
/*							*/
/*	END NEW ARIX SCCS HEADER			*/
/*							*/
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*	ATT:#ident	"libF77:h_sign.c	1.3"	*/
	
#ident	"@(#)libF77:h_sign.c	25.1"

short h_sign(a,b)
short *a, *b;
{
short x;
x = (*a >= 0 ? *a : - *a);
return( *b >= 0 ? x : -x);
}
